#include "generate.h"

#include <algorithm>
#include <cassert>
#include <random>

#include "common.h"

namespace
{
/**
 * @brief randomNoiseLevel - возвращает случайное значение уровня шума.
 * @return уровень шума (в диапазоне от 0 до +/- 15%).
 */
double randomNoiseLevel()
{
    static const double kNoiseLevelMin = -0.15;
    static const double kNoiseLevelMax =  0.15;

    static std::default_random_engine generator;
    static std::uniform_real_distribution<double> distribution(kNoiseLevelMin, kNoiseLevelMax);

    return distribution(generator);
}

/**
 * @brief addNoise - добавляет к значению value случайный шум в диапазоне +/- 0-15% от maxValue.
 * @param value - исходное значение.
 * @param maxValue - максимальная амплитуда значения.
 * @return "зашумлённое" значение value.
 */
double addNoise(const double value, const double maxValue)
{
    const double noise = maxValue * randomNoiseLevel();

    return (value + noise);
}

}

const double SineBehaviour::kVolumeMin = 0.3;
const double SineBehaviour::kVolumeMax = 3.0;

const CompositeSignal generate(const size_t signalLength,
                               const std::vector<SineSignal>& baseSignals,
                               bool noiseEnabled)
{
    assert(signalLength > 0);

    unused(baseSignals);

    const double kDefaultValue = 0.0;
    CompositeSignal result(signalLength, kDefaultValue);

    // TODO: impl

    if (noiseEnabled)
    {
        const auto minmax = std::minmax_element(std::begin(result),
                                                std::end(result));
        const double maxAmplitude = std::max(std::abs(*minmax.first),
                                             std::abs(*minmax.second));
        for (double& each : result)
        {
            each = ::addNoise(each, maxAmplitude);
        }
    }

    return result;
}
