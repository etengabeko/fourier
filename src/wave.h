#ifndef WAVE_H
#define WAVE_H

#include <vector>

/**
 * @struct Wave
 * @brief TODO
 */
struct Wave
{
    const double frequency = 0.0;
    const double confidence = 0.0;
    const unsigned int start_idx = 0;
    const unsigned int length = 0;

    Wave() = default;

    Wave(const double afrequency,
         const double aconfidence,
         const unsigned int astart_idx,
         const unsigned int alength);

};

using WaveDecomposition = std::vector<Wave>;

#endif // WAVE_H
