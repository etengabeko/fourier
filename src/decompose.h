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
 * @brief decompose - реализация алгоритма декомпозиции сигнала signal на составляющие базовые сигналы с частотами frequencies.
 * @param signal - сложный сигнал, систавленный из суммы простых сигналов с частотами frequencies.
 * @param frequencies - набор частот, составляющих сложный сигнал.
 * @return набор характеристик базовых сигналов, выделенных из состава сложного.
 */
WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies);

#endif // DECOMPOSE_H
