#include "common.h"
#include "decompose.h"
#include "dft.h"
#include "generate.h"
#include "filter.h"
#include "logger.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace
{

/**
 * @brief writeValuesToCsv - записывает значения values в csv-файл с именем fileName.
 * @param fileName - имя выходного файла.
 * @param titles - список заголовков столбцов данных.
 * @param linesCount - количество записываемых строк данных.
 * @param columns - список данных для записи (по столбцам).
 */
void writeValuesToCsv(const std::string& fileName,
                      const std::vector<std::string>& titles,
                      const size_t linesCount,
                      const std::vector<std::vector<double>>& columns)
{
    std::ofstream out(fileName);
    if (!out.good())
    {
        Logger::error(strerror(errno));
        return;
    }

    bool isFirstColumn = true;
    for (const std::string& eachTitle : titles)
    {
        if (!isFirstColumn)
            out << ", ";
        else
            isFirstColumn = false;

        out << eachTitle;
    }
    out << std::endl;

    for (size_t i = 0; i < linesCount; ++i)
    {
        if (!out)
        {
            Logger::error(strerror(errno));
            return;
        }

        isFirstColumn = true;
        for (const std::vector<double>& eachColumn : columns)
        {
            if (!isFirstColumn)
                out << ", ";
            else
                isFirstColumn = false;

            out << (eachColumn.size() > i ? std::to_string(eachColumn.at(i)) : "");
        }
        out << std::endl;
    }

    Logger::info("Writed " + fileName);
}

/**
 * @brief baseSignalValues - возвращает набор значений амплитуды базового сигнала signal.
 * @param signal - сигнал для записи.
 * @param signalEnables [optional] - набор значений активности сигнала (вкл/выкл сигнал):
 *        1.0 если сигнал включен (т.е. его значение участвует в результирующем сигнале), иначе 0.0;
 */
const std::vector<double> baseSignalValues(const SineSignal& signal,
                                           std::vector<double>* signalEnables = nullptr)
{
    enum class SignalState
    {
        Off = 0,
        On = 1
    };

    const size_t kLength = signal.behaviour.size();

    if (signalEnables != nullptr)
    {
        signalEnables->clear();
        signalEnables->reserve(kLength);
    }

    std::vector<double> result;
    result.reserve(kLength);

    for (size_t index = 0; index < kLength; ++index)
    {
        if (signalEnables != nullptr)
        {
            signalEnables->push_back(static_cast<double>(signal.behaviour.at(index).enabled ? SignalState::On : SignalState::Off));
        }
        result.push_back(sineSignalValue(signal, index));
    }

    return result;
}

/**
 * @brief makeBaseSignals - возвращает набор базовых синусоидальных сигналов, из которых складывается результирующий сигнал.
 * @param signalLength - длина результирующего сигнала (количество временнЫх отсчётов (дискретов))
 * @param frequencies - список базовых частот.
 * @return список базовых сигналов с их параметрами.
 *
 * @note Метод получения базовых сигналов можно автоматизировать. Однако в данном случае
 *       количество базовых частот и их характеристики будут заранее определены.
 */
const std::vector<SineSignal> makeBaseSignals(const size_t signalLength,
                                              std::vector<double>& frequencies)
{

    const size_t kFrequenciesCount = 3; //!< Количество базовых частот, из которых складывается результирующий сигнал.
    frequencies.reserve(kFrequenciesCount);

    std::vector<SineSignal> result(kFrequenciesCount);

    // Первая составляющая сложного сигнала:
    SineSignal& first = result[0];
    first.sine.freqFactor = 5.0;
    first.sine.startPhase = M_PI_2;
    first.behaviour.resize(signalLength);
    size_t period = frequencyToPeriod(first.sine.freqFactor);
    auto itFirst = std::begin(first.behaviour);
    auto itLast = (signalLength >= 10 * period) ? std::begin(first.behaviour) + (10 * period)
                                                : std::end(first.behaviour);
    auto itEnd = std::end(first.behaviour);
    double volume = 0.5;
    do
    {
        std::fill(itFirst, itLast, SineBehaviour{ volume, true });
        volume += 0.5;
        if (volume > SineBehaviour::kVolumeMax)
        {
            volume = 0.5;
        }
        itFirst = (std::distance(itFirst, itEnd) > static_cast<int>(20 * period)) ? (itFirst + (20 * period))
                                                                                  : itLast;
        itLast = (std::distance(itLast, itEnd) > static_cast<int>(20 * period)) ? (itLast + (20 * period))
                                                                                : itEnd;
    } while (itLast != itEnd);

    // Вторая составляющая сложного сигнала:
    SineSignal& second = result[1];
    second.sine.freqFactor = 2.0;
    second.sine.startPhase = -M_PI_4;
    second.behaviour.resize(signalLength);
    period = frequencyToPeriod(second.sine.freqFactor);
    itFirst = (signalLength >= 5 * period) ? std::begin(second.behaviour) + (5 * period)
                                           : std::begin(second.behaviour);
    itLast = (signalLength >= 15 * period) ? std::begin(second.behaviour) + (15 * period)
                                           : std::end(second.behaviour);
    itEnd = std::end(second.behaviour);
    volume = SineBehaviour::kVolumeMax;
    do
    {
        std::fill(itFirst, itLast, SineBehaviour{ volume, true });
        volume -= 2.0 * SineBehaviour::kVolumeMin;
        if (volume < SineBehaviour::kVolumeMin)
        {
            volume = SineBehaviour::kVolumeMax;
        }
        itFirst = (std::distance(itFirst, itEnd) > static_cast<int>(15 * period)) ? (itFirst + (15 * period))
                                                                                  : itLast;
        itLast = (std::distance(itLast, itEnd) > static_cast<int>(15 * period)) ? (itLast + (15 * period))
                                                                                : itEnd;
    } while (itLast != itEnd);

    // Третья составляющая сложного сигнала:
    SineSignal& third = result[2];
    third.sine.freqFactor = 10.0;
    third.sine.startPhase = 0.0;
    third.behaviour.resize(signalLength);
    std::fill(std::begin(third.behaviour),
              std::end(third.behaviour),
              SineBehaviour{SineBehaviour::kVolumeMin, true});

    frequencies.push_back(first.sine.freqFactor);
    frequencies.push_back(second.sine.freqFactor);
    frequencies.push_back(third.sine.freqFactor);

    return result;
}

}

/**
 * @brief CompositeSignal - результирующий сигнал - набор дискретных значений,
 *        полученных суммированием синусоидальных базовых сигналов.
 */
using CompositeSignal = std::vector<double>;

/**
 * @brief SignalSpectrum - спектр сигнала - набор дискретных значений,
 *        характеризующих сигнал в частотной области.
 */
using SignalSpectrum = std::vector<std::complex<double>>;

int main(int argc, char* argv[])
{
    unused(argc);
    unused(argv);

    // Параметры исследования:
    const size_t kSignalLength = 1000; //!< Длина исследуемых отрезков сигналов (в дискретах).
    const bool kNoiseEnabled = true;   //!< Добавлять ли шум при генерации результирующего сигнала?

    // Создание набора базовых сигналов:
    std::vector<double> frequencies;
    const std::vector<SineSignal> baseSignals = makeBaseSignals(kSignalLength, frequencies);

    // Генерация результирующего сигнала из набора базовых:
    CompositeSignal signal = generate(kSignalLength,
                                      baseSignals,
                                      kNoiseEnabled);

    // Необязательный блок. Нужен лишь для сохранения полученных значений в csv-файлы (например, для построения графиков).
    {
        // Вычисление спектра результирующего сигнала:
        SignalSpectrum spectrum = fourier::dft(signal);

        // Восстановление исходного сигнала по его спектру:
        CompositeSignal repaired = fourier::inverseDft(spectrum);

        // Восстановление гармоник, соответствующих базовым частотам:
        std::vector<std::vector<double>> baseHarmonics;
        baseHarmonics.reserve(baseSignals.size());
        std::vector<std::string> titles;
        titles.reserve(baseSignals.size());
        for (const SineSignal& each : baseSignals)
        {
            static size_t index = 0;
            titles.push_back("harmonic #" + std::to_string(++index));
            baseHarmonics.push_back(fourier::inverseDft(spectrum,
                                                        frequencyToIndex(each.sine.freqFactor, kSignalLength)));
        }
        ::writeValuesToCsv("base_harmonics.csv",
                           titles,
                           kSignalLength,
                           baseHarmonics);

        // Выделение базовых составляющих из сигнала и запись их в csv-файлы:
        for (const SineSignal& each : baseSignals)
        {
            static size_t index = 0;
            const std::string fileName = "base_signal_#" + std::to_string(++index) + ".csv";

            std::vector<double> eachEnables;
            CompositeSignal eachValues = ::baseSignalValues(each, &eachEnables);

            SignalSpectrum eachSpectrum;
            CompositeSignal eachRepaired = filterByFrequency(signal,
                                                             each.sine.freqFactor,
                                                             &eachSpectrum);
            ::writeValuesToCsv(fileName,
                               { "on/off", "original", "spectrum", "repaired" },
                               kSignalLength,
                               { eachEnables, eachValues, frequencyResponse(eachSpectrum), eachRepaired });
        }

        // Запись результирующего сигнала, его спектра и восстановленного сигнала в csv-файл:
        ::writeValuesToCsv("repaired-signal.csv",
                           { "original", "spectrum", "repaired" },
                           kSignalLength,
                           { signal, frequencyResponse(spectrum), repaired });
    }
/* TODO
    // Разложение результирующего сигнала на набор базовых:
    WaveDecomposition waves = decompose(signal, frequencies);

    // Логгирование результата разложения:
    Logger::info("Signal decomposition:");
    for (const Wave& each : waves)
    {
        static size_t index = 0;
        Logger::info("Wave #" + std::to_string(++index) + ":\n" + each.toString());
    }
*/
    return EXIT_SUCCESS;
}
