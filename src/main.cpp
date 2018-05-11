#include "commons.h"
#include "decompose.h"
#include "dft.h"
#include "filter.h"
#include "generate.h"
#include "logger.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{

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

    const size_t kFrequenciesCount = 4; //!< Количество базовых частот, из которых складывается результирующий сигнал.
    frequencies.reserve(kFrequenciesCount);

    std::vector<SineSignal> result(kFrequenciesCount);

    // Первая составляющая сложного сигнала:
    SineSignal& first = result[0];
    first.sine.freqFactor = 5.0;
    first.sine.startPhase = M_PI_2;
    first.behaviour.resize(signalLength);
    size_t period = frequencyToPeriod(first.sine.freqFactor);
    size_t offset = (signalLength > (period / 2)) ? (period / 2) : 0;

    auto itEnd = std::end(first.behaviour);
    auto itFirst = std::begin(first.behaviour) + offset;
    auto itLast = (signalLength >= (9 * period + offset)) ? std::begin(first.behaviour) + (9 * period + offset)
                                                          : itEnd;
    double volume = 0.5;
    do
    {
        std::fill(itFirst, itLast, SineBehaviour{ volume, true });
        volume += 0.5;
        if (volume > SineBehaviour::kVolumeMax)
        {
            volume = 0.5;
        }
        itFirst = (std::distance(itFirst, itEnd) > static_cast<int>(15 * period)) ? (itFirst + (15 * period))
                                                                                  : itLast;
        itLast = (std::distance(itLast, itEnd) > static_cast<int>(15 * period)) ? (itLast + (15 * period))
                                                                                : itEnd;
    } while (itLast != itEnd);

    // Вторая составляющая сложного сигнала:
    SineSignal& second = result[1];
    second.sine.freqFactor = 2.0;
    second.sine.startPhase = -M_PI_4;
    second.behaviour.resize(signalLength);
    period = frequencyToPeriod(second.sine.freqFactor);
    offset = (signalLength > (3 * period / 2)) ? (3 * period / 2) : 0;

    itEnd = std::end(second.behaviour);
    itFirst = std::begin(second.behaviour) + offset;
    itLast = (signalLength >= (15 * period / 2 + offset)) ? std::begin(second.behaviour) + (15 * period / 2 + offset)
                                                          : itEnd;
    volume = SineBehaviour::kVolumeMax;
    do
    {
        std::fill(itFirst, itLast, SineBehaviour{ volume, true });
        volume -= 2.0 * SineBehaviour::kVolumeMin;
        if (volume < SineBehaviour::kVolumeMin)
        {
            volume = SineBehaviour::kVolumeMax;
        }
        itFirst = (std::distance(itFirst, itEnd) > static_cast<int>(13 * period)) ? (itFirst + (13 * period))
                                                                                  : itLast;
        itLast = (std::distance(itLast, itEnd) > static_cast<int>(13 * period)) ? (itLast + (13 * period))
                                                                                : itEnd;
    } while (itLast != itEnd);

    // Третья составляющая сложного сигнала:
    SineSignal& third = result[2];
    third.sine.freqFactor = 10.0;
    third.sine.startPhase = M_PI / 6;
    third.behaviour.resize(signalLength);
    period = frequencyToPeriod(third.sine.freqFactor);
    offset = (signalLength > (period / 3)) ? (period / 3) : 0;

    itEnd = std::end(third.behaviour);
    itFirst = std::begin(third.behaviour) + offset;
    itLast = (signalLength >= (5 * period + offset)) ? std::begin(third.behaviour) + (5 * period + offset)
                                                     : itEnd;
    volume = SineBehaviour::kVolumeMax;
    do
    {
        std::fill(itFirst, itLast, SineBehaviour{ volume, true });

        itFirst = (std::distance(itFirst, itEnd) > static_cast<int>(10 * period)) ? (itFirst + (10 * period))
                                                                                  : itLast;
        itLast = (std::distance(itLast, itEnd) > static_cast<int>(10 * period)) ? (itLast + (10 * period))
                                                                                : itEnd;
    } while (itLast != itEnd);

    // Четвёртая составляющая сложного сигнала:
    SineSignal& fourth = result[3];
    fourth.sine.freqFactor = 5.5;
    fourth.sine.startPhase = 0.0;
    fourth.behaviour.resize(signalLength);
    period = frequencyToPeriod(fourth.sine.freqFactor);
    offset = 0;

    itFirst = std::begin(fourth.behaviour) + offset;
    itLast = std::end(fourth.behaviour);
    volume = SineBehaviour::kVolumeMin;
    std::fill(itFirst, itLast, SineBehaviour{ volume, true });

    frequencies.push_back(first.sine.freqFactor);
    frequencies.push_back(second.sine.freqFactor);
    frequencies.push_back(third.sine.freqFactor);
    frequencies.push_back(fourth.sine.freqFactor);

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
    Logger::info(  "Analyze signal length = " + std::to_string(kSignalLength)
                 + ", add noise = " + (kNoiseEnabled ? "True" : "False") + ".");

    // Создание набора базовых сигналов:
    Logger::trace("Generate base signals.");
    std::vector<double> frequencies;
    const std::vector<SineSignal> baseSignals = makeBaseSignals(kSignalLength, frequencies);

    // Генерация результирующего сигнала из набора базовых:
    Logger::trace("Generate composite signal.");
    CompositeSignal signal = generate(kSignalLength, baseSignals, kNoiseEnabled);

    // Необязательный блок. Нужен лишь для сохранения полученных значений в csv-файлы (например, для построения графиков).
    {
        // Вычисление спектра результирующего сигнала:
        Logger::trace("Calculating spectrum of composite signal.");
        SignalSpectrum spectrum = fourier::dft(signal);

        // Восстановление исходного сигнала по его спектру:
        Logger::trace("Repairing signal by its spectrum.");
        CompositeSignal repaired = fourier::inverseDft(spectrum);

        // Запись базовых составляющих сигнала в csv-файлы:
        Logger::trace("Writing csv files:");
        for (const SineSignal& each : baseSignals)
        {
            static size_t index = 0;
            const std::string fileName = "base_signal_#" + std::to_string(++index) + ".csv";

            std::vector<double> eachEnables;
            CompositeSignal eachValues = ::baseSignalValues(each, &eachEnables);
            SignalSpectrum eachSpectrum = fourier::dft(eachValues);

            writeValuesToCsv(fileName,
                             { "on/off", "original", "spectrum" },
                             kSignalLength,
                             { eachEnables, eachValues, frequencyResponse(eachSpectrum) });
        }

        // Запись результирующего сигнала, его спектра и восстановленного сигнала в csv-файл:
        writeValuesToCsv("repaired-signal.csv",
                         { "original", "spectrum", "repaired" },
                         kSignalLength,
                         { signal, frequencyResponse(spectrum), repaired });
    }

    // Разложение результирующего сигнала на набор базовых:
    Logger::trace("Start signal decomposition.");
    WaveDecomposition waves = decompose(signal, frequencies);
    Logger::trace("Decomposition finished.");

    // Логгирование результата разложения:
    Logger::info("Signal decomposition result:");
    for (const Wave& each : waves)
    {
        static size_t index = 0;
        Logger::info("Wave #" + std::to_string(++index) + ":\n" + each.toString());
    }

    return EXIT_SUCCESS;
}
