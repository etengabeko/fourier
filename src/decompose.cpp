#include "decompose.h"

#include <algorithm>
#include <cassert>
#include <numeric>

#include "commons.h"
#include "dft.h"
#include "filter.h"
#include "logger.h"
#include "wave.h"

namespace
{

struct WindowBounds
{
    std::vector<double>::const_iterator lower;
    std::vector<double>::const_iterator upper;

    WindowBounds() = default;
    WindowBounds(const std::vector<double>::const_iterator& first,
                 const std::vector<double>::const_iterator& last) :
        lower(first),
        upper(last)
    { }
};

std::vector<WindowBounds> splitToWindows(const std::vector<double>& signal,
                                         const double frequency,
                                         const size_t offset = 1)
{
    const size_t kWindowSize = frequencyToPeriod(frequency);

    std::vector<WindowBounds> result;

    if (signal.size() > kWindowSize)
    {
        result.reserve(signal.size()- kWindowSize + 1);

        auto it = signal.cbegin() + kWindowSize,
             end = signal.cend();
        while (it != end)
        {
            result.emplace_back((it - kWindowSize), it);
            it += offset;
        }
    }
    else
    {
        result.emplace_back(signal.cbegin(), signal.cend());
    }

    return result;
}

template <typename Iterator>
double meanValue(Iterator first, Iterator last)
{
    if (first == last)
    {
        return 0.0;
    }

    return (std::accumulate(first, last, 0.0) / static_cast<double>(std::distance(first, last)));
}

double meanValue(const std::vector<double>& values)
{
    return meanValue(std::begin(values), std::end(values));
}

WaveDecomposition joinDecomposition(const WaveDecomposition& decomposition, bool* isAnyJoined)
{
    assert(isAnyJoined != nullptr);
    *isAnyJoined = false;

    if (decomposition.size() <= 1)
    {
        return decomposition;
    }

    const size_t kMinimumLength = kMinimumWaveDurationPeriods * frequencyToPeriod(decomposition.front().frequency);

    WaveDecomposition result;
    result.reserve(decomposition.size());

    for (auto it = std::begin(decomposition), end = std::end(decomposition); it != (end - 1); ++it)
    {
        const Wave& current = *it;
        const Wave& next = *(it + 1);

        if (   next.start_idx < (current.start_idx + current.length)
            || (next.start_idx - (current.start_idx + current.length)) < kMinimumLength)
        {
            Wave joined(current);
            joined.length = next.start_idx + next.length - current.start_idx;
            joined.confidence = meanValue({ current.confidence, next.confidence });

            result.push_back(joined);
            *isAnyJoined = true;
        }
        else
        {
            result.push_back(current);
            if ((it + 2) == end)
            {
                result.push_back(next);
            }
        }
    }

    return result;
}

WaveDecomposition decomposeByProbabilites(const std::vector<double>& probabilities,
                                          const double frequency)
{
    const double kThreshold = 0.75;
    const size_t windowWidth = frequencyToPeriod(frequency);
    const size_t windowsAliasing = windowWidth / 2;

    WaveDecomposition result;
    const std::vector<WindowBounds> windows = splitToWindows(probabilities, frequency, (windowWidth - windowsAliasing));
    for (const WindowBounds& each : windows)
    {
        const double windowMeanValue = meanValue(each.lower, each.upper);
        if (windowMeanValue >= kThreshold)
        {
            result.emplace_back(frequency,
                                windowMeanValue,
                                std::distance(std::begin(probabilities), each.lower),
                                std::distance(each.upper, each.lower));
        }
    }

    bool isContinue = true;
    while (isContinue)
    {
        result = joinDecomposition(result, &isContinue);
    }

    return result;
}

}

WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies)
{
    std::vector<std::string> columnTitles;
    std::vector<std::vector<double>> columnValues;
    columnTitles.reserve(frequencies.size());
    columnValues.reserve(frequencies.size());
    size_t length = 0;

    for (const double& eachFrequency : frequencies)
    {
        static size_t index = 0;

        size_t kWindowSize = frequencyToPeriod(eachFrequency);
        const size_t coefWindowExpanding = signal.size() / kWindowSize;
        kWindowSize *= coefWindowExpanding;

        const std::vector<WindowBounds> windowsBounds = splitToWindows(signal, eachFrequency);

        columnTitles.push_back("filtered #" + std::to_string(++index));
        std::vector<double>& eachProbability = *(columnValues.insert(columnValues.end(), std::vector<double>()));
        eachProbability.reserve(windowsBounds.size());

        for (const auto& eachWindow : windowsBounds)
        {
            std::vector<double> eachSignal(eachWindow.lower, eachWindow.upper);
            if (eachSignal.size() < kWindowSize)
            {
                eachSignal.resize(kWindowSize);
            }

            std::vector<std::complex<double>> eachFilteredSpectrum;
            filterByFrequency(eachSignal, eachFrequency, &eachFilteredSpectrum);
            const std::complex<double> frequencyValue = eachFilteredSpectrum.at(frequencyToIndex(eachFrequency,
                                                                                                 eachFilteredSpectrum.size()));
            eachProbability.push_back(coefWindowExpanding * modulus(frequencyValue));
        }

        length = std::max(length, eachProbability.size());
    }

    writeValuesToCsv("filtered.csv", columnTitles, length, columnValues);

    WaveDecomposition result;

    for (size_t i = 0, size = frequencies.size(); i < size; ++i)
    {
        WaveDecomposition forEachFrequency = ::decomposeByProbabilites(columnValues.at(i),
                                                                       frequencies.at(i));
        result.insert(std::end(result),
                      std::begin(forEachFrequency), std::end(forEachFrequency));
    }

    return result;
}
