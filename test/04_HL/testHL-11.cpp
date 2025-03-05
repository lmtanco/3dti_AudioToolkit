#define SAMPLINGFREQ 48000

// Not sure where these come from but trying to match the original test
#define HL_ATTACK_TIME_MS 200
#define HL_RELEASE_TIME_MS 50

#include <HAHLSimulation/GammatoneMultibandExpander.h>
#include <HAHLSimulation/SimpleGammatoneMultibandExpander.h>
#include <Common/Buffer.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#include <matioCpp/matioCpp.h>
#include <filesystem>
#include <iostream>

void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn);

// First value is band center, second value is level in dBHL
using AudiometryValue = std::pair<float, float>;
using Audiometry = std::vector<AudiometryValue>;

// compare floats with a tolerance
bool compareFloats(float a, float b, float tolerance = 0.0001)
{
    return std::abs(a - b) < tolerance;
}

// Configures CSimpleGammatoneMultibandExpander with audiometry data
class CAudiometryConfigurator
{
    public:
    
    void config(HAHLSimulation::CSimpleGammatoneMultibandExpander &expander, const Audiometry &_audiometry)  // This is the function that is being tested
    {
        // ASSERT that the audiometry has the correct number of bands
        ASSERT(_audiometry.size() == expander.GetNumBands(), RESULT_ERROR_INVALID_PARAM, "Audiometry should have the same number of bands as the expander", "");

        // ASSERT that the audiometry has the correct number of values for the frequency band (fist value is frequency, second value is level)
        for (size_t i = 0; i < _audiometry.size(); i++)
        {
            ASSERT(compareFloats(_audiometry[i].first, expander.GetBandFrequency(i)), RESULT_ERROR_INVALID_PARAM, "Audiometry should have in the same bands as the expander bank", "");
        }

        

       // Make copy of audiometry 
       audiometry = _audiometry;

    }
    
    private: 

    // Latest audiometry
    Audiometry audiometry;
};

int main()
{
    HAHLSimulation::CSimpleGammatoneMultibandExpander expander;
    CAudiometryConfigurator configurator;
    Audiometry audiometryCentralLoss = {{{125, 0}, {250, 0}, {500, 0}, {1000, 0}, {2000, 50}, {3000, 50}, {4000, 0}, {6000, 0}, {8000, 0}}};
    expander.Setup(SAMPLINGFREQ, {125, 250, 500, 1000, 2000, 3000, 4000, 6000, 8000});
    configurator.config(expander, audiometryCentralLoss);
    return 0;
}

void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn)
{
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;
    float *pSampleData = drwav_open_file_and_read_pcm_frames_f32(stringIn, &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData != NULL)
    {
        for (size_t i = 0; i < totalPCMFrameCount; i++)
        {
            samplesVector.push_back(pSampleData[i]);
        }
        drwav_free(pSampleData, NULL);
    }
    else
    {
        std::cerr << "Error loading wav file" << std::endl;
    }
}
