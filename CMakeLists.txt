cmake_minimum_required(VERSION 3.0.0)

project(GstObjectTrack)
set(target boilerplatevideofilter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindGstreamer.cmake)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

find_package(PkgConfig REQUIRED)  
pkg_check_modules(GSTREAMER REQUIRED
                  gstreamer-1.0>=1.6 
                  gstreamer-video-1.0>=1.6
                  gstreamer-app-1.0>=1.4)


# this is kinda jetson specific, and at lease aarch64 specific
include_directories(${CMAKE_BINARY_DIR}         # this is the build dir, where the generated version.h file is located
                    ${GSTREAMER_INCLUDE_DIRS}
                    ${GSTREAMER_VIDEO_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
                    )

link_directories(/usr/lib/aarch64-linux-gnu/gstreamer-1.0)

file(GLOB_RECURSE SOURCES "src/*.cpp")
#file(GLOB SOURCES "src/*.cpp")

LIST(APPEND SOURCES ${GST_SOURCE})
LIST(APPEND SOURCES ${GST_LIBS_SOURCE})
LIST(APPEND SOURCES ${GST_PARSE})

add_library(${target} SHARED ${SOURCES})

target_link_libraries(${target}
                      ${GSTREAMER_LIBRARIES}
                      ${GSTREAMER_VIDEO_LIBRARIES}
)

# Versioning 
add_custom_target(PRE_BUILD_VERSIONING
  COMMAND ${CMAKE_COMMAND}
    -Dtop_dir="${CMAKE_CURRENT_SOURCE_DIR}"               
    -Dbuild_dir="${CMAKE_CURRENT_BINARY_DIR}"
    -DAUTO_VERSION="${AUTO_VERSION}"             
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cmake"        
)

add_dependencies(${target} PRE_BUILD_VERSIONING)

# Debian Package Generation
add_custom_target(debgen
  COMMAND ${CMAKE_COMMAND}
    -DTOP_DIR="${CMAKE_CURRENT_SOURCE_DIR}"               
    -Dbuild_dir="${CMAKE_CURRENT_BINARY_DIR}"
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"             
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/packagegen.cmake"        
)

# Debian Package Install
add_custom_target(debinstall
  COMMAND ${CMAKE_COMMAND}
    -DTOP_DIR="${CMAKE_CURRENT_SOURCE_DIR}"               
    -Dbuild_dir="${CMAKE_CURRENT_BINARY_DIR}"
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"             
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/debinstall.cmake"        
)

