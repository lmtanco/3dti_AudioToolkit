#include <Common/BiquadFilter.h>
#include <matioCpp/matioCpp.h>

#define BUFFERSIZE 16384
#define SAMPLINGFREQ 48000


int main(int argc, char* argv[])
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
    double fc = 15000;
    double Q = 1.4;
    // Get gain from the command line
    double Gain = 10.0;
    if (argc > 1) {
        Gain = std::atof(argv[1]);
        std::cout << "Using gain from command line: " << Gain << "\n";
    }
    
    filter.Setup(fs, fc, Q, Common::T_filterType::HIGHSHELF, Gain);
    
    // Filter the input with the filter;
    filter.Process(inputBuffer, outputBuffer);
    
    // Declare a Matio vector
    matioCpp::Vector<float> vector_out("output_vector_highshelf");
    vector_out = outputBuffer;
    
    // Save the vectors.
    {
            //Saving to file
            matioCpp::File file = matioCpp::File::Create("test5-highshelf.mat"); //Create a new file called "test.mat"
            file.write(vector_out);
    }

    // TODO: Run test from here, see: 
    // https://es.mathworks.com/matlabcentral/answers/472865-calling-matlab-functions-from-c-application-built-with-cmake-in-visual-studio 
    
    
}
