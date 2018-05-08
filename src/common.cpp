#include "common.h"

#include <algorithm>
#include <cmath>

const double SineBehaviour::kVolumeMin = 0.3;
const double SineBehaviour::kVolumeMax = 3.0;

double modulus(const std::complex<double>& complex)
{
    return std::sqrt(sqr(complex.real()) + sqr(complex.imag()));
}

double argument(const std::complex<double>& complex)
{
    return std::atan2(complex.imag(), complex.real());
}

const std::vector<double> frequencyResponse(const std::vector<std::complex<double>>& spectrum)
{
    std::vector<double> result(spectrum.size());
    std::transform(std::begin(spectrum),
                   std::end(spectrum),
                   std::begin(result),
                   &modulus);
    return result;
}

const std::vector<double> phaseResponse(const std::vector<std::complex<double>>& spectrum)
{
    std::vector<double> result(spectrum.size());
    std::transform(std::begin(spectrum),
                   std::end(spectrum),
                   std::begin(result),
                   &argument);
    return result;
}

size_t frequencyToIndex(const double frequency, const size_t width)
{
    return std::round(width / (2.0 * M_PI * frequency));
}
