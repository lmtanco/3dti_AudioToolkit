#include "SoundSource.h"
#include "WavWriter.h"
#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <HRTF/HRTFFactory.h>
#include <HRTF/HRTFCereal.h>
#include <BRIR/BRIRFactory.h>
#include <BRIR/BRIRCereal.h>
#include <ISM/ISM.h>
#include <RtAudio.h>

#include <filesystem>
namespace fs = std::filesystem;

#define SAMPLERATE 48000
#define BUFFERSIZE 512
#define SOFAFILE "HRTF/SOFA/HRTF_SADIE_II_D1_44100_24bit_256tap_FIR_SOFA_aligned.sofa"
#define BRIRFILE "BRIR/SOFA/Sala108_listener1_sourceQuad_2m_48kHz_reverb_adjusted.sofa"
#define ROOMFILE "ISM/XML/trapezoidal_1_A1.xml"

#define LENGTH_OF_NORMALS 0.2
#define DEFAULT_SCALE 20
#define INITIAL_REFLECTION_ORDER 3
#define FRAME_RATE 60

/// 3dti Toolkit 
Binaural::CCore							myCore;				 // Core interface
std::shared_ptr<Binaural::CListener>			listener;			 // Pointer to listener interface
std::shared_ptr<Binaural::CEnvironment>		environment;
std::shared_ptr<Binaural::CSingleSourceDSP>  anechoicSourceDSP;   // Pointer to the original source DSP
SoundSource source1Wav;   
Common::CTransform sourcePosition;
Common::CEarPair<CMonoBuffer<float>> outputBufferStereo;
// void processAnechoic(CMonoBuffer<float> &bufferInput, Common::CEarPair<CMonoBuffer<float>> & bufferOutput);
// void FillBuffer(CMonoBuffer<float>& output, unsigned int & position, unsigned int & endChunk, std::vector<float>& samplesVector);
// void audioProcess(Common::CEarPair<CMonoBuffer<float>>& bufferOutput, int bufferSize);

/// ISM
std::shared_ptr<ISM::CISM>                   ISMHandler;
ISM::Room                               mainRoom;	
std::vector<shared_ptr<Binaural::CSingleSourceDSP>> imageSourceDSPList;			// Vector of pointers to all image source DSPs
std::vector<std::vector<float>> absortionsWalls;                                // Vector of wall absorptions
void LoadRoomGeometryFromXML(fs::path roomPath, ISM::RoomGeometry &roomGeometry, std::vector<std::vector<float>> &absortionsWalls);

// void processImages(CMonoBuffer<float> &bufferInput, Common::CEarPair<CMonoBuffer<float>> & bufferOutput);
// void processReverb(CMonoBuffer<float> &bufferInput, Common::CEarPair<CMonoBuffer<float>> & bufferOutput);

/// Audio
std::shared_ptr<RtAudio> audio;
// int SelectAudioDevice();
// void LoadWav(std::vector<float>& samplesVector, const char* stringIn);
// static int rtAudioCallback(void *outputBuffer, void *inputBuffer, unsigned int bufferSize, double streamTime, RtAudioStreamStatus status, void *data);

/// setup
void setup();

int main()
{
    setup();
    return 0;
}

/// @brief  Setup the 3dti Toolkit core, listener, environment and ISM handler, exits if any files are not found
void setup()
{
    // Retrieve current path
    // fs::path currentPath = std::getenv("CURRENT_PATH");
    fs::path currentPath = "/Users/dianauma/Repos/3dti_AudioToolkit_lmtanco/3dti_AudioToolkit/resources";
 
    // 3dti Toolkit core
    myCore.SetAudioState({SAMPLERATE, BUFFERSIZE});
    listener = myCore.CreateListener();

    // 3dti Listener
    Common::CVector3 listenerPosition = {-0.45, 0.02, -0.68};
    Common::CTransform listenerTransform; 
    listenerTransform.SetPosition(listenerPosition);
    listener->SetListenerTransform(listenerTransform);
    listener->DisableCustomizedITD(); // Disable customized head radius
    fs::path sofaPath = currentPath / SOFAFILE;
    if(fs::exists(sofaPath)){   
        bool notused;
        HRTF::CreateFromSofa(sofaPath.string(), listener, notused);
    }
    else {
        std::cout << "SOFA file not found in " << sofaPath.string() <<  std::endl;
        exit(-1);
    }

    // 3dti Environment
    environment = myCore.CreateEnvironment();
    environment->SetReverberationOrder(TReverberationOrder::BIDIMENSIONAL);
    fs::path brirPath = currentPath / BRIRFILE;
    if(fs::exists(brirPath)){
        BRIR::CreateFromSofa(brirPath.string(), environment);
    }
    else {
        std::cout << "BRIR file not found in " << brirPath.string() <<  std::endl;
        exit(-1);
    }    

    // Room setup
    ISM::RoomGeometry trapezoidal;
    fs::path roomPath = currentPath / ROOMFILE;
    if(fs::exists(roomPath)){
        // ISM::LoadRoomFromXML(roomPath.string(), trapezoidal); // TODO: This would be a nice way to load the room
        LoadRoomGeometryFromXML(roomPath, trapezoidal, absortionsWalls);

    }
    else {
        std::cout << "Room file not found in " << roomPath.string() <<  std::endl;
        exit(-1);
    }

    // ISM Handler
    ISMHandler = std::make_shared<ISM::CISM>(&myCore);
    ISMHandler->setupArbitraryRoom(trapezoidal);

}

#include <pugixml.hpp>

/// @brief  Load room geometry from XML file
void LoadRoomGeometryFromXML(fs::path roomPath, ISM::RoomGeometry &roomGeometry, std::vector<std::vector<float>> &absortionsWalls)
{
    // Open XML file
    pugi::xml_document doc;
    //pugi::xml_node xml = doc.root();
    auto result = doc.load_file(roomPath.string().c_str());
    if (!result)
    {
        std::cout << "Error loading XML file: " << result.description() << std::endl;
        exit(-1);
    }

    // Find all corners
    pugi::xpath_node_set corners = doc.select_nodes("//ROOMGEOMETRY/CORNERS/CORNER");
    for (pugi::xpath_node corner : corners)
    {
        pugi::xml_node node = corner.node();
        std::string p3Dstr = node.attribute("_3Dpoint").value();
        std::vector<float> p3D; 
        std::stringstream ss(p3Dstr);
        float i;
        while (ss >> i)
        {
            p3D.push_back(i);
            if (ss.peek() == ',')
                ss.ignore();
        }
        Common::CVector3 cornerPoint = {p3D[0], p3D[1], p3D[2]};
        roomGeometry.corners.push_back(cornerPoint);
    }

    // Find all walls and their absorptions
    pugi::xpath_node_set walls = doc.select_nodes("//ROOMGEOMETRY/WALLS/WALL");
    for (pugi::xpath_node wall : walls)
    {
        // Get corner indices which define the wall
        pugi::xml_node node = wall.node();
        std::string cornersStr = node.attribute("corner").value();
        std::vector<int> cornersOfWall;
        std::stringstream ss(cornersStr);
        int i;
        while (ss >> i)
        {
            cornersOfWall.push_back(i);
            if (ss.peek() == ',')
                ss.ignore();
        }
        roomGeometry.walls.push_back(cornersOfWall);

        // Get wall absorptions
        std::string absortionStr = node.attribute("absor").value();
        std::vector<float> absortionPerBand;
        std::stringstream ssAbs(absortionStr);
        float j;
        while (ssAbs >> j)
        {
            absortionPerBand.push_back(j);
            if (ssAbs.peek() == ',')
                ssAbs.ignore();
        }
        absortionsWalls.push_back(absortionPerBand);
    } 



}