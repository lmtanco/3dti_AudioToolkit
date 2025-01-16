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
    LoadWav(inputBuffer, "pink_48000_4_sec.wav");

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

    // Access the individual filters process the input individually for each filter
    Common::CGammatoneFilterBank &filterBank = expander.GetGammatoneFilterBank();
    auto numFilters = filterBank.GetNumFilters();
    std::vector<CMonoBuffer<float>> filterOutputs;
    for (auto i = 0; i < numFilters; i++)
    {
        CMonoBuffer<float> filterOutput(inputBuffer.size());
        auto filter = filterBank.GetFilter(i);
        filter->Process(inputBuffer, filterOutput);
        filterOutputs.push_back(filterOutput);
        // Acces the filter frequency and bandwith for each filter
        auto freq = filter->GetCenterFrequency();
        auto bw = filter->GetERBBandwidth();
    }
  

    // Copy the input and output to matio vectors
    matioCpp::Vector<float> inputVector("inputVector");
    matioCpp::Vector<float> outputVector("outputVector");
    inputVector = inputBuffer;
    outputVector = outputBuffer;

    // Copy the individual filter outputs to matio vectors
    std::vector<matioCpp::Vector<float>> filterOutputVectors;
    for (auto i = 0; i < numFilters; i++)
    {
        std::ostringstream oss;
        oss << "filterOutputVector_" << std::setw(2) << std::setfill('0') << i;
        matioCpp::Vector<float> filterOutputVector(oss.str());
        filterOutputVector = filterOutputs[i];
        filterOutputVectors.push_back(filterOutputVector);
    }

    // Save the input and output to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-04-input.mat");
        file.write(inputVector);
    }
    {
        matioCpp::File file = matioCpp::File::Create("testHL-04-output.mat");
        file.write(outputVector);
    }
    // Save the individual filter outputs to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-04-filterOutputs.mat");
        for (auto i = 0; i < numFilters; i++)
        {
            file.write(filterOutputVectors[i]);
        }
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

