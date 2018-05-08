#ifndef GENERATE_H
#define GENERATE_H

#include <cstddef>
#include <vector>

#include "common.h"

/**
 * @brief generate - генерирует сигнал длиной signalLength из набора базовых сигналов,
 *        определённых параметрами baseSignals.
 * @param signalLength - длина результирующего сигнала (количество его дискретных значений).
 * @param baseSignals - параметры синусоидальных базовых сигналов.
 * @param noiseEnabled - вкл/выкл добавление случайного шума (в пределах 0%-15% максимальной амплитуды сигнала).
 * @return набор значений результирующего сигнала.
 */
const std::vector<double> generate(const size_t signalLength,
                                   const std::vector<SineSignal>& baseSignals,
                                   bool noiseEnabled = false);

/**
 * @brief sineSignalValue - возвращает значение амплитуды базового сигнала с характеристиками signal
 *        в момент времени, определённый значением index.
 * @param signal - параметры базового синусоидального сигнала.
 * @param index - индекс, характеризующий момент времени.
 * @return значение амплитуды сигнала.
 */
double sineSignalValue(const SineSignal& signal, const size_t index);

#endif // GENERATE_H
