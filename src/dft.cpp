#include "dft.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

namespace
{
/**
 * @brief sqr - возвращает квадрат значения value.
 */
template <typename T>
T sqr(T value) { return (value * value); }

/**
 * @brief kI - мнимая единица.
 */
const std::complex<double> kI { 0.0, 1.0 };

/**
 * @brief modulus - возвращает модуль комплексного числа complex.
 * @param complex - заданное комплексное число.
 * @return модуль числа complex.
 */
double modulus(const std::complex<double>& complex)
{
    return std::sqrt(::sqr(complex.real()) + ::sqr(complex.imag()));
}

/**
 * @brief argument - возвращает аргумент комплексного числа complex.
 * @param complex - заданное комплексное число.
 * @return аргумент числа complex.
 */
double argument(const std::complex<double>& complex)
{
    return std::atan2(complex.imag(), complex.real());
}

}

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
            sum += signal[signalIndex] * std::exp(kI * -2.0 * M_PI * static_cast<double>(spectrumIndex) * static_cast<double>(signalIndex) / static_cast<double>(kLength));
        }
        spectrum[spectrumIndex] = sum / static_cast<double>(kLength);
    }
    return spectrum;
}

const std::vector<double> inverse_dft(const std::vector<std::complex<double>>& spectrum)
{
    const size_t kLength = spectrum.size();
    std::vector<double> signal(kLength, 0.0);

    for (size_t spectrumIndex = 0; spectrumIndex < kLength; ++spectrumIndex)
    {
        const std::vector<double> harmonic = inverse_dft(spectrum, spectrumIndex);
        for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
        {
            signal[signalIndex] += harmonic[signalIndex];
        }
    }
    return signal;
}

const std::vector<double> inverse_dft(const std::vector<std::complex<double>>& spectrum, const size_t spectrumIndex)
{
    const size_t kLength = spectrum.size();
    std::vector<double> sineSignal(kLength, 0.0);

    for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
    {
        const std::complex<double> complexValue = spectrum[spectrumIndex] * std::exp(kI * 2.0 * M_PI * static_cast<double>(signalIndex) * static_cast<double>(spectrumIndex) / static_cast<double>(kLength));
        sineSignal[signalIndex] = complexValue.real();
    }

    return sineSignal;
}

const std::vector<double> frequencyResponse(const std::vector<std::complex<double>>& spectrum)
{
    std::vector<double> result(spectrum.size());
    std::transform(std::begin(spectrum),
                   std::end(spectrum),
                   std::begin(result),
                   modulus);
    return result;
}

const std::vector<double> phaseResponse(const std::vector<std::complex<double>>& spectrum)
{
    std::vector<double> result(spectrum.size());
    std::transform(std::begin(spectrum),
                   std::end(spectrum),
                   std::begin(result),
                   argument);
    return result;
}

} // fourier
