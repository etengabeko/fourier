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
                                         const size_t windowWidth,
                                         const size_t offset = 1)
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
            if (std::distance(it, end) >= static_cast<int>(offset))
            {
                it += offset;
            }
            else
            {
                result.emplace_back(it, end);
                break;
            }
        }
    }
    else
    {
        result.emplace_back(signal.cbegin(), signal.cend());
    }

    return result;
}

/**
 * @brief splitByThreshold - выделяет из входной последовательности signal окна,
 *        в которых все значения выше порогового значения threshold.
 * @param signal - анализируемая последовательность.
 * @param threshold - пороговое значение.
 * @return набор окон.
 */
std::vector<WindowBounds> splitByThreshold(const std::vector<double>& signal,
                                           const double threshold)
{
    std::vector<WindowBounds> result;

    auto end = std::end(signal);
    auto first = end;

    for (auto it = std::begin(signal); it != end; ++it)
    {
        if (*it >= threshold)
        {
            if (first == end)
            {
                first = it;
            }
        }
        else if (first != end)
        {
            result.emplace_back(first, it);
            first = end;
        }
    }
    if (first != end)
    {
        result.emplace_back(first, end);
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
 * @brief meanAverageSmooth - сглаживает входной ряд значений, используя метод скользящего среднего.
 * @param sequence - входная последовательность.
 * @param windowSize - размер окна для вычисления среднего.
 * @return сглаженная последовательность.
 */
const std::vector<double> meanAverageSmooth(const std::vector<double>& sequence,
                                            const size_t windowSize)
{
    if (sequence.size() < windowSize)
    {
        return sequence;
    }

    std::vector<double> result(sequence);
    auto currentOutput = std::begin(result) + windowSize / 2;

    auto first = std::begin(sequence);
    auto last = first + windowSize;
    auto end = std::end(sequence);
    while (last != end)
    {
        *(currentOutput++) = meanValue((first++), (last++));
    }

    return result;
}

/**
 * @brief joinDecomposition - объединяет последовательно расположенные окна WindowBounds набора decomposition,
 *        если промежуток между ними менее значения maxGap.
 * @param decomposition - анализируемая последовательность окон.
 * @param maxGap - максимальная длина промежутка между соседними окнами, при которой их можно объединить.
 * @param isAnyJoined - флаг результата - были ли совершены какие-либо объединения?
 * @return последовательность объединённых окон.
 */
std::vector<WindowBounds> joinDecomposition(const std::vector<WindowBounds>& decomposition,
                                            const size_t maxGap,
                                            bool* isAnyJoined)
{
    assert(isAnyJoined != nullptr);
    *isAnyJoined = false;

    if (decomposition.size() <= 1)
    {
        return decomposition;
    }

    std::vector<WindowBounds> result;
    result.reserve(decomposition.size());

    auto current = std::begin(decomposition),
         next = current + 1,
         end = std::end(decomposition);

    while (current != end && next != end)
    {
        const WindowBounds& currentWindow = *current;
        const WindowBounds& nextWindow = *next;

        if (static_cast<size_t>(std::distance(currentWindow.upper, nextWindow.lower)) <= maxGap)
        {
            result.emplace_back(currentWindow.lower, nextWindow.upper);
            *isAnyJoined = true;
        }
        else
        {
            result.push_back(currentWindow);
            result.push_back(nextWindow);
        }

        current += 2;
        next += 2;
    }

    if (current != end)
    {
        result.push_back(*current);
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
    const double kThreshold = 0.45; //!< Пороговое значение вероятности, от которого считаем, что составляющая присутствует в сигнале.
    const double maxValue = *std::max_element(std::begin(probabilities),
                                              std::end(probabilities));

    std::vector<WindowBounds> windows = splitByThreshold(probabilities, (kThreshold * maxValue));
    volatile bool isContinue = true;
    while (isContinue)
    {
        windows = joinDecomposition(windows,
                                    (kMinimumWaveDurationPeriods * frequencyToPeriod(frequency)),
                                    const_cast<bool*>(&isContinue));
    }

    WaveDecomposition result;
    for (const WindowBounds& each : windows)
    {
        if (static_cast<size_t>(std::distance(each.lower, each.upper)) >= (kMinimumWaveDurationPeriods * frequencyToPeriod(frequency)))
        {
            const double windowMeanValue = meanValue(each.lower, each.upper);
            result.emplace_back(frequency,
                                (windowMeanValue / maxValue),
                                std::distance(std::begin(probabilities), each.lower),
                                std::distance(each.lower, each.upper));
        }
    }

    return result;
}

}

WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies)
{
    std::vector<std::string> columnTitles;
    std::vector<std::vector<double>> columnValues;
    columnTitles.reserve(frequencies.size() * 3);
    columnValues.reserve(frequencies.size() * 3);
    size_t length = 0;

    for (size_t i = 0, size = frequencies.size(); i < size; ++i)
    {
        Logger::trace("Decompose frequency " + std::to_string(i+1) + "/" + std::to_string(size) + ".");

        const double& eachFrequency = frequencies.at(i);
        const size_t kWindowSize = frequencyToPeriod(eachFrequency);
        const size_t coefWindowExpanding = signal.size() / kWindowSize;
        const size_t expandedSize = kWindowSize * coefWindowExpanding;
        Logger::trace("Split to windows, window size = " + std::to_string(kWindowSize) + " discrets.");

        const std::vector<WindowBounds> windowsBounds = splitToWindows(signal, kWindowSize);
        Logger::trace("Windows count = " + std::to_string(windowsBounds.size()) + ".");

        columnTitles.push_back("probability #" + std::to_string(i+1));
        std::vector<double>& eachProbability = *(columnValues.insert(columnValues.end(), std::vector<double>()));

        Logger::trace("Calculate signal probabilities in windows.");
        eachProbability.reserve(signal.size());
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

        Logger::trace("Smoothing by mean average.");
        columnTitles.push_back("smooth #" + std::to_string(i+1));
        columnValues.push_back(::meanAverageSmooth(eachProbability, kWindowSize));

        length = std::max(length, eachProbability.size());
    }

    Logger::trace("Start probabilities analyzing.");
    WaveDecomposition result;
    for (size_t i = 0, size = columnValues.size(); i < size; i += 2)
    {
        Logger::trace("Decompose for frequency #" + std::to_string(i/2 + 1) + ".");
        WaveDecomposition forEachFrequency = ::decomposeByProbabilites(columnValues.at(i+1),
                                                                       frequencies.at(i/2));
        result.insert(std::end(result),
                      std::begin(forEachFrequency), std::end(forEachFrequency));

        columnTitles.push_back("detected on/off #" + std::to_string(i/2 + 1));
        enum
        {
            Off = 0,
            On = 1
        };
        auto inserted = columnValues.insert(columnValues.end(),
                                            std::vector<double>(signal.size(), Off));
        for (const Wave& each : forEachFrequency)
        {
            std::fill(std::begin(*inserted) + each.start_idx,
                      std::begin(*inserted) + each.start_idx + each.length,
                      On);
        }
    }

    writeValuesToCsv("base_probabilities.csv", columnTitles, length, columnValues);

    return result;
}
