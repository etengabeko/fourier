#ifndef DFT_H
#define DFT_H

#include "common.h"

/**
 * @brief discreteFourierTransform - вычисление дискретного преобразования Фурье
 *        для сигнала signal, представленного последовательностью отсчётов (дискретов).
 * @param signal - преобразуемый сигнал.
 * @return последовательность чисел, соответствующая амплитудной составляющей спектра сигнала.
 */
const SignalSpectrum discreteFourierTransform(const CompositeSignal& signal);

#endif // DFT_H
