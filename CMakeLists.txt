cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(VideoPlayer VERSION 1.0.0)

# Add all source files inside src (recursively)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Set up the mod binary
add_library(${PROJECT_NAME} SHARED ${SOURCES})

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()


# Set up dependencies, resources, and link Geode.

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# add ffmpeg packages

set(FFMPEG_DIR ${CMAKE_SOURCE_DIR}/lib/ffmpeg)

find_library(AVCODEC_LIB NAMES avcodec PATHS ${FFMPEG_DIR}/lib)
find_library(AVDEVICE_LIB NAMES avdevice PATHS ${FFMPEG_DIR}/lib)
find_library(AVFILTER_LIB NAMES avfilter PATHS ${FFMPEG_DIR}/lib)
find_library(AVFORMAT_LIB NAMES avformat PATHS ${FFMPEG_DIR}/lib)
find_library(AVUTIL_LIB NAMES avutil PATHS ${FFMPEG_DIR}/lib)
find_library(SWRESAMPLE_LIB NAMES swresample PATHS ${FFMPEG_DIR}/lib)
find_library(SWSCALE_LIB NAMES swscale PATHS ${FFMPEG_DIR}/lib)

# add openCV and onnx packages

# find_package(OpenCV REQUIRED)

find_library(ONNX_RUNTIME_LIB NAMES onnxruntime PATHS ${CMAKE_SOURCE_DIR}/lib/onnxruntime/lib)

# target include

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${FFMPEG_DIR}/include 
    lib/onnxruntime/include 
    # ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
    ${AVCODEC_LIB}
    ${AVDEVICE_LIB}
    # ${AVFILTER_LIB}
    ${AVFORMAT_LIB}
    ${AVUTIL_LIB}
    # ${SWRESAMPLE_LIB}
    ${SWSCALE_LIB}


    ${ONNX_RUNTIME_LIB}
    
    # ${OpenCV_LIBRARIES}
)

setup_geode_mod(${PROJECT_NAME})
