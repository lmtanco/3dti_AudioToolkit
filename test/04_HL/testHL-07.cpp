/**
 * @file testHL.cpp
 * @brief 
 * @version 0.1
 * @date 2024-10-06
 * 
 * @copyright Copyright (c) 2024 Universidad de Málaga
 * 
 */

#define SAMPLINGFREQ 48000


#define HL_INITIAL_FREQ_HZ 125
#define HL_BANDS_NUMBER 8
// These two numbers above give the following bands:
// 125, 250, 500, 1000, 2000, 4000, 8000, 16000,

// However later the grouping is expressed via the band limits
// 353.5533906, 707.1067812, 1414.213562, 2449.489743, 3464.101615, 4898.979486, 6928.20323, which come from the bands
// 250, 500, 1000, 2000, 3000, 4000, 6000, 8000

// 353.553 = 250*sqrt(2)
// 707.106 = 500*sqrt(2)
// 1414.213 = 1000*sqrt(2)


#define HL_ATTACK_TIME_MS 200
#define HL_RELEASE_TIME_MS 50
#define HL_DBS_SPL_FOR_0_DBS_FS 100

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

    std::shared_ptr<HAHLSimulation::CGammatoneMultibandExpander> expander_left = std::make_shared<HAHLSimulation::CGammatoneMultibandExpander>();
    
    // Declare an input buffer
    CMonoBuffer<float> inputBuffer;

    // Load wav file
    LoadWav(inputBuffer, "pink_48000_4_sec.wav");

    // Delcare output buffer of same size as input
    CMonoBuffer<float> outputBuffer(inputBuffer.size());

    // Setup the gammatone multiband expander
    bool filterGrouping = true; // We will be grouping the filters according to the audiometry band limits
    expander_left->Setup(SAMPLINGFREQ, HL_INITIAL_FREQ_HZ, HL_BANDS_NUMBER, filterGrouping);
    // Audiometry band limits.
    // Note that the band limits are strange, not octaves. 
    vector<float> bandLimits = {353.5533906f, 707.1067812f, 1414.213562f, 2449.489743f, 3464.101615f, 4898.979486f, 6928.20323f};
    //vector<float> bandLimits = {176.776695296637f, 353.553390593274f,	707.106781186548f,	1414.21356237310f,	2828.42712474619f,	5656.85424949238f,	11313.7084989848f};
    //vector<float> bandLimits = {250.0f*sqrtf(2), 500.0f*sqrtf(2), 1000.0f*sqrtf(2), 2000.0f*sqrtf(2), 4000.0f*sqrtf(2), 8000.0f*sqrtf(2), 16000.0f*sqrtf(2)};
    expander_left->SetGroups(bandLimits);
    
    // Set the attack time for the expanders
    auto numBands = expander_left->GetNumBands(filterGrouping);
    for (size_t i = 0; i < numBands; i++)
    {
        expander_left->GetBandExpander(i, filterGrouping)->SetAttack(HL_ATTACK_TIME_MS);
        expander_left->GetBandExpander(i, filterGrouping)->SetRelease(HL_RELEASE_TIME_MS);
    }

    // Process the input buffer with the left expander before applying the hearing loss simulation
    expander_left->Process(inputBuffer, outputBuffer);

    // Access the individual filters process the input individually for each filter
    Common::CGammatoneFilterBank &filterBank = expander_left->GetGammatoneFilterBank();
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
        matioCpp::File file = matioCpp::File::Create("testHL-07-input.mat");
        file.write(inputVector);
    }
    {
        matioCpp::File file = matioCpp::File::Create("testHL-07-output.mat");
        file.write(outputVector);
    }
    // Save the individual filter outputs to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-07-filterOutputs.mat");
        for (auto i = 0; i < numFilters; i++)
        {
            file.write(filterOutputVectors[i]);
        }
    }

    // Save the band group indices to a .mat file
    matioCpp::Vector<int> bandIndicesStart("bandIndicesStart");
    matioCpp::Vector<int> bandIndicesEnd("bandIndicesEnd");
    std::vector<int> startIndices, endIndices;
    for (const auto& indices : expander_left->GetBandIndices()) {
        startIndices.push_back(indices[0]);
        endIndices.push_back(indices[1]);
    }
    bandIndicesStart = startIndices;
    bandIndicesEnd = endIndices;

    {
        matioCpp::File file = matioCpp::File::Create("testHL-07-bandIndices.mat");
        file.write(bandIndicesStart);
        file.write(bandIndicesEnd);
    }

    // Save the actual band limits to a .mat file
    matioCpp::Vector<float> bandLimitsVector("bandLimits");
    bandLimitsVector = bandLimits;
    {
        matioCpp::File file = matioCpp::File::Create("testHL-07-bandLimits.mat");
        file.write(bandLimitsVector);
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