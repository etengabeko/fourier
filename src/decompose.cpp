#include "decompose.h"

#include <algorithm>
#include <numeric>

#include "common.h"
#include "dft.h"
#include "filter.h"
#include "wave.h"

namespace
{

size_t windowSizePeriods() { return 1; }

size_t windowSizeForFrequency(const double frequency)
{
    return (windowSizePeriods() * frequencyToPeriod(frequency));
}

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
                                         const double frequency)
{
    const size_t kWindowSize = windowSizeForFrequency(frequency);

    std::vector<WindowBounds> result;

    if (signal.size() > kWindowSize)
    {
        result.reserve(signal.size()- kWindowSize + 1);

        auto it = signal.cbegin() + kWindowSize,
             end = signal.cend();
        while (it != end)
        {
            result.emplace_back((it - kWindowSize), it);
            ++it;
        }
    }
    else
    {
        result.emplace_back(signal.cbegin(), signal.cend());
    }

    return result;
}

}

WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies,
                            std::function<void(const std::string&,const std::vector<std::string>&,const size_t,const std::vector<std::vector<double>>&)> writeFunc)
{
    WaveDecomposition result;

    std::vector<std::string> columnTitles;
    std::vector<std::vector<double>> columnValues;
    columnTitles.reserve(frequencies.size());
    columnValues.reserve(frequencies.size());
    size_t length = 0;

    for (const double& eachFrequency : frequencies)
    {
        static size_t index = 0;

        size_t kWindowSize = ::windowSizeForFrequency(eachFrequency);
        const size_t coefWindowExpanding = signal.size() / kWindowSize;
        kWindowSize *= coefWindowExpanding;

        const std::vector<WindowBounds> windowsBounds = splitToWindows(signal, eachFrequency);

        columnTitles.push_back("filtered #" + std::to_string(++index));
        std::vector<double>& eachFiltered = *(columnValues.insert(columnValues.end(), std::vector<double>()));
        eachFiltered.reserve(windowsBounds.size());

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
            eachFiltered.push_back(coefWindowExpanding * modulus(frequencyValue));
        }

        length = std::max(length, eachFiltered.size());
    }

    writeFunc("filtered.csv", columnTitles, length, columnValues);

    return result;
}
