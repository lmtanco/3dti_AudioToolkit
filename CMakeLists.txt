cmake_minimum_required(VERSION 3.5)
project(3dti_AudioToolkit)

set(CMAKE_CXX_STANDARD 17) # Not sure if this is necessary

#find all the source files in 3dti_Toolkit/Common subdirectory
file(GLOB_RECURSE SOURCES_COMMON "3dti_Toolkit/Common/*.cpp" "3dti_Toolkit/Common/*.h")
file(GLOB_RECURSE SOURCES_BINAURAL "3dti_Toolkit/BinauralSpatializer/*.cpp" "3dti_Toolkit/BinauralSpatializer/*.h")
file(GLOB_RECURSE SOURCES_ISM "3dti_Toolkit/ISM/*.cpp" "3dti_Toolkit/ISM/*.h"    )

file(GLOB_RECURSE SOURCES_HRTF "3dti_ResourceManager/HRTF/*.cpp" "3dti_ResourceManager/HRTF/*.h"    )
file(GLOB_RECURSE SOURCES_BRIR "3dti_ResourceManager/BRIR/*.cpp" "3dti_ResourceManager/BRIR/*.h"    )

#file(GLOB_RECURSE SOURCES_HAHL "3dti_Toolkit/HAHLSimulation/*.cpp" "3dti_Toolkit/HAHLSimulation/*.h") 

#print out the contents of the SOURCES_TOOLKIT variable to see if it is correct
# message(STATUS "SOURCES_TOOLKIT: ${SOURCES_TOOLKIT}")
#print out the contentes of the SOURCES_RESOURCE_MANAGER variable to see if it is correct
# message(STATUS "SOURCES_RESOURCE_MANAGER: ${SOURCES_RESOURCE_MANAGER}")

#The 3dti_Toolkit library is the main target of this CMakeLists.txt file
add_library(threedti-common ${SOURCES_COMMON})
add_library(threedti-binaural ${SOURCES_BINAURAL})
add_library(threedti-ism ${SOURCES_ISM}   )
add_library(threedti-hrtf ${SOURCES_HRTF}   )
add_library(threedit-brir ${SOURCES_BRIR}   )
#add_library(threedti-hahl ${SOURCES_HAHL})

target_include_directories(threedti-common 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
)

target_include_directories(threedti-binaural 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
)

target_include_directories(threedti-ism 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
)

# Fetch our fork of libsofa
include(FetchContent)
option(BUILD_SHARED_LIBS "By default link libsofa statically" OFF)
FetchContent_Declare(
    libsofa
    GIT_REPOSITORY "https://github.com/lmtanco/API_Cpp.git"
    GIT_TAG origin/cmake
    GIT_PROGRESS true
)
FetchContent_MakeAvailable(libsofa)

target_include_directories(threedti-hrtf 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
    PUBLIC 3dti_ResourceManager/third_party_libraries/cereal/include
    PRIVATE ${libsofa_SOURCE_DIR}/src
)

target_include_directories(threedit-brir 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
    PUBLIC 3dti_ResourceManager/third_party_libraries/cereal/include
    PRIVATE ${libsofa_SOURCE_DIR}/src
)

target_link_libraries(threedti-ism
    PUBLIC threedti-common
    PUBLIC threedti-binaural
)

target_link_libraries(threedti-hrtf
    PRIVATE libsofa
)

target_link_libraries(threedit-brir
    PRIVATE libsofa
)

# target_include_directories(threedti-hahl 
#     PUBLIC 3dti_Toolkit
#     PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
# )

find_package(matioCpp REQUIRED)

# Declare a list of executables (this is for the filters)
set(TESTS 01 02 03 04 05 )  

foreach(TEST ${TESTS})
    add_executable(test${TEST} test/01_filters/testFilter-${TEST}.cpp)  # Añade el ejecutable
    target_link_libraries(test${TEST} 
        PRIVATE threedti-common
        PRIVATE matioCpp::matioCpp)  # Enlaza las bibliotecas
endforeach()

# Find rtaudio library
find_package(RtAudio REQUIRED)


# Declare tests for Binaural spatialization. 
add_executable(testBinauralSpatializer test/02_binaural/testBinauralSpatializer.cpp)
target_link_libraries(testBinauralSpatializer 
    PRIVATE threedti-common
    PRIVATE threedti-binaural
    PRIVATE threedti-hrtf
    PRIVATE threedit-brir
   # PRIVATE matioCpp::matioCpp
    PRIVATE RtAudio::rtaudio )  # Enlaza las bibliotecas

#Find pugixml library
find_package(pugixml REQUIRED)

# Declare tests for Image Source Method testing
add_executable(testISM test/03_ISM/testISM.cpp)
target_link_libraries(testISM 
    PRIVATE threedti-ism
    PRIVATE threedti-hrtf
    PRIVATE threedit-brir
    PRIVATE pugixml::pugixml
    PRIVATE RtAudio::rtaudio )  # Enlaza las bibliotecas