#ifndef FILTER_H
#define FILTER_H

#include <complex>
#include <vector>

/**
 * @brief filterByFrequency - выделяет из сложного сигнала compositeSignal базовую составляющую,
 *        соответствующую частоте frequency (используя свёртку сигналов).
 * @param compositeSignal - сложный сигнал, полученный наложением нескольких базовых синусоидальных составляющих.
 * @param frequency - множитель частоты выделяемой составляющей.
 * @param spectrum [optional] - спектр выделенного базового сигнала.
 * @return набор дискретных отсчётов выделенного базового сигнала.
 */
const std::vector<double> filterByFrequency(const std::vector<double>& compositeSignal,
                                            const double frequency,
                                            std::vector<std::complex<double>>* spectrum = nullptr);

const std::vector<double> lowPassFilterByFrequency(const std::vector<double>& signal,
                                                   const double frequency);

const std::vector<double> highPassFilterByFrequency(const std::vector<double>& signal,
                                                    const double frequency);

#endif // FILTER_H
