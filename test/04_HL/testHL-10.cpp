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

int main()
{
    HAHLSimulation::CGammatoneMultibandExpander expander1;
    HAHLSimulation::CSimpleGammatoneMultibandExpander expander2;

    // Setup using different overloads
    expander1.Setup(SAMPLINGFREQ, 125, 8, false);
    expander2.Setup(SAMPLINGFREQ, {125, 250, 500, 1000, 2000, 3000, 4000, 6000, 8000});

    // Set the same groups for expander1 as expander2
    std::vector<float> bandLimits = {353.5533906f, 707.1067812f, 1414.213562f, 2449.489743f, 3464.101615f, 4898.979486f, 6928.20323f};
    expander1.SetGroups(bandLimits);

    // Compare number of bands
    auto numBands1 = expander1.GetNumBands(false);
    auto numBands2 = expander2.GetNumBands();
    ASSERT(numBands1 == numBands2, RESULT_ERROR_INVALID_PARAM, "Number of bands should be the same", "");

    // Compare the band frequencies
    for (size_t i = 0; i < numBands1; i++)
    {
        auto freq1 = expander1.GetBandFrequency(i, false);
        auto freq2 = expander2.GetBandFrequency(i);
        ASSERT(freq1 == freq2, RESULT_ERROR_INVALID_PARAM, "Band frequencies should be the same", "");
    }

    // Set the attack for the band expanders
    for (size_t i = 0; i < numBands1; i++)
    {
        expander1.GetBandExpander(i, false)->SetAttack(HL_ATTACK_TIME_MS);
        expander1.GetBandExpander(i, false)->SetRelease(HL_RELEASE_TIME_MS);
    }

    for (size_t i = 0; i < numBands2; i++)
    {
        expander2.bands[i].expander->SetAttack(HL_ATTACK_TIME_MS);
        expander2.bands[i].expander->SetRelease(HL_RELEASE_TIME_MS);
    }

    // Process the input buffer
    CMonoBuffer<float> inputBuffer;
    LoadWav(inputBuffer, "pink_48000_4_sec.wav");
    CMonoBuffer<float> ouputBufferWithGrouping(inputBuffer.size());
    CMonoBuffer<float> outputBufferNoGrouping(inputBuffer.size());

    // Process without grouping
    expander1.Process(inputBuffer, ouputBufferWithGrouping);
    expander2.Process(inputBuffer, outputBufferNoGrouping, false);

    // Copy the input and output to matio vectors
    matioCpp::Vector<float> inputVector("inputVector");
    matioCpp::Vector<float> outputVector1("outputVector1");
    matioCpp::Vector<float> outputVector2("outputVector2");
    inputVector = inputBuffer;
    outputVector1 = ouputBufferWithGrouping;
    outputVector2 = outputBufferNoGrouping;

    // Save the input to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-10-input.mat");
        file.write(inputVector);
    }
    // Save the output to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-10-output1.mat");
        file.write(outputVector1);
    }
    {
        matioCpp::File file = matioCpp::File::Create("testHL-10-output2.mat");
        file.write(outputVector2);
    }
}

void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn)
{
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;
    float *pSampleData = drwav_open_file_and_read_pcm_frames_f32(stringIn, &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (pSampleData != NULL) {
        for (size_t i = 0; i < totalPCMFrameCount; i++)
        {
            samplesVector.push_back(pSampleData[i]);
        }
        drwav_free(pSampleData, NULL);
    }
    else
    {
        std::cerr << "Error loading wav file" << std::endl;
        std::exit(-1);
    }
}
