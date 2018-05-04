#ifndef WAVE_H
#define WAVE_H

#include <string>
#include <vector>

/**
 * @struct Wave
 * @brief Характеристики базового сигнала в составе сложного.
 */
struct Wave
{
    double frequency = 0.0;     //!< Относительная частота
    double confidence = 0.0;    //!< (не используется)
    unsigned int start_idx = 0; //!< Индекс отсчёта, с которого данная частота проявлена в сложном сигнале.
    unsigned int length = 0;    //!< Длительность проявления данной частоты в сложном сигнале (в отсчётах).

    Wave() = default;

    Wave(const double afrequency,
         const double aconfidence,
         const unsigned int astart_idx,
         const unsigned int alength);

    const std::string toString() const;
};

using WaveDecomposition = std::vector<Wave>;

#endif // WAVE_H
