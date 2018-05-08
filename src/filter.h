#ifndef FILTER_H
#define FILTER_H

#include <complex>
#include <vector>

/**
 * @brief filterByFrequency - выделяет из сложного сигнала compositeSignal базовую составляющую,
 *        соответствующую частоте frequency.
 * @param compositeSignal - сложный сигнал, полученный наложением нескольких базовых синусоидальных составляющих.
 * @param frequency - множитель частоты выделяемой составляющей.
 * @param spectrum [optional] - спектр выделенного базового сигнала.
 * @return набор дискретных отсчётов выделенного базового сигнала.
 */
const std::vector<double> filterByFrequency(const std::vector<double>& compositeSignal,
                                            const double frequency,
                                            std::vector<std::complex<double>>* spectrum = nullptr);

#endif // FILTER_H
