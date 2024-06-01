#include <Common/BiquadFilter.h>
#include <matioCpp/matioCpp.h>

#define BUFFERSIZE 1024
#define SAMPLINGFREQ 48000


int main()
{
    // Declare a MonoBuffer
    CMonoBuffer<float> inputBuffer, outputBuffer;
    auto N = inputBuffer.size();
    N = BUFFERSIZE;
    inputBuffer.resize(N, 0.0);
    outputBuffer.resize(N, 0.0);
    
    // Create a delta of value 1 in inputBuffer[0].
    inputBuffer[0] = 1.0;
    
    // Create a BiQuadFilter
    Common::CBiquadFilter filter;
    double fs = SAMPLINGFREQ;
    double fc = 2000;
    double Q = 1.4;
    double Gain = 10.0;
    filter.Setup(fs, fc, Q, Common::T_filterType::PEAKNOTCH, Gain);
    
    // Filter the input with the filter;
    filter.Process(inputBuffer, outputBuffer);
    
    // Declare a Matio vector
    matioCpp::Vector<float> vector_out("output_vector_peaknotch");
    vector_out = outputBuffer;
    
    // Save the vectors.
    {
            //Saving to file
            matioCpp::File file = matioCpp::File::Create("test3-peaknotch.mat"); //Create a new file called "test.mat"
            file.write(vector_out);
    }
    
    
}
