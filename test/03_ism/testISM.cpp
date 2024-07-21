#include <BinauralSpatializer/3DTI_BinauralSpatializer.h>
#include <HRTF/HRTFFactory.h>
#include <HRTF/HRTFCereal.h>
#include <ISM/ISM.h>
#include <matioCpp/matioCpp.h>

#include <filesystem>
namespace fs = std::filesystem;

#define SAMPLERATE 48000
#define BUFFERSIZE 32768
#define SOFAFILE "HRTF/SOFA/HRTF_SADIE_II_D1_44100_24bit_256tap_FIR_SOFA_aligned.sofa"
#define ROOMFILE "ISM/XML/trapezoidal_1_A1.xml"
#define INITIAL_REFLECTION_ORDER 3
#define INITIAL_DIST_SILENCED_FRAMES  1000

/// 3dti Toolkit 
Binaural::CCore							myCore;				 // Core interface
std::shared_ptr<Binaural::CListener>			listener;			 // Pointer to listener interface
std::shared_ptr<Binaural::CEnvironment>		environment;
std::shared_ptr<Binaural::CSingleSourceDSP>  anechoicSourceDSP;   // Pointer to the original source DSP
Common::CTransform sourcePosition;
Common::CEarPair<CMonoBuffer<float>> outputBufferStereo;
int numberOfSilencedFrames = 0;

/// ISM
std::shared_ptr<ISM::CISM>                   ISMHandler;
ISM::Room                               mainRoom;	
std::vector<shared_ptr<Binaural::CSingleSourceDSP>> imageSourceDSPList;			// Vector of pointers to all image source DSPs
std::vector<std::vector<float>> absortionsWalls;                                // Vector of wall absorptions
void LoadRoomGeometryFromXML(fs::path roomPath, ISM::RoomGeometry &roomGeometry, std::vector<std::vector<float>> &absortionsWalls);
std::vector<shared_ptr<Binaural::CSingleSourceDSP>> createImageSourceDSP(); 
void audioProcess(const CMonoBuffer<float>& input,  Common::CEarPair<CMonoBuffer<float>>& bufferOutput, int bufferSize);

/// setup
void setup();

int main()
{
    // Setup everything:3dti Toolkit core, listener, environment and ISM handler
    setup();

    // Create Delta Input Buffer
    CMonoBuffer<float> inputBuffer;
    inputBuffer.resize(BUFFERSIZE, 0.0);
    inputBuffer[0] = 1.0;

    // Create Ouptut Buffer
    Common::CEarPair<CMonoBuffer<float>> outputStereoBuffer;
    outputStereoBuffer.left.resize(BUFFERSIZE);
    outputStereoBuffer.right.resize(BUFFERSIZE);
    
    // Process 
    audioProcess(inputBuffer, outputStereoBuffer, BUFFERSIZE);

    // Create Matio Vectors
    matioCpp::Vector<float> leftChannel("left");
    matioCpp::Vector<float> rightChannel("right");
    leftChannel = outputStereoBuffer.left;
    rightChannel = outputStereoBuffer.right;

    // Write to file
    {
    matioCpp::File file = matioCpp::File::Create("test-ism.mat");
    file.write(leftChannel);
    file.write(rightChannel);
    }

    return 0;
}

/// @brief  Setup the 3dti Toolkit core, listener, and ISM handler, exits if any files are not found
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
    ISMHandler->setupArbitraryRoom(trapezoidal);  // --> calls SourceImages::createImages(mainRoom, reflectionOrder)
    ISMHandler->setAbsortion(absortionsWalls);    // --> calls SourceImages::createImages(mainRoom, reflectionOrder)
    ISMHandler->setReflectionOrder(INITIAL_REFLECTION_ORDER);  // --> calls SourceImages::createImages(mainRoom, reflectionOrder)
    mainRoom = ISMHandler->getRoom(); 

    // Not sure the following is necessary, from here ...->
    ISMHandler->setMaxDistanceImageSources(INITIAL_DIST_SILENCED_FRAMES); // --> calls SourceImages::createImages(mainRoom, reflectionOrder)
    numberOfSilencedFrames = ISMHandler->calculateNumOfSilencedFrames(INITIAL_DIST_SILENCED_FRAMES);
    // <-... to here

    // Create an impulse source
    Common::CVector3 sourcePosition = {1.0, 0.0, 0.0};
    ISMHandler->setSourceLocation(sourcePosition);
    anechoicSourceDSP = myCore.CreateSingleSourceDSP();
    Common::CTransform sourceTransform;
    sourceTransform.SetPosition(sourcePosition);
    anechoicSourceDSP->SetSourceTransform(sourceTransform);
    anechoicSourceDSP->SetSpatializationMode(Binaural::TSpatializationMode::NoSpatialization);
    anechoicSourceDSP->DisableNearFieldEffect();
    anechoicSourceDSP->DisableAnechoicProcess();
    anechoicSourceDSP->DisableReverbProcess();

    // Disable all walls but one
    for (int i = 1; i < mainRoom.getWalls().size(); i++)
    {
        ISMHandler->disableWall(i); 	// --> calls SourceImages::createImages(mainRoom, reflectionOrder)
    }

    // Create Image Sources
    imageSourceDSPList = createImageSourceDSP();

    // Retrieve Image Source Data
    std::vector<ISM::ImageSourceData> data = ISMHandler->getImageSourceData();
    
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

/// @brief The app must create the actual DSP processors of the virtual sources
std::vector<shared_ptr<Binaural::CSingleSourceDSP>> createImageSourceDSP()
{
	std::vector<shared_ptr<Binaural::CSingleSourceDSP>> tempImageSourceDSPList;
	std::vector<Common::CVector3> imageSourceLocationList = ISMHandler->getImageSourceLocations();

	for (int i = 0; i < imageSourceLocationList.size(); i++)
	{
		shared_ptr<Binaural::CSingleSourceDSP> tempSourceDSP = myCore.CreateSingleSourceDSP();								// Creating audio source
		Common::CTransform sourcePosition;
		sourcePosition.SetPosition(imageSourceLocationList.at(i));
		tempSourceDSP->SetSourceTransform(sourcePosition);							//Set source position
		tempSourceDSP->SetSpatializationMode(Binaural::TSpatializationMode::HighQuality);	// Choosing high quality mode for anechoic processing
		tempSourceDSP->DisableNearFieldEffect();						// Audio source will not be close to listener, so we don't need near field effect
		tempSourceDSP->EnableAnechoicProcess();							// Enable anechoic processing for this source
		tempSourceDSP->DisableDistanceAttenuationAnechoic();			// THIS IS PARTICULAR OF THIS APP
		tempSourceDSP->EnablePropagationDelay();
		tempSourceDSP->DisableReverbProcess();   
		tempImageSourceDSPList.push_back(tempSourceDSP);
	}
	return tempImageSourceDSPList;
}


/// @brief  Process the audio buffers
void audioProcess(const CMonoBuffer<float>& input,  Common::CEarPair<CMonoBuffer<float>>& bufferOutput, int bufferSize)
{
    // Assert imageSourceData and imageSourceDSPList have the same size
    std::vector<ISM::ImageSourceData> imageSourceData = ISMHandler->getImageSourceData();
    if (imageSourceData.size() != imageSourceDSPList.size())
    {
        std::cout << "Error: imageSourceData and imageSourceDSPList have different sizes" << std::endl;
        exit(-1);
    }

    // Update listener position
    Common::CTransform listenerTransform = listener->GetListenerTransform();
    Common::CVector3 listenerPosition = listenerTransform.GetPosition();

    // Process images
    std::vector<CMonoBuffer<float>> imageBuffers;
    ISMHandler->proccess(input, imageBuffers, listenerPosition);

    // Assert imageBuffers and imageSourceDSPList have the same size
    if (imageBuffers.size() != imageSourceDSPList.size())
    {
        std::cout << "Error: imageBuffers and imageSourceDSPList have different sizes" << std::endl;
        exit(-1);
    }

    // Process reverb generated by the image sources
    for (int i = 0; i < imageSourceDSPList.size(); i++)
    {
        if (imageSourceData.at(i).visible)
        {
            Common::CEarPair<CMonoBuffer<float>> tempBufferOutput;
            imageSourceDSPList.at(i)->SetBuffer(imageBuffers.at(i));
            imageSourceDSPList.at(i)->ProcessAnechoic(tempBufferOutput.left, tempBufferOutput.right);
            bufferOutput.left += tempBufferOutput.left;
            bufferOutput.right += tempBufferOutput.right;
        }
    }

}
