#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include <string>
#include <vector>

#include "wave.h"

/**
 * @brief kMinimumWaveDurationPeriods - минимальная длительность отрезка сигнала,
 *        в течение которой базовый сигнал включен или выключен.
 */
const size_t kMinimumWaveDurationPeriods = 5;

/**
 * @brief decompose - TODO
 * @param signal
 * @param frequencies
 * @return
 */
WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies);

#endif // DECOMPOSE_H
