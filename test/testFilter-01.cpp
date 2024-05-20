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
    inputBuffer.resize(N);
    outputBuffer.resize(N);
    
    // Create a delta of value 1 in inputBuffer[0].
    inputBuffer.Fill(N,0.0);
    inputBuffer[0] = 1.0;
    
    // Create a BiQuadFilter
    Common::CBiquadFilter filter;
    auto fs = SAMPLINGFREQ;
    auto fc = 1000;
    auto Q = 0.707;
    filter.Setup(fs, fc, Q, Common::T_filterType::LOWPASS);

    
    // Filter the input with the filter;
    filter.Process(inputBuffer, outputBuffer);
    
    // Declare a Matio vector
    matioCpp::Vector<float> vector_in("input_vector");
    vector_in = inputBuffer;
    matioCpp::Vector<float> vector_out("output_vector");
    vector_out = outputBuffer;
    
    // Save the vectors.
    {
            //Saving to file
            matioCpp::File file = matioCpp::File::Create("test.mat"); //Create a new file called "test.mat"
            //Saving to file
            file.write(vector_in);
            file.write(vector_out);
    }
    
    
}
