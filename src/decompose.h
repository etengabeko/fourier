#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include "wave.h"

/**
 * @brief decompose - TODO
 * @param signal
 * @param frequencies
 * @return
 */
WaveDecomposition decompose(const std::vector<double>& signal,
                            const std::vector<double>& frequencies);

#endif // DECOMPOSE_H
