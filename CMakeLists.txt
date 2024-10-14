cmake_minimum_required(VERSION 3.5)
project(3dti_AudioToolkit)

set(CMAKE_CXX_STANDARD 17) # Not sure if this is necessary

########################################### LIBRARIES ###########################################

# Find out what libraries we are going to build
option(BUILD_COMMON "Build the common library" ON)
option(BUILD_BINAURAL "Build the binaural library" ON)
option(BUILD_ISM "Build the ISM library" ON)
option(BUILD_HRTF "Build the HRTF library" ON)
option(BUILD_BRIR "Build the BRIR library" ON)
option(BUILD_HAHL "Build the HAHL library" ON)

#find all the source files in 3dti_Toolkit/Common subdirectory according to selected options

# Common library
if (BUILD_COMMON)
    file(GLOB_RECURSE SOURCES_COMMON "3dti_Toolkit/Common/*.cpp" "3dti_Toolkit/Common/*.h")
    add_library(threedti-common ${SOURCES_COMMON})
    target_include_directories(threedti-common 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer)
endif()

# Binaural spatializer library
if (BUILD_BINAURAL)
    file(GLOB_RECURSE SOURCES_BINAURAL "3dti_Toolkit/BinauralSpatializer/*.cpp" "3dti_Toolkit/BinauralSpatializer/*.h")
    add_library(threedti-binaural ${SOURCES_BINAURAL})
    target_include_directories(threedti-binaural 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer)
endif()

# Image Source Method library
if (BUILD_ISM)
    file(GLOB_RECURSE SOURCES_ISM "3dti_Toolkit/ISM/*.cpp" "3dti_Toolkit/ISM/*.h")
    add_library(threedti-ism ${SOURCES_ISM})
    target_include_directories(threedti-ism 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer)
    target_link_libraries(threedti-ism
        PUBLIC threedti-common
        PUBLIC threedti-binaural)
endif()

# If we are going to build the HRTF library or the BRIR library, we need to fetch our fork of libsofa
if (BUILD_HRTF OR BUILD_BRIR)
    include(FetchContent)
    option(BUILD_SHARED_LIBS "By default link libsofa statically" OFF)
    FetchContent_Declare(
        libsofa
        GIT_REPOSITORY "https://github.com/lmtanco/API_Cpp.git"
        GIT_TAG origin/cmake
        GIT_PROGRESS true
    )
    FetchContent_MakeAvailable(libsofa)
endif()

# HRTF library
if (BUILD_HRTF)
    file(GLOB_RECURSE SOURCES_HRTF "3dti_ResourceManager/HRTF/*.cpp" "3dti_ResourceManager/HRTF/*.h")
    add_library(threedti-hrtf ${SOURCES_HRTF})
    target_include_directories(threedti-hrtf 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
        PUBLIC 3dti_ResourceManager/third_party_libraries/cereal/include
        PRIVATE ${libsofa_SOURCE_DIR}/src)
    target_link_libraries(threedti-hrtf
        PRIVATE libsofa)
endif()

# BRIR library
if (BUILD_BRIR)
    file(GLOB_RECURSE SOURCES_BRIR "3dti_ResourceManager/BRIR/*.cpp" "3dti_ResourceManager/BRIR/*.h")
    add_library(threedti-brir ${SOURCES_BRIR})
    target_include_directories(threedti-brir 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
        PUBLIC 3dti_ResourceManager/third_party_libraries/cereal/include
        PRIVATE ${libsofa_SOURCE_DIR}/src)
    target_link_libraries(threedti-brir
        PRIVATE libsofa)
endif()

# Hearing Aid - Hearing Loss library
if (BUILD_HAHL)
    file(GLOB_RECURSE SOURCES_HAHL "3dti_Toolkit/HAHLSimulation/*.cpp" "3dti_Toolkit/HAHLSimulation/*.h")
    add_library(threedti-hahl ${SOURCES_HAHL})
    target_include_directories(threedti-hahl 
        PUBLIC 3dti_Toolkit
        PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
        PUBLIC 3dti_ResourceManager/third_party_libraries/eigen)
    target_link_libraries(threedti-hahl
        PUBLIC threedti-common)
endif()

########################################### TESTS ###########################################

find_package(matioCpp REQUIRED)

# Declare a list of executables 
option(BUILD_FILTER_TESTS "Build the filter tests" ON)
option(BUILD_BINAURAL_TESTS "Build the binaural tests" ON)
option(BUILD_ISM_TESTS "Build the ISM tests" ON)
option(BUILD_HL_TESTS "Build the hearing loss tests" ON)


if (BUILD_FILTER_TESTS)
    # Tests for the filters
    set(TESTS 01 02 03 04 05 06 07)  
    foreach(TEST ${TESTS})
        add_executable(test${TEST} test/01_filters/testFilter-${TEST}.cpp)  # Añade el ejecutable
        target_link_libraries(test${TEST} 
            PRIVATE threedti-common
            PRIVATE matioCpp::matioCpp)  # Enlaza las bibliotecas
    endforeach()
endif()

if (BUILD_BINAURAL_TESTS)
    # Find rtaudio library
    find_package(RtAudio REQUIRED)

    # Declare tests for Binaural spatialization. 
    add_executable(testBinauralSpatializer test/02_binaural/testBinauralSpatializer.cpp)
    target_link_libraries(testBinauralSpatializer 
        PRIVATE threedti-common
        PRIVATE threedti-binaural
        PRIVATE threedti-hrtf
        PRIVATE threedti-brir
        # PRIVATE matioCpp::matioCpp
        PRIVATE RtAudio::rtaudio )  # Enlaza las bibliotecas
endif()

if (BUILD_ISM_TESTS)
    #Find pugixml library
    find_package(pugixml REQUIRED)

    # Declare tests for Image Source Method testing
    add_executable(testISM test/03_ISM/testISM.cpp)
    target_link_libraries(testISM 
        PRIVATE threedti-ism
       PRIVATE threedti-hrtf
       #PRIVATE threedti-brir
       PRIVATE pugixml::pugixml
       PRIVATE matioCpp::matioCpp
       #PRIVATE RtAudio::rtaudio 
    )  # Enlaza las bibliotecas
endif()

if (BUILD_HL_TESTS)
    include (CTest)
    enable_testing()
    # Declare tests for Hearing Loss simulation
    set(TESTSHL 01)  
    foreach(TEST ${TESTSHL})
        add_executable(testHL-${TEST} test/04_HL/testHL-${TEST}.cpp)  # Añade el ejecutable
        target_include_directories(testHL-${TEST} 
            PRIVATE test/04_HL) # For dr_wav.h
        target_link_libraries(testHL-${TEST} 
            PRIVATE threedti-hahl
            PRIVATE threedti-common
            PRIVATE matioCpp::matioCpp)  # Enlaza las bibliotecas

        # Copy the input file sweep_4seg.wav to the executable directory
        add_custom_command(TARGET testHL-${TEST} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/resources/AudioSamples/sweep_4seg.wav
            $<TARGET_FILE_DIR:testHL-${TEST}>/sweep_4seg.wav) # This would be build/Debug/  (if we are in Debug mode)
            #${CMAKE_CURRENT_BINARY_DIR}/sweep.wav) # This would be build/

        add_test(NAME testHL-${TESTSHL} COMMAND testHL-${TESTSHL} WORKING_DIRECTORY $<TARGET_FILE_DIR:testHL-${TEST}>)
    endforeach()
endif()