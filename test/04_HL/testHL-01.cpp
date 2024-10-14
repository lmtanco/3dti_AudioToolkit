/**
 * @file testHL.cpp
 * @author Luis Molina-Tanco (lmtanco@uma.esº)
 * @brief 
 * @version 0.1
 * @date 2024-10-06
 * 
 * @copyright Copyright (c) 2024 Universidad de Málaga
 * 
 */

#define BUFFERSIZE 32768
#define SAMPLINGFREQ 48000
#define HL_INITIAL_FREQ_HZ 250
#define HL_BANDS_NUMBER 8
#define HL_ATTACK_TIME_MS 200
#define HL_RELEASE_TIME_MS 50

#include <HAHLSimulation/GammatoneMultibandExpander.h>  
#include <Common/Buffer.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#include <matioCpp/matioCpp.h>
#include <filesystem>


void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn);

int main(int argc, char const *argv[])
{   

    // Obtain the current path 
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Show the current path
    std::cout << "Current path is " << currentPath << std::endl;

    HAHLSimulation::CGammatoneMultibandExpander expander;

    // Declare an input buffer
    CMonoBuffer<float> inputBuffer;

    // Load wav file
    LoadWav(inputBuffer, "sweep_4seg.wav");

    // Delcare output buffer of same size as input
    CMonoBuffer<float> outputBuffer(inputBuffer.size());

    // Setup the gammatone multiband expander
    bool filterGrouping = true; // We will be grouping the filters according to the audiometry band limits
    expander.Setup(SAMPLINGFREQ, HL_INITIAL_FREQ_HZ, HL_BANDS_NUMBER, filterGrouping);
    // Audiometry band limits. 
    vector<float> bandLimits = {353.5533906f, 707.1067812f, 1414.213562f, 2449.489743f, 3464.101615f, 4898.979486f, 6928.20323f};
    expander.SetGroups(bandLimits);

    // Set the attack time for the expanders
    auto numBands = expander.GetNumBands(filterGrouping);
    for (size_t i = 0; i < numBands; i++)
    {
        expander.GetBandExpander(i, filterGrouping)->SetAttack(HL_ATTACK_TIME_MS);
        expander.GetBandExpander(i, filterGrouping)->SetRelease(HL_RELEASE_TIME_MS);
    }

    // Process the input buffer
    expander.Process(inputBuffer, outputBuffer);

    // Copy the input and output to matio vectors
    matioCpp::Vector<float> inputVector("inputVector");
    matioCpp::Vector<float> outputVector("outputVector");
    inputVector = inputBuffer;
    outputVector = outputBuffer;
    
    // Get size of inputVector, make sure it was copied ok
    if (inputVector.size() != inputBuffer.size())
    {
        std::cerr << "Error copying input buffer to matio vector" << std::endl;
        std::exit(-1);
    }
    // Same for outputVector
    if (outputVector.size() != outputBuffer.size())
    {
        std::cerr << "Error copying output buffer to matio vector" << std::endl;
        std::exit(-1);
    }

    // Save the input and output to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-01-input.mat");       	
        file.write(inputVector);
    }
    {
        matioCpp::File file = matioCpp::File::Create("testHL-01-output.mat");       	
        file.write(outputVector);
    }

    return 0;
}

#include <iostream>

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

