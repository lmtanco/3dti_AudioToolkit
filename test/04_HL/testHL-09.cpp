#define SAMPLINGFREQ 48000

// Not sure where these come from but trying to match the original test
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

    // Setup using different overloads
    expander1.Setup(SAMPLINGFREQ, {125, 250, 500, 1000, 2000, 3000, 4000, 6000, 8000});

    // Set the attack for the band expanders
    auto numBands = expander1.GetNumBands();
    for (size_t i = 0; i < numBands; i++)
    {
        expander1.bands[i].expander->SetAttack(HL_ATTACK_TIME_MS);
        expander1.bands[i].expander->SetRelease(HL_RELEASE_TIME_MS);
    }

    // Process the input buffer
    CMonoBuffer<float> inputBuffer;
    LoadWav(inputBuffer, "pink_48000_4_sec.wav");
    CMonoBuffer<float> outputBuffer(inputBuffer.size());

    // Important, the filterGrouping parameter is not any more a member of the class, just a parameter of the Process function
    bool filterGrouping = true;
    expander1.Process(inputBuffer, outputBuffer, filterGrouping);


    // Access the individual filters process the input individually for each filter
    auto numFilters = expander1.filters.size();
    std::vector<CMonoBuffer<float>> filterOutputs;
    for (auto i = 0; i < numFilters; i++)
    {
        CMonoBuffer<float> oneFilterOutputBuffer(inputBuffer.size());
        expander1.filters[i].filter->Process(inputBuffer, oneFilterOutputBuffer);
        filterOutputs.push_back(oneFilterOutputBuffer);
    }

    // Output all information of the bands and filters to the console
    for (int i = 0; i < expander1.GetNumBands(); i++)
    {
        std::cout << "Band " << i << " has center frequency " << expander1.GetBandFrequency(i) << " Hz" << std::endl;
        std::cout << "Band " << i << " has lower limit " << expander1.bands[i].lowerLimit_Hz << " Hz" << std::endl;
        std::cout << "Band " << i << " has upper limit " << expander1.bands[i].upperLimit_Hz << " Hz" << std::endl;
        std::cout << "Band " << i << " has lower index " << expander1.bands[i].lowerIndex << std::endl;
        std::cout << "Band " << i << " has upper index " << expander1.bands[i].upperIndex << std::endl;
   
    }

    // Copy the input and output to matio vectors
    matioCpp::Vector<float> inputVector("inputVector");
    matioCpp::Vector<float> outputVector("outputVector");
    inputVector = inputBuffer;
    outputVector = outputBuffer;

    // Save the input to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-09-input.mat");
        file.write(inputVector);
    }
    // Save the output to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-09-output.mat");
        file.write(outputVector);
    }
    // Save the individual filter outputs to a mat file
    {
        matioCpp::File file = matioCpp::File::Create("testHL-09-filterOutputs.mat");
        for (auto i = 0; i < numFilters; i++)
        {
            std::ostringstream oss;
            oss << "filterOutputVector_" << std::setw(2) << std::setfill('0') << i;
            matioCpp::Vector<float> filterOutputVector(oss.str());
            filterOutputVector = filterOutputs[i];
            file.write(filterOutputVector);
        }
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