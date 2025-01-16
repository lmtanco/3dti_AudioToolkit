#define SAMPLINGFREQ 48000
#define HL_INITIAL_FREQ_HZ 125
#define HL_BANDS_NUMBER 8
#define HL_ATTACK_TIME_MS 200
#define HL_RELEASE_TIME_MS 50

#include <HAHLSimulation/SimpleGammatoneMultibandExpander.h>
#include <Common/Buffer.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#include <matioCpp/matioCpp.h>
#include <filesystem>
#include <iostream>

void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn);

int main()
{
    HAHLSimulation::CSimpleGammatoneMultibandExpander expander1;
    HAHLSimulation::CSimpleGammatoneMultibandExpander expander2;
    HAHLSimulation::CSimpleGammatoneMultibandExpander expander3;

    // Setup using different overloads
    expander1.Setup(SAMPLINGFREQ, {125, 250, 500, 1000, 2000, 4000, 8000, 16000});
    std::vector<float> bandCenters = {125, 250, 500, 1000, 2000, 4000, 8000, 16000};
    expander2.Setup(SAMPLINGFREQ, bandCenters);
    expander3.Setup(SAMPLINGFREQ, HL_INITIAL_FREQ_HZ, HL_BANDS_NUMBER);

    // Compare internal data
    for (int i = 0; i < expander1.GetNumBands(); i++) {
        if (expander1.GetBandFrequency(i) != expander2.GetBandFrequency(i) || expander1.GetBandFrequency(i) != expander3.GetBandFrequency(i)) {
            std::cerr << "Mismatch in band frequency at index " << i << std::endl;
            return -1;
        }
        if (expander1.bands[i].lowerLimit_Hz != expander2.bands[i].lowerLimit_Hz || expander1.bands[i].lowerLimit_Hz != expander3.bands[i].lowerLimit_Hz) {
            std::cerr << "Mismatch in lower limit at index " << i << std::endl;
            return -1;
        }
        if (expander1.bands[i].upperLimit_Hz != expander2.bands[i].upperLimit_Hz || expander1.bands[i].upperLimit_Hz != expander3.bands[i].upperLimit_Hz) {
            std::cerr << "Mismatch in upper limit at index " << i << std::endl;
            return -1;
        }
        if (expander1.bands[i].lowerIndex != expander2.bands[i].lowerIndex || expander1.bands[i].lowerIndex != expander3.bands[i].lowerIndex) {
            std::cerr << "Mismatch in lower index at index " << i << std::endl;
            return -1;
        }
        if (expander1.bands[i].upperIndex != expander2.bands[i].upperIndex || expander1.bands[i].upperIndex != expander3.bands[i].upperIndex) {
            std::cerr << "Mismatch in upper index at index " << i << std::endl;
            return -1;
        }
    }

    std::cout << "All band frequencies match across all expanders." << std::endl;
    return 0;
}
