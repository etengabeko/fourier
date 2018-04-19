#include "common.h"
#include "decompose.h"
#include "generate.h"
#include "logger.h"

namespace
{

const std::vector<SineSignal> makeBaseSignals(const size_t signalLength,
                                              const std::vector<double>& frequencies)
{
    unused(signalLength);
    unused(frequencies);

    // TODO: make base signals

    return std::vector<SineSignal>();
}

}

int main(int argc, char* argv[])
{
    unused(argc);
    unused(argv);

    const size_t kSignalLength = 1000;
    const bool kNoiseEnabled = false;
    const std::vector<double> frequencies{};

    CompositeSignal signal = generate(kSignalLength,
                                      makeBaseSignals(kSignalLength, frequencies),
                                      kNoiseEnabled);
    // TODO: log signal

    WaveDecomposition waves = decompose(signal, frequencies);

    // TODO: log decomposition

    unused(waves);

    return EXIT_SUCCESS;
}
