#include "filter.h"

#include <algorithm>

#include "common.h"
#include "dft.h"
#include "generate.h"

namespace
{

const std::vector<double> makeStandardSignal(const double frequency, const size_t length)
{
    const size_t kStandardSignalMinimumLength = 5 * frequencyToPeriod(frequency);

    std::vector<SineBehaviour> behaviour(length, { SineBehaviour::kVolumeMin, false });
    std::fill(std::begin(behaviour),
              (length >= kStandardSignalMinimumLength ? std::begin(behaviour) + kStandardSignalMinimumLength
                                                      : std::end(behaviour)),
              SineBehaviour{ SineBehaviour::kVolumeMax, true });
    const SineSignal sine{ { frequency, 0.0 }, behaviour };

    std::vector<double> result;
    result.reserve(length);

    for (size_t index = 0; index < length; ++index)
    {
        result.push_back(sineSignalValue(sine, index));
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

    std::vector<std::complex<double>> harmonicSpectrum(kLength, { 0.0, 0.0 });
    std::transform(std::begin(compositeSignalSpectrum),
                   std::end(compositeSignalSpectrum),
                   std::begin(standardSignalSpectrum),
                   std::begin(harmonicSpectrum),
                   [](const std::complex<double>& eachComposite,
                      const std::complex<double>& eachStandard)
                   { return (eachComposite * eachStandard); });

    if (spectrum != nullptr)
    {
        *spectrum = harmonicSpectrum;
    }

    return fourier::inverseDft(harmonicSpectrum);
}
