#ifndef COMMON_H
#define COMMON_H

#include <vector>

template <typename T>
void unused(T val) { (void)(val); }

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

/** TODO
 * @brief SignalSpectrum - спектр сигнала - набор дискретных значений амплитуд
 *        частотных составляющих комбинированного сигнала в диапазоне от 1 до 1/N Гц,
 *        где N - количество дискретов в наборе.
 */
using SignalSpectrum = std::vector<double>;

#endif // COMMON_H
