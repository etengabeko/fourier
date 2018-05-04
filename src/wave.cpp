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

const std::string Wave::toString() const
{
    std::string result;
    result += "{";
    result += "\n    frequency = ";
    result += std::to_string(frequency);
    result += "\n    confidence = ";
    result += std::to_string(confidence);
    result += "\n    start_idx = ";
    result += std::to_string(start_idx);
    result += "\n    length = ";
    result += std::to_string(length);
    result += "\n}";

    return result;
}
