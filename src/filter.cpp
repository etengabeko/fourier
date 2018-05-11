#include "filter.h"

#include <algorithm>
#include <map>
#include <utility>

#include "commons.h"
#include "dft.h"
#include "generate.h"
#include "logger.h"

namespace
{

const std::vector<double> makeStandardSignal(const double frequency, const size_t length)
{
    static std::map<std::pair<double, size_t>, std::vector<double>> standardSignalsCache;

    auto founded = standardSignalsCache.find({ frequency, length });
    if (founded == std::end(standardSignalsCache))
    {
        const SineSignal sine{ { frequency, 0.0 }, std::vector<SineBehaviour>(length, { SineBehaviour::kVolumeMax, true }) };

        std::vector<double> signalValues;
        signalValues.reserve(length);

        for (size_t index = 0; index < length; ++index)
        {
            signalValues.push_back(sineSignalValue(sine, index));
        }

        auto inserted = standardSignalsCache.insert({ { frequency, length }, signalValues });
        if (inserted.second)
        {
            founded = inserted.first;
        }
        else
        {
            Logger::error(  "Unexpected error: can't insert value in signals cache: "
                            "{ frequency = " + std::to_string(frequency)
                          + ", length = " + std::to_string(length)
                          + " }.");
            return std::vector<double>();
        }
    }

    return founded->second;
}

const std::vector<std::complex<double>> makeStandardSpectrum(const double frequency,
                                                             const std::vector<double> signal)
{
    static std::map<std::pair<double, size_t>, std::vector<std::complex<double>>> standardSpectrumsCache;

    const size_t kLength = signal.size();

    auto founded = standardSpectrumsCache.find({ frequency, kLength });
    if (founded == std::end(standardSpectrumsCache))
    {
        auto inserted = standardSpectrumsCache.insert({ { frequency, kLength }, fourier::dft(signal) });
        if (inserted.second)
        {
            founded = inserted.first;
        }
        else
        {
            Logger::error(  "Unexpected error: can't insert value in spectrums cache: "
                            "{ frequency = " + std::to_string(frequency)
                          + ", length = " + std::to_string(kLength)
                          + " }.");
            return std::vector<std::complex<double>>();
        }
    }

    return founded->second;
}

enum class FilterType
{
    LowPass,
    HighPass
};

const std::vector<std::complex<double>> makeSincSpectrum(const double frequency, const size_t length, FilterType type)
{
    std::vector<std::complex<double>> result(length, {0.0, 0.0});

    const size_t cutoffLowerIndex = frequencyToIndex(frequency, length);
    const size_t cutoffUpperIndex = length - cutoffLowerIndex;

    for (size_t i = 0; i < length; ++i)
    {
        switch (type)
        {
        case FilterType::LowPass:
            if (i <= cutoffLowerIndex || cutoffUpperIndex <= i)
            {
                result[i] = { 1.0, 0.0 };
            }
            break;
        case FilterType::HighPass:
            if (cutoffLowerIndex <= i && i <= cutoffUpperIndex)
            {
                result[i] = { 1.0, 0.0 };
            }
            break;
        default:
            break;
        }
    }

    return result;
}

}

const std::vector<double> filterByFrequency(const std::vector<double>& compositeSignal,
                                            const double frequency,
                                            std::vector<std::complex<double>>* spectrum)
{
    const size_t kLength = compositeSignal.size();

    const std::vector<double> standardSignal = ::makeStandardSignal(frequency, kLength);
    const std::vector<std::complex<double>> standardSignalSpectrum = ::makeStandardSpectrum(frequency, standardSignal);

    const std::vector<std::complex<double>> compositeSignalSpectrum = fourier::dft(compositeSignal);

    std::vector<std::complex<double>> convolutionSpectrum(kLength, { 0.0, 0.0 });
    std::transform(std::begin(compositeSignalSpectrum),
                   std::end(compositeSignalSpectrum),
                   std::begin(standardSignalSpectrum),
                   std::begin(convolutionSpectrum),
                   [](const std::complex<double>& eachComposite,
                      const std::complex<double>& eachStandard)
                   { return (eachComposite * eachStandard); });

    if (spectrum != nullptr)
    {
        *spectrum = convolutionSpectrum;
    }

    return fourier::inverseDft(convolutionSpectrum);
}

const std::vector<double> lowPassFilterByFrequency(const std::vector<double>& signal,
                                                   const double frequency)
{
    const size_t kLength = signal.size();

    const std::vector<std::complex<double>> sincSpectrum = ::makeSincSpectrum(frequency, kLength, FilterType::LowPass);
    const std::vector<std::complex<double>> signalSpectrum = fourier::dft(signal);

    std::vector<std::complex<double>> convolutionSpectrum(kLength, { 0.0, 0.0 });
    std::transform(std::begin(signalSpectrum),
                   std::end(signalSpectrum),
                   std::begin(sincSpectrum),
                   std::begin(convolutionSpectrum),
                   [](const std::complex<double>& eachSignal,
                      const std::complex<double>& eachSinc)
                   { return (eachSignal * eachSinc); });

    return fourier::inverseDft(convolutionSpectrum);
}

const std::vector<double> highPassFilterByFrequency(const std::vector<double>& signal,
                                                   const double frequency)
{
    const size_t kLength = signal.size();

    const std::vector<std::complex<double>> sincSpectrum = ::makeSincSpectrum(frequency, kLength, FilterType::HighPass);
    const std::vector<std::complex<double>> signalSpectrum = fourier::dft(signal);

    std::vector<std::complex<double>> convolutionSpectrum(kLength, { 0.0, 0.0 });
    std::transform(std::begin(signalSpectrum),
                   std::end(signalSpectrum),
                   std::begin(sincSpectrum),
                   std::begin(convolutionSpectrum),
                   [](const std::complex<double>& eachSignal,
                      const std::complex<double>& eachSinc)
                   { return (eachSignal * eachSinc); });

    return fourier::inverseDft(convolutionSpectrum);
}
