#include <Common/CascadeGraphicEq9OctaveBands.h>
#include <matioCpp/matioCpp.h>

#define BUFFERSIZE 32768
#define SAMPLINGFREQ 48000


int main(int argc, char* argv[])
{
    // Declare a MonoBuffer
    CMonoBuffer<float> inputBuffer, inputBufferCopy, outputBuffer;
    auto N = inputBuffer.size();
    N = BUFFERSIZE;
    inputBuffer.resize(N, 0.0);
    inputBufferCopy.resize(N, 0.0);
    outputBuffer.resize(N, 0.0);
    
    // Create a delta of value 1 in inputBuffer[0].
    inputBuffer[0] = 1.0;
    inputBufferCopy[0] = 1.0;
    
    // Create a CascadeGraphicEq9OctaveBands
    //std::vector<float> gains = { 10, 10, 10, 10, 10, 10, 10, 10, 10 };

    // Create some random gains of values 0.7071 plus or minus 0.25
    //std::vector<float> gains = { 0.7071, 0.4571, 0.957, 0.5571, 0.1571, 0.8071, 0.6071, 0.3071, 0.1071 };

    // Create gains from 0.9 down and back up to 0.9 so that the average is 0.9
    //std::vector<float> gains = {0.9,0.7,0.5,0.3,0.1,0.3,0.5,0.7,0.9};
    std::vector<float> gains = {0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};

    // // Range for 
    for (auto &gain : gains) {
        gain *= 10.0;
    }
 
    //std::vector<float> gains = { 0.7071, 0.7071, 0.7071, 0.7071, 0.7071, 0.7071, 0.7071, 0.7071, 0.7071 };
    // Use 10 dB gains which are 3.1623 in linear scale 
    //std::vector<float> gains = {3.1623, 3.1623, 3.1623, 3.1623, 3.1623, 3.1623, 3.1623, 3.1623, 3.1623};
   
    // Testing the filter with the ReduceRipple flag set to true
    //constexpr bool ReduceRipple = false;
    //Common::CascadeGraphicEq9OctaveBands<ReduceRipple> filter(gains);
    Common::CascadeGraphicEq9OctaveBands filter(SAMPLINGFREQ);
    filter.SetCommandGains(SAMPLINGFREQ, gains);
    
    // Filter the input with the filter;
    filter.Process(inputBuffer);
    outputBuffer = inputBuffer;

    // Extract each filter as a BiQuadFilter
    auto FilterLowShelf = filter.GetFilter(0);
    auto FilterHighShelf = filter.GetFilter(8);
    auto FilterBand1 = filter.GetFilter(1);
    auto FilterBand2 = filter.GetFilter(2);
    auto FilterBand3 = filter.GetFilter(3);
    auto FilterBand4 = filter.GetFilter(4);
    auto FilterBand5 = filter.GetFilter(5);
    auto FilterBand6 = filter.GetFilter(6);
    auto FilterBand7 = filter.GetFilter(7);

    // Process the delta input with each filter and store it separately
    CMonoBuffer<float> outputLowShelf, outputHighShelf, outputBand1, outputBand2, outputBand3, outputBand4, outputBand5, outputBand6, outputBand7;
    outputLowShelf.resize(N, 0.0);
    outputHighShelf.resize(N, 0.0);
    outputBand1.resize(N, 0.0);
    outputBand2.resize(N, 0.0);
    outputBand3.resize(N, 0.0);
    outputBand4.resize(N, 0.0);
    outputBand5.resize(N, 0.0);
    outputBand6.resize(N, 0.0);
    outputBand7.resize(N, 0.0);

    FilterLowShelf->Process(inputBufferCopy, outputLowShelf);
    FilterHighShelf->Process(inputBufferCopy, outputHighShelf);
    FilterBand1->Process(inputBufferCopy, outputBand1);
    FilterBand2->Process(inputBufferCopy, outputBand2);
    FilterBand3->Process(inputBufferCopy, outputBand3);
    FilterBand4->Process(inputBufferCopy, outputBand4);
    FilterBand5->Process(inputBufferCopy, outputBand5);
    FilterBand6->Process(inputBufferCopy, outputBand6);
    FilterBand7->Process(inputBufferCopy, outputBand7);

    // Copy each output to a matio vector
    matioCpp::Vector<float> outputVector("outputVector");
    matioCpp::Vector<float> outputLowShelfVector("outputLowShelf");
    matioCpp::Vector<float> outputHighShelfVector("outputHighShelf");
    matioCpp::Vector<float> outputBand1Vector("outputBand1");
    matioCpp::Vector<float> outputBand2Vector("outputBand2");
    matioCpp::Vector<float> outputBand3Vector("outputBand3");
    matioCpp::Vector<float> outputBand4Vector("outputBand4");
    matioCpp::Vector<float> outputBand5Vector("outputBand5");
    matioCpp::Vector<float> outputBand6Vector("outputBand6");
    matioCpp::Vector<float> outputBand7Vector("outputBand7");
    outputVector = outputBuffer;
    outputLowShelfVector = outputLowShelf;
    outputHighShelfVector = outputHighShelf;
    outputBand1Vector = outputBand1;
    outputBand2Vector = outputBand2;
    outputBand3Vector = outputBand3;
    outputBand4Vector = outputBand4;
    outputBand5Vector = outputBand5;
    outputBand6Vector = outputBand6;
    outputBand7Vector = outputBand7;
    
    // Save the output of each filter
    {
        //Saving to file
        matioCpp::File file = matioCpp::File::Create("test7-grapheq-filters.mat"); 
        file.write(outputVector);
        file.write(outputLowShelfVector);
        file.write(outputHighShelfVector);
        file.write(outputBand1Vector);
        file.write(outputBand2Vector);
        file.write(outputBand3Vector);
        file.write(outputBand4Vector);
        file.write(outputBand5Vector);
        file.write(outputBand6Vector);
        file.write(outputBand7Vector);
    }        

    
    // TODO: Run test from here, see: 
    // https://es.mathworks.com/matlabcentral/answers/472865-calling-matlab-functions-from-c-application-built-with-cmake-in-visual-studio 
    
    
}
