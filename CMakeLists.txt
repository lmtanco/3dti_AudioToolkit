cmake_minimum_required(VERSION 3.5)
project(3dti_AudioToolkit)

set(CMAKE_CXX_STANDARD 11) # Not sure if this is necessary

#find all the source files in 3dti_Toolkit/Common subdirectory
file(GLOB_RECURSE SOURCES_COMMON "3dti_Toolkit/Common/*.cpp" "3dti_Toolkit/Common/*.h") 

#print out the contents of the SOURCES_TOOLKIT variable to see if it is correct
# message(STATUS "SOURCES_TOOLKIT: ${SOURCES_TOOLKIT}")
#print out the contentes of the SOURCES_RESOURCE_MANAGER variable to see if it is correct
# message(STATUS "SOURCES_RESOURCE_MANAGER: ${SOURCES_RESOURCE_MANAGER}")

#The 3dti_Toolkit library is the main target of this CMakeLists.txt file
add_library(threedti-common ${SOURCES_COMMON})

target_include_directories(threedti-common 
    PUBLIC 3dti_Toolkit
    PUBLIC 3dti_ResourceManager/third_party_libraries/boost_circular_buffer
)

find_package(matioCpp REQUIRED)

# Add the test01 executable 
add_executable(test01 test/testFilter-01.cpp)
target_link_libraries(test01 
    PRIVATE threedti-common
    PRIVATE matioCpp::matioCpp)


