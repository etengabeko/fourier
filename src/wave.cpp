#include "wave.h"

Wave::Wave(const double afrequency,
           const double aconfidence,
           const unsigned int astart_idx,
           const unsigned int alength) :
    frequency(afrequency),
    confidence(aconfidence),
    start_idx(astart_idx),
    length(alength)
{

}
