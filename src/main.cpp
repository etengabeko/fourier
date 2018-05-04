#include "common.h"
#include "decompose.h"
#include "dft.h"
#include "generate.h"
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
 * @brief writeSignalToCsv - записывает базовый сигнал в csv-файл с именем fileName.
 *        Параметры сигнала записываются в два столбца:
 *         1) on/off - 1.0 если сигнал включен (т.е. его значение участвует в результирующем сигнале), иначе 0.0;
 *         2) value - значение амплитуды сигнала.
 * @param fileName - имя выходного файла.
 * @param signal - сигнал для записи.
 */
void writeSignalToCsv(const std::string& fileName,
                      const SineSignal& signal)
{
    enum class SignalState
    {
        Off = 0,
        On = 1
    };

    const size_t size = signal.behaviour.size();

    std::vector<double> signalEnables;
    std::vector<double> signalValues;
    signalEnables.reserve(size);
    signalValues.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        signalEnables.push_back(static_cast<double>(signal.behaviour.at(i).enabled ? SignalState::On : SignalState::Off));
        signalValues.push_back(sineSignalValue(signal, i));
    }

    writeValuesToCsv(fileName,
                     { "On/Off", "Value" },
                     size,
                     { signalEnables, signalValues });
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

    SineSignal& first = result[0];
    first.sine.freqFactor = 5.0;
    first.sine.startPhase = M_PI_2;
    first.behaviour.resize(signalLength);
    std::fill(std::begin(first.behaviour) + 100,
              std::begin(first.behaviour) + 500,
              SineBehaviour{2.5, true});
    std::fill(std::begin(first.behaviour) + 900,
              std::end(first.behaviour),
              SineBehaviour{1.5, true});

    SineSignal& second = result[1];
    second.sine.freqFactor = 5.5;
    second.sine.startPhase = -M_PI_4;
    second.behaviour.resize(signalLength);
    std::fill(std::begin(second.behaviour),
              std::begin(second.behaviour) + 300,
              SineBehaviour{1.0, true});
    std::fill(std::begin(second.behaviour) + 400,
              std::begin(second.behaviour) + 600,
              SineBehaviour{2.0, true});
    std::fill(std::begin(second.behaviour) + 700,
              std::begin(second.behaviour) + 800,
              SineBehaviour{3.0, true});

    SineSignal& third = result[2];
    third.sine.freqFactor = 2.0;
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

int frequencyToIndex(const double frequency, const size_t length)
{
    return std::round(length / (2.0 * M_PI * frequency));
}

}

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
        using ComplexVector = std::vector<std::complex<double>>;

        // Вычисление спектра результирующего сигнала:
        ComplexVector spectrum = fourier::dft(signal);

        // Восстановление исходного сигнала по его спектру:
        CompositeSignal repaired = fourier::inverse_dft(spectrum);

//        for (const SineSignal& each : baseSignals)
//        {
//            static size_t index = 0;
//            Logger::debug(  "Harmonic #" + std::to_string(++index)
//                          + ": frequency = " + std::to_string(each.sine.freqFactor)
//                          + ", index = " + std::to_string(::frequencyToIndex(each.sine.freqFactor, kSignalLength))
//                          + ", value = " + std::to_string(fourier::modulus(spectrum.at(::frequencyToIndex(each.sine.freqFactor, kSignalLength)))));
//        }

        // Восстановление гармоник, соответствующих базовым частотам:
        for (const SineSignal& each : baseSignals)
        {
            static size_t index = 0;
            const std::string fileName = "repair_base_#" + std::to_string(++index) + ".csv";
            CompositeSignal harmonic = fourier::inverse_dft(spectrum, ::frequencyToIndex(each.sine.freqFactor, kSignalLength));
            ::writeValuesToCsv(fileName,
                               { "repair" },
                               kSignalLength,
                               { harmonic });
        }

        // Запись базовых сигналов в csv-файлы:
        for (const SineSignal& each : baseSignals)
        {
            static size_t index = 0;
            const std::string fileName = "base_signal_#" + std::to_string(++index) + ".csv";
            ::writeSignalToCsv(fileName, each);
        }

        // Запись результирующего сигнала, его спектра и восстановленного сигнала в csv-файл:
        ::writeValuesToCsv("repair-signal.csv",
                           { "original signal", "spectrum", "repair signal" },
                           kSignalLength,
                           { signal, fourier::frequencyResponse(spectrum), repaired });
    }

    // Разложение результирующего сигнала на набор базовых:
    WaveDecomposition waves = decompose(signal, frequencies);

    // Логгирование результата разложения:
    Logger::info("Signal decomposition:");
    for (const Wave& each : waves)
    {
        static size_t index = 0;
        Logger::info("Wave #" + std::to_string(++index) + ":\n" + each.toString());
    }

    return EXIT_SUCCESS;
}
