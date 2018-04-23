#include "common.h"
#include "decompose.h"
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
 * @brief writeCsv - записывает значения values в csv-файл с именем fileName.
 * @param fileName - имя выходного файла.
 * @param title - заголовок столбца данных.
 * @param values - данные для записи.
 */
void writeCsv(const std::string& fileName,
              const std::string& title,
              const std::vector<double>& values)
{
    std::ofstream out(fileName);
    if (!out.good())
    {
        Logger::error(strerror(errno));
        return;
    }

    out << "id" << ", " << title << std::endl;
    for (size_t i = 0, sz = values.size(); i< sz; ++i)
    {
        if (!out)
        {
            Logger::error(strerror(errno));
            return;
        }
        out << (i + 1) << ", " << values.at(i) << std::endl;
    }

    Logger::info("Writed " + fileName);
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
    third.sine.freqFactor = 1.0;
    third.sine.startPhase = 0.0;
    third.behaviour.resize(signalLength);
    std::fill(std::begin(third.behaviour),
              std::end(third.behaviour),
              SineBehaviour{SineBehaviour::kVolumeMax, true});

    frequencies.push_back(first.sine.freqFactor);
    frequencies.push_back(second.sine.freqFactor);
    frequencies.push_back(third.sine.freqFactor);

    return result;
}

}

int main(int argc, char* argv[])
{
    unused(argc);
    unused(argv);

    const size_t kSignalLength = 1000;
    const bool kNoiseEnabled = false;
    std::vector<double> frequencies;
    const std::vector<SineSignal> baseSignals = makeBaseSignals(kSignalLength, frequencies);

    CompositeSignal signal = generate(kSignalLength,
                                      baseSignals,
                                      kNoiseEnabled);
    ::writeCsv("final_signal.csv", "Final signal", signal);

    WaveDecomposition waves = decompose(signal, frequencies);

    // TODO: log decomposition

    unused(waves);

    return EXIT_SUCCESS;
}
