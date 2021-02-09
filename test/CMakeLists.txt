cmake_minimum_required(VERSION 3.0)

project(ObjectTrack_Test)
set(target objecttrack_test)
set(CMAKE_CXX_STANDARD 11)	

find_package(PkgConfig REQUIRED)  # for gstreamer pkg_check_modules
find_package(Threads REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0 gstreamer-controller-1.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR} 
                    ${GSTREAMER_INCLUDE_DIRS} 
                    )

# FYI these are the gstreamer include dirs
# /usr/include/gstreamer-1.0;
# /usr/include/orc-0.4;
# /usr/include/gstreamer-1.0;
# /usr/include/glib-2.0;
# /usr/lib/aarch64-linux-gnu/glib-2.0/include
message(STATUS " gstreamer include dirs: ${GSTREAMER_INCLUDE_DIRS}")

add_executable(${target} objecttrack_test.cpp)

target_link_libraries(${target} ${GSTREAMER_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

