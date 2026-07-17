// testHL-06b: forensic comparison of two band-limit configurations of the
// Gammatone hearing loss simulator, using the same audiometry (test-06 values):
//
//  - "oct" configuration: band limits are geometric means of the octave band
//    centers {125..16000}, so the audiometry indices align with the expander's
//    internal octave bands (self-consistent, no issue-#17 index mismatch).
//  - "exp" configuration: band limits actually used in the validation experiment,
//    derived from the clinical frequencies {250,500,1000,2000,3000,4000,6000,8000}
//    (audiometry indices do NOT match the expander octave bands: issue #17).
//
// For both configurations it saves:
//  - the configured expander parameters per group band (ratio, threshold, gain)
//    -> testHL-06b-bandConfig.mat
//  - the stereo outputs for inputs scaled x0.5, x1, x2, x4, x8
//    -> testHL-06b-stereoOutputs.mat

#define SAMPLINGFREQ 48000
#define HL_INITIAL_FREQ_HZ 125
#define HL_BANDS_NUMBER 8
#define HL_ATTACK_TIME_MS 200
#define HL_RELEASE_TIME_MS 50
#define HL_DBS_SPL_FOR_0_DBS_FS 100

#include <HAHLSimulation/HearingLossSim.h>
#include <HAHLSimulation/GammatoneMultibandExpander.h>
#include <Common/Buffer.h>
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
#include <matioCpp/matioCpp.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void LoadWav(CMonoBuffer<float> &samplesVector, const char *stringIn);

// Create and configure a gammatone multiband expander with the given group band limits
static std::shared_ptr<HAHLSimulation::CGammatoneMultibandExpander> MakeExpander(const std::vector<float> &bandLimits)
{
    auto expander = std::make_shared<HAHLSimulation::CGammatoneMultibandExpander>();
    expander->Setup(SAMPLINGFREQ, HL_INITIAL_FREQ_HZ, HL_BANDS_NUMBER, true);
    expander->SetGroups(bandLimits);
    for (int i = 0; i < expander->GetNumBands(true); i++)
    {
        expander->GetBandExpander(i, true)->SetAttack(HL_ATTACK_TIME_MS);
        expander->GetBandExpander(i, true)->SetRelease(HL_RELEASE_TIME_MS);
    }
    return expander;
}

// Save the configured state of one expander to the open .mat file, with a variable suffix
static void SaveExpanderConfig(matioCpp::File &file,
                               std::shared_ptr<HAHLSimulation::CGammatoneMultibandExpander> expander,
                               const std::string &suffix)
{
    std::vector<float> octAtt, ratios, thresholds, gainsDB;
    for (int i = 0; i < HL_BANDS_NUMBER; i++)
        octAtt.push_back(expander->GetAttenuationForOctaveBand(i));
    for (int i = 0; i < expander->GetNumBands(true); i++)
    {
        ratios.push_back(expander->GetBandExpander(i, true)->GetRatio());
        thresholds.push_back(expander->GetBandExpander(i, true)->GetThreshold());
        gainsDB.push_back(expander->GetGroupBandGainDB(i));
    }
    std::vector<float> centers = expander->GetGroupBandCentralFrequencies();

    matioCpp::Vector<float> centersVec("groupBandCenters_" + suffix);
    centersVec = centers;
    file.write(centersVec);
    matioCpp::Vector<float> octAttVec("octaveAttenuations_" + suffix);
    octAttVec = octAtt;
    file.write(octAttVec);
    matioCpp::Vector<float> ratiosVec("groupRatios_" + suffix);
    ratiosVec = ratios;
    file.write(ratiosVec);
    matioCpp::Vector<float> thresholdsVec("groupThresholds_" + suffix);
    thresholdsVec = thresholds;
    file.write(thresholdsVec);
    matioCpp::Vector<float> gainsVec("groupGainsDB_" + suffix);
    gainsVec = gainsDB;
    file.write(gainsVec);
}

int main()
{
    // Input signal
    CMonoBuffer<float> inputBuffer;
    LoadWav(inputBuffer, "pink_48000_4_sec.wav");

    // Band limits of the two configurations
    std::vector<float> bandLimits_oct = {176.776695296637f, 353.553390593274f, 707.106781186548f,
                                         1414.21356237310f, 2828.42712474619f, 5656.85424949238f, 11313.7084989848f};
    std::vector<float> bandLimits_exp = {353.5533906f, 707.1067812f, 1414.213562f,
                                         2449.489743f, 3464.101615f, 4898.979486f, 6928.20323f};

    // Audiometries (test-06 values, same for both configurations)
    std::vector<float> audiometryLeft  = {0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> audiometryRight = {0, 0, 0, 50, 50, 0, 0, 0};

    // Expanders and simulators for both configurations
    auto left_oct = MakeExpander(bandLimits_oct);
    auto right_oct = MakeExpander(bandLimits_oct);
    auto left_exp = MakeExpander(bandLimits_exp);
    auto right_exp = MakeExpander(bandLimits_exp);

    HAHLSimulation::CHearingLossSim sim_oct;
    HAHLSimulation::CHearingLossSim sim_exp;
    sim_oct.Setup(SAMPLINGFREQ, HL_DBS_SPL_FOR_0_DBS_FS, HL_BANDS_NUMBER, (int)inputBuffer.size());
    sim_exp.Setup(SAMPLINGFREQ, HL_DBS_SPL_FOR_0_DBS_FS, HL_BANDS_NUMBER, (int)inputBuffer.size());

    // Temporal distortion is enabled by default and, with zero jitter noise, degenerates
    // into a pure delay of bufferSize/2 samples with cross-buffer contamination (issues #1/#3)
    sim_oct.GetTemporalDistortionSimulator()->DisableTemporalDistortionSimulator(Common::T_ear::BOTH);
    sim_exp.GetTemporalDistortionSimulator()->DisableTemporalDistortionSimulator(Common::T_ear::BOTH);

    sim_oct.SetMultibandExpander(Common::T_ear::LEFT, left_oct);
    sim_oct.SetMultibandExpander(Common::T_ear::RIGHT, right_oct);
    sim_exp.SetMultibandExpander(Common::T_ear::LEFT, left_exp);
    sim_exp.SetMultibandExpander(Common::T_ear::RIGHT, right_exp);

    sim_oct.SetFromAudiometry_dBHL(Common::T_ear::LEFT, audiometryLeft);
    sim_oct.SetFromAudiometry_dBHL(Common::T_ear::RIGHT, audiometryRight);
    sim_exp.SetFromAudiometry_dBHL(Common::T_ear::LEFT, audiometryLeft);
    sim_exp.SetFromAudiometry_dBHL(Common::T_ear::RIGHT, audiometryRight);

    // Save the configured state of the four expanders
    {
        matioCpp::File file = matioCpp::File::Create("testHL-06b-bandConfig.mat");

        matioCpp::Vector<float> bandLimitsOctVec("bandLimits_oct");
        bandLimitsOctVec = bandLimits_oct;
        file.write(bandLimitsOctVec);
        matioCpp::Vector<float> bandLimitsExpVec("bandLimits_exp");
        bandLimitsExpVec = bandLimits_exp;
        file.write(bandLimitsExpVec);

        std::vector<float> octaveFreqs;
        for (int i = 0; i < HL_BANDS_NUMBER; i++)
            octaveFreqs.push_back(left_oct->GetOctaveBandFrequency(i));
        matioCpp::Vector<float> octaveFreqsVec("octaveBandFrequencies");
        octaveFreqsVec = octaveFreqs;
        file.write(octaveFreqsVec);

        // Frequencies at which the audiometry vector is intended, per configuration:
        // "oct" = the expander's octave bands; "exp" = the clinical audiometry frequencies
        matioCpp::Vector<float> audFreqOctVec("audiometryFrequencies_oct");
        audFreqOctVec = octaveFreqs;
        file.write(audFreqOctVec);
        std::vector<float> clinicalFreqs = {250, 500, 1000, 2000, 3000, 4000, 6000, 8000};
        matioCpp::Vector<float> audFreqExpVec("audiometryFrequencies_exp");
        audFreqExpVec = clinicalFreqs;
        file.write(audFreqExpVec);

        // Audiometry values (dBHL) fed to both configurations
        matioCpp::Vector<float> audValLVec("audiometryValues_left");
        audValLVec = audiometryLeft;
        file.write(audValLVec);
        matioCpp::Vector<float> audValRVec("audiometryValues_right");
        audValRVec = audiometryRight;
        file.write(audValRVec);

        SaveExpanderConfig(file, left_oct, "left_oct");
        SaveExpanderConfig(file, right_oct, "right_oct");
        SaveExpanderConfig(file, left_exp, "left_exp");
        SaveExpanderConfig(file, right_exp, "right_exp");
    }

    // Inputs scaled x0.5, x1, x2, x4, x8
    std::vector<Common::CEarPair<CMonoBuffer<float>>> stereoInputBuffers;
    for (int i = -1; i < 4; i++)
    {
        Common::CEarPair<CMonoBuffer<float>> stereoInputBuffer{inputBuffer, inputBuffer};
        stereoInputBuffer.left.ApplyGain(powf(2, i));
        stereoInputBuffer.right.ApplyGain(powf(2, i));
        stereoInputBuffers.push_back(stereoInputBuffer);
    }

    // Process each input through both simulators
    auto processAll = [&stereoInputBuffers](HAHLSimulation::CHearingLossSim &sim)
    {
        std::vector<Common::CEarPair<CMonoBuffer<float>>> outputs;
        for (int i = 0; i < 5; i++)
        {
            Common::CEarPair<CMonoBuffer<float>> out;
            out.left.resize(stereoInputBuffers[i].left.size());
            out.right.resize(stereoInputBuffers[i].right.size());
            sim.Process(stereoInputBuffers[i], out);
            outputs.push_back(out);
        }
        return outputs;
    };
    auto outputs_oct = processAll(sim_oct);
    auto outputs_exp = processAll(sim_exp);

    // Save inputs and outputs
    {
        matioCpp::File file = matioCpp::File::Create("testHL-06b-stereoOutputs.mat");

        std::string gainNames[5] = {"x05", "x1", "x2", "x4", "x8"};
        for (int i = 0; i < 5; i++)
        {
            matioCpp::Vector<float> inputVec("input_" + gainNames[i]);
            inputVec = stereoInputBuffers[i].left;
            file.write(inputVec);
        }
        auto writeOutputs = [&file, &gainNames](std::vector<Common::CEarPair<CMonoBuffer<float>>> &outputs, const std::string &cfg)
        {
            for (int i = 0; i < 5; i++)
            {
                matioCpp::Vector<float> outLeft("hearingLoss_" + gainNames[i] + "_left_" + cfg);
                matioCpp::Vector<float> outRight("hearingLoss_" + gainNames[i] + "_right_" + cfg);
                outLeft = outputs[i].left;
                outRight = outputs[i].right;
                file.write(outLeft);
                file.write(outRight);
            }
        };
        writeOutputs(outputs_oct, "oct");
        writeOutputs(outputs_exp, "exp");
    }

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
        std::exit(-1);
    }
}
