#ifndef DFT_H
#define DFT_H

#include <complex>
#include <vector>

namespace fourier
{
/**
 * @brief dft - вычисление дискретного преобразования Фурье
 *        для сигнала signal, представленного последовательностью отсчётов (дискретов).
 * @param signal - преобразуемый сигнал.
 * @return спектр сигнала.
 */
const std::vector<std::complex<double>> dft(const std::vector<double>& signal);

/**
 * @brief inverse_dft - вычисление обратного дискретного преобразования Фурье
 *        для сигнала, представленного спектром spectrum.
 * @param spectrum - спектр сигнала.
 * @return последовательность отсчётов восстановленного сигнала (только его действительная часть).
 */
const std::vector<double> inverse_dft(const std::vector<std::complex<double>>& spectrum);

/**
 * @brief inverse_dft - вычисление обратного дискретного преобразования Фурье
 *        для одной гармоники сигнала, представленного спектром spectrum. Частота восстанавливаемой гармоники соответствует индексу spectrumIndex.
 * @param spectrum - спектр сигнала.
 * @param spectrumIndex - индекс в последовательность спктра, соответствующий частоте восстанавливаемой гармоники.
 * @return последовательность отсчётов восстановленной гармоники сигнала (только действительная часть).
 */
const std::vector<double> inverse_dft(const std::vector<std::complex<double>>& spectrum, const size_t spectrumIndex);

/**
 * @brief frequencyResponse - вычисление модуля спектра (амплитудно-частотная характеристика (АЧХ) сигнала).
 * @param spectrum - спектр сигнала.
 * @return значения амплитуды сигнала в зависимости от частоты.
 */
const std::vector<double> frequencyResponse(const std::vector<std::complex<double>>& spectrum);

/**
 * @brief phaseResponse - вычисление аргумента спектра (фазово-частотная характеристика (ФЧХ) сигнала).
 * @param spectrum - спектр сигнала.
 * @return значения фазы сигнала в зависимости от частоты.
 */
const std::vector<double> phaseResponse(const std::vector<std::complex<double>>& spectrum);

} // fourier

#endif // DFT_H
