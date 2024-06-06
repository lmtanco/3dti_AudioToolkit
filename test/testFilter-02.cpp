#include <Common/BiquadFilter.h>
#include <matioCpp/matioCpp.h>

// NOTE FOR MYSELF:
//
// Frequency resolution is the distance in Hz between two adjacent frequency bins in the FFT.
// The frequency resolution is equal to the sampling frequency divided by the FFT size.
// i.e. freqRes = SAMPLINGFREQ / BUFFERSIZE
// The frequency resolution is the smallest frequency difference that can be detected by the FFT.
// If the frequency resolution is too large, detail will be lost in the frequency domain, especially 
// if the detail is in the lower frequencies. 
// For example, in this test, if the fc of the Peak Notch filter is is 125Hz, a BUFFERSIZE of 1024 will not work, 
// because the frequency resolution is 46.875 Hz, which is comparable to the fc of the filter.
// For instance for a sampling frequency of 48000 Hz and a buffer size of 16384, the frequency resolution is 2.93 Hz.
#define BUFFERSIZE 16384
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
    double fc = 125;
    double Q = std::sqrt(2.0);
    double Gain = 4.0;
    filter.Setup(fs, fc, Q, Common::T_filterType::PEAKNOTCH, Gain, false);
    
    // Filter the input with the filter;
    filter.Process(inputBuffer, outputBuffer);
    
    // Declare a Matio vector
    matioCpp::Vector<float> vector_in("input_vector_peaknotch");
    vector_in = inputBuffer;
    matioCpp::Vector<float> vector_out("output_vector_peaknotch");
    vector_out = outputBuffer;
    
    // Save the vectors.
    {
            //Saving to file
            matioCpp::File file = matioCpp::File::Create("test2-peaknotch.mat"); //Create a new file called "test.mat"
            //Saving to file
            file.write(vector_in);
            file.write(vector_out);
    }
    
    
}
