#include "dft.h"

#include <cmath>
#include <complex>
#include <cstddef>

namespace
{
/**
 * @brief sqr - возвращает квадрат значения value.
 */
template <typename T>
T sqr(T value) { return (value * value); }

}

const SignalSpectrum discreteFourierTransform(const CompositeSignal& signal)
{
    const size_t kLength = signal.size();
    std::vector<std::complex<double>> complexSpectrum(kLength);

    // Вычисление спектральных составляющих сигнала (Дискретное преобразование Фурье (DFT)):
    for (size_t spectrumIndex = 0; spectrumIndex < kLength; ++spectrumIndex)
    {
        const double spectrumTerm = -2 * M_PI / kLength * spectrumIndex;
        for (size_t signalIndex = 0; signalIndex < kLength; ++signalIndex)
        {
            const std::complex<double> expTerm(0.0, spectrumTerm * signalIndex);
            complexSpectrum[spectrumIndex] += signal[signalIndex] * std::exp(expTerm);
        }
    }

    // Выделение амплитудной части спектра (вычисление модуля комплексного числа):
    SignalSpectrum result;
    result.reserve(kLength);
    for (const std::complex<double>& each : complexSpectrum)
    {
        const double modulus = std::sqrt(::sqr(each.real()) + ::sqr(each.imag()));
        result.push_back(modulus);
    }

    return result;
}
