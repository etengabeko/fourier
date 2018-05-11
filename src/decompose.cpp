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

/**
 * @brief splitToWindows - разделяет входную последовательность signal на окна длиной windowWidth.
 *        Каждое следующее окно смещается относительно предыдущего на значение offset (окна могут перекрываться).
 * @param signal - разделяемая последовательность значений.
 * @param windowWidth - ширина окна.
 * @param offset - смещение следующего окна от предыдущего.
 * @return набор окон.
 */
std::vector<WindowBounds> splitToWindows(const std::vector<double>& signal,
                                         const size_t windowWidth)
{

    std::vector<WindowBounds> result;

    if (signal.size() > windowWidth)
    {
        result.reserve(signal.size()- windowWidth + 1);

        auto it = signal.cbegin() + windowWidth,
             end = signal.cend();
        while (it != end)
        {
            result.emplace_back((it - windowWidth), it);
            ++it;
        }
    }
    else
    {
        result.emplace_back(signal.cbegin(), signal.cend());
    }

    return result;
}


/**
 * @brief meanValue - вычисляет среднее значение последовательности, заданной парой итераторов: [first, last).
 * @param first - начало анализируемой последовательности.
 * @param last - конец анализируемой последовательности.
 * @return среднее арифметическое последовательности значений.
 */
template <typename Iterator>
double meanValue(Iterator first, Iterator last)
{
    if (first == last)
    {
        return 0.0;
    }

    return (std::accumulate(first, last, 0.0) / static_cast<double>(std::distance(first, last)));
}

/**
 * @brief meanValue - вычисляет среднее значение values.
 * @param values - анализируемая последовательность значений.
 * @return среднее арифметическое значений values.
 */
double meanValue(const std::vector<double>& values)
{
    return meanValue(std::begin(values), std::end(values));
}

/**
 * @brief joinDecomposition - объединяет последовательно расположенные пары структур Wave набора decomposition,
 *        если промежуток между ними менее значения kMinimumWaveDurationPeriods.
 * @param decomposition - анализируемая последовательность структур.
 * @param isAnyJoined - флаг результата - были ли совершены какие-либо объединения?
 * @return последовательность объединённый структур Wave.
 */
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

    auto current = std::begin(decomposition),
         next = current + 1,
         end = std::end(decomposition);

    while (current != end && next != end)
    {
        const Wave& currentWave = *current;
        const Wave& nextWave = *next;

        if (   nextWave.start_idx < (currentWave.start_idx + currentWave.length)
            || (nextWave.start_idx - (currentWave.start_idx + currentWave.length)) < kMinimumLength)
        {
            Wave joined(currentWave);
            joined.length = nextWave.start_idx + nextWave.length - currentWave.start_idx;
            joined.confidence = meanValue({ currentWave.confidence, nextWave.confidence });

            result.push_back(joined);
            *isAnyJoined = true;
        }
        else
        {
            result.push_back(currentWave);
            result.push_back(nextWave);
        }

        current += 2;
        next += 2;
    }

    if (current != end)
    {
        Wave& lastWave = result.back();
        const Wave& currentWave = *current;

        if (   currentWave.start_idx < (lastWave.start_idx + lastWave.length)
            || (currentWave.start_idx - (lastWave.start_idx + lastWave.length)) < kMinimumLength)
        {
            lastWave.length = currentWave.start_idx + currentWave.length - lastWave.start_idx;
            lastWave.confidence = meanValue({ lastWave.confidence, currentWave.confidence });

            *isAnyJoined = true;
        }
    }

    return result;
}

/**
 * @brief decomposeByProbabilites - выделяет из временного распределения вероятности обнаружения базового сигнала в сложном
 *        структуры с параметрами, характеризующими обнаруженный базовый сигнал.
 * @param probabilities - временное распределение вероятности обнаружения сигнала с частотой frequency в сложном сигнале.
 * @param frequency - частота базового сигнала.
 * @return набор структур Wave, характеризующих наличие базового сигнала с частотой frequency в составе сложного сигнала.
 */
WaveDecomposition decomposeByProbabilites(const std::vector<double>& probabilities,
                                          const double frequency)
{
    const double kThreshold = 0.50;

    WaveDecomposition result;
    const std::vector<WindowBounds> windows = splitToWindows(probabilities, frequencyToPeriod(frequency));
    for (const WindowBounds& each : windows)
    {
        const double windowMeanValue = meanValue(each.lower, each.upper);
        if (windowMeanValue >= kThreshold)
        {
            result.emplace_back(frequency,
                                windowMeanValue,
                                std::distance(std::begin(probabilities), each.lower),
                                std::distance(each.lower, each.upper));
        }
    }

    volatile bool isContinue = true;
    while (isContinue)
    {
        result = joinDecomposition(result, const_cast<bool*>(&isContinue));
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
        ++index;
        Logger::trace("Decompose frequency " + std::to_string(index) + "/" + std::to_string(frequencies.size()) + ".");

        const size_t kWindowSize = frequencyToPeriod(eachFrequency);
        const size_t coefWindowExpanding = signal.size() / kWindowSize;
        const size_t expandedSize = kWindowSize * coefWindowExpanding;
        Logger::trace("Split to windows, window size = " + std::to_string(kWindowSize) + " discrets.");

        const std::vector<WindowBounds> windowsBounds = splitToWindows(signal, kWindowSize);
        Logger::trace("Windows count = " + std::to_string(windowsBounds.size()) + ".");

        columnTitles.push_back("probability #" + std::to_string(index));
        std::vector<double>& eachProbability = *(columnValues.insert(columnValues.end(), std::vector<double>()));
        eachProbability.reserve(windowsBounds.size());

        Logger::trace("Calculate signal probabilities in windows.");
        for (const auto& eachWindow : windowsBounds)
        {
            std::vector<double> eachSignal(eachWindow.lower, eachWindow.upper);
            if (eachSignal.size() < expandedSize)
            {
                eachSignal.resize(expandedSize);
            }

            std::vector<std::complex<double>> eachFilteredSpectrum;
            filterByFrequency(eachSignal, eachFrequency, &eachFilteredSpectrum);
            const std::complex<double> frequencyValue = eachFilteredSpectrum.at(frequencyToIndex(eachFrequency,
                                                                                                 eachFilteredSpectrum.size()));
            // Вычисленную амплитуду сигнала для данной частоты будем считать вероятностью обнаружения данной частоты на данном отрезке сложного сигнала.
            eachProbability.push_back(coefWindowExpanding * modulus(frequencyValue));
        }

        length = std::max(length, eachProbability.size());
    }

    writeValuesToCsv("base_probabilities.csv", columnTitles, length, columnValues);

    Logger::trace("Start probabilities analyzing.");
    WaveDecomposition result;
    for (size_t i = 0, size = frequencies.size(); i < size; ++i)
    {
        Logger::trace("Decompose for frequency #" + std::to_string(i+1) + ".");
        WaveDecomposition forEachFrequency = ::decomposeByProbabilites(columnValues.at(i),
                                                                       frequencies.at(i));
        result.insert(std::end(result),
                      std::begin(forEachFrequency), std::end(forEachFrequency));
    }

    for (Wave& each : result)
    {
        if (each.confidence > 1.0)
        {
            each.confidence = 1.0;
        }
    }

    return result;
}
