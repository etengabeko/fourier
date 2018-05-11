#include "dft.h"

#include <cmath>

#include "commons.h"

namespace fourier
{

const std::vector<std::complex<double>> dft(const std::vector<double>& signal)
{
    const size_t kLength = signal.size();
    std::vector<std::complex<double>> spectrum(kLength, { 0.0, 0.0 });

    for (size_t spectrumIndex = 0; spectrumIndex < kLength; ++spectrumIndex)
    {
        std::complex<double> sum(0.0, 0.0);
        for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
        {
            sum += signal[signalIndex] * std::exp(kImaginaryUnit * -2.0 * M_PI * static_cast<double>(spectrumIndex) * static_cast<double>(signalIndex) / static_cast<double>(kLength));
        }
        spectrum[spectrumIndex] = sum / static_cast<double>(kLength);
    }
    return spectrum;
}

const std::vector<double> inverseDft(const std::vector<std::complex<double>>& spectrum)
{
    const size_t kLength = spectrum.size();
    std::vector<double> signal(kLength, 0.0);

    for (size_t spectrumIndex = 0; spectrumIndex < kLength; ++spectrumIndex)
    {
        const std::vector<double> harmonic = inverseDft(spectrum, spectrumIndex);
        for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
        {
            signal[signalIndex] += harmonic[signalIndex];
        }
    }
    return signal;
}

const std::vector<double> inverseDft(const std::vector<std::complex<double>>& spectrum, const size_t spectrumIndex)
{
    const size_t kLength = spectrum.size();
    std::vector<double> sineSignal(kLength, 0.0);

    for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
    {
        const std::complex<double> complexValue = spectrum[spectrumIndex] * std::exp(kImaginaryUnit * 2.0 * M_PI * static_cast<double>(signalIndex) * static_cast<double>(spectrumIndex) / static_cast<double>(kLength));
        sineSignal[signalIndex] = complexValue.real();
    }

    return sineSignal;
}

} // fourier
