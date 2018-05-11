#ifndef COMMONS_H
#define COMMONS_H

#include <complex>
#include <vector>

/**
 * @brief unused - служебная пометка для неиспользуемых переменных.
 */
template <typename T>
void unused(T val) { (void)(val); }

/**
 * @brief sqr - возвращает квадрат значения value.
 */
template <typename T>
T sqr(T value) { return (value * value); }

/**
 * @brief kImaginaryUnit - мнимая единица.
 */
const std::complex<double> kImaginaryUnit { 0.0, 1.0 };

/**
 * @brief modulus - возвращает модуль комплексного числа complex.
 * @param complex - заданное комплексное число.
 * @return модуль числа complex.
 */
double modulus(const std::complex<double>& complex);

/**
 * @brief argument - возвращает аргумент комплексного числа complex.
 * @param complex - заданное комплексное число.
 * @return аргумент числа complex.
 */
double argument(const std::complex<double>& complex);

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

/**
 * @brief frequencyToIndex - преобразует множитель частоты frequency в индекс спектра ширины length.
 * @param frequency - множитель частоты.
 * @param length - ширина спектра.
 * @return индекс в последовательности значений спектра, соответствующих указанной частоте.
 */
size_t frequencyToIndex(const double frequency, const size_t width);

/**
 * @brief frequencyToPeriod - возвращает период синусоидального сигнала (в дискретах) с частотой frequency.
 * @param frequency - множитель частоты синусоиды.
 * @return период синусодидального сигнала.
 */
size_t frequencyToPeriod(const double frequency);

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

#endif // COMMONS_H
