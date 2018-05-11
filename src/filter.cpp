#include "filter.h"

#include <algorithm>

#include "common.h"
#include "dft.h"
#include "generate.h"

namespace
{

const std::vector<double> makeStandardSignal(const double frequency, const size_t length)
{
    const SineSignal sine{ { frequency, 0.0 }, std::vector<SineBehaviour>(length, { SineBehaviour::kVolumeMax, true }) };

    std::vector<double> result;
    result.reserve(length);

    for (size_t index = 0; index < length; ++index)
    {
        result.push_back(sineSignalValue(sine, index));
    }

    return result;
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
    const std::vector<std::complex<double>> standardSignalSpectrum = fourier::dft(standardSignal);

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
