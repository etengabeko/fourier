#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include <functional>
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
                            const std::vector<double>& frequencies,
                            std::function<void(const std::string&,const std::vector<std::string>&,const size_t,const std::vector<std::vector<double>>&)> writeFunc);

#endif // DECOMPOSE_H
