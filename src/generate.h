#ifndef GENERATE_H
#define GENERATE_H

#include <cstddef>
#include <vector>

/**
 * @struct SineOption
 * @brief Основные параметры синусоидального базового сигнала.
 */
struct SineOption
{
    double freqFactor = 0.0;  //!< Множитель частоты (частота определяется как i/freqFactor).
    double startPhase = 0.0;  //!< Начальная фаза.
};

/**
 * @struct SineBehaviour
 * @brief Поведение базового сигнала в конкретный момент времени (для определённого дискретного отсчёта).
 */
struct SineBehaviour
{
    static const double kVolumeMin;  //!< Минимальное допустимое значение громкости (= 0.3).
    static const double kVolumeMax;  //!< Максимальное допустимое значение громкости (= 3.0).

    double volumeLevel = kVolumeMax; //!< Громкость сигнала.
    bool   enabled = false;          //!< Включен ли сигнал.
};

/**
 * @struct SineSignal
 * @brief Параметры базового сигнала.
 */
struct SineSignal
{
    SineOption sine;                      //!< Характеристики синусоиды базового сигнала.
    std::vector<SineBehaviour> behaviour; //!< Поведение базового сигнала во времени.
};

/**
 * @brief CompositeSignal - результирующий сигнал - набор дискретных значений,
 *        полученных суммированием синусоидальных базовых сигналов.
 */
using CompositeSignal = std::vector<double>;

/**
 * @brief generate - генерирует сигнал длиной signalLength из набора базовых сигналов,
 *        определённых параметрами baseSignals.
 * @param signalLength - длина результирующего сигнала (количество его дискретных значений).
 * @param baseSignals - параметры синусоидальных базовых сигналов.
 * @param noiseEnabled - вкл/выкл добавление случайного шума (в пределах 0%-15% максимальной амплитуды сигнала).
 * @return набор значений результирующего сигнала.
 */
const CompositeSignal generate(const size_t signalLength,
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
