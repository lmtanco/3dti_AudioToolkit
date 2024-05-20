#include <Common/BiquadFilter.h>
#include <matioCpp/matioCpp.h>

#define BUFFERSIZE 1024

int main()
{
    // Declare a MonoBuffer
    CMonoBuffer<float> inputBuffer, outputBuffer;
    auto N = inputBuffer.size();
    N = BUFFERSIZE;
    inputBuffer.resize(N);
    outputBuffer.resize(N);
    
    
    
    // Declare a Matio vector
    matioCpp::Vector<float> vector_in("inpput_vector");
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
