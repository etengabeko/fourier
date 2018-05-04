#include "decompose.h"

#include <algorithm>
#include <cmath>
#include <utility>

#include "common.h"
#include "dft.h"
#include "wave.h"

namespace
{

double noiseLevel() { return 0.15; }

int frequencyToIndex(const double frequency, const size_t length)
{
    return std::round(length / (2.0 * M_PI * frequency));
}

const std::vector<std::pair<size_t, size_t>> split(const std::vector<double>& signal)
{
    const double kThreshold = ::noiseLevel() * *(std::max_element(std::begin(signal),
                                                                  std::end(signal),
                                                                  [](const double& lhs, const double& rhs)
                                                                  {
                                                                      return std::abs(lhs) < std::abs(rhs);
                                                                  }));
    std::vector<std::pair<size_t, size_t>> result;

    bool isFound = false;
    size_t start = 0;
    for (size_t i = 0, size = signal.size(); i < size; ++i)
    {
        if (std::abs(signal.at(i)) > kThreshold)
        {
            if (!isFound)
            {
                start = i;
                isFound = true;
            }
        }
        else
        {
            if (isFound)
            {
                result.emplace_back(start, i);
                start = 0;
                isFound = false;
            }
        }
    }

    return result;
}

WaveDecomposition split(const CompositeSignal& baseSine, const double frequency)
{
    const double kConfidence = -1.0;
    const size_t kThreshold = baseSine.size() * 0.01; // TODO

    WaveDecomposition result;
    Wave wave(frequency, kConfidence, 0, 0);

    const std::vector<std::pair<size_t, size_t>> wavesParts = ::split(baseSine);
    for (auto begin = std::begin(wavesParts), end = std::end(wavesParts), it = begin; it != end; ++it)
    {
        auto next = it + 1;

        if (it == begin)
        {
            wave.start_idx = it->first;
        }
        else if (next == end)
        {
            wave.length = it->second - wave.start_idx;
            result.push_back(wave);
        }
        else if ((next->first - it->second) > kThreshold)
        {
            wave.length = it->second - wave.start_idx;
            result.push_back(wave);

            wave.start_idx = next->first;
            wave.length = 0;
        }
    }

    return result;
}

}

WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies)
{
    using SignalSpectrum = std::vector<std::complex<double>>;

    WaveDecomposition result;
    result.reserve(frequencies.size());

    const SignalSpectrum spectrum = fourier::dft(signal);

    for (const double& each : frequencies)
    {
        const WaveDecomposition waves = ::split(fourier::inverse_dft(spectrum,
                                                                     ::frequencyToIndex(each, frequencies.size())),
                                                each);
        result.insert(result.end(),
                      std::begin(waves),
                      std::end(waves));
    }

    return result;
}
