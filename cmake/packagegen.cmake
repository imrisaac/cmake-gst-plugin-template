cmake_minimum_required(VERSION 3.0.0)

message(STATUS "Running package generator")

# Read version sring created by version.cmake
file(READ .version_string VERSION_STRING)

message(STATUS "creating main and dev packages")
set(BIN_NAME "libgstobjecttrack")
set(PACKAGE_NAME "${BIN_NAME}-${VERSION_STRING}-aarch64-main")
set(PACKAGE_NAME_DEV "${BIN_NAME}-${VERSION_STRING}-aarch64-dev")

# main package
file(REMOVE_RECURSE ${PACKAGE_NAME})
file(MAKE_DIRECTORY ${PACKAGE_NAME})
file(MAKE_DIRECTORY "${PACKAGE_NAME}/DEBIAN")
file(MAKE_DIRECTORY "${PACKAGE_NAME}/usr")
file(MAKE_DIRECTORY "${PACKAGE_NAME}/usr/lib")
file(MAKE_DIRECTORY "${PACKAGE_NAME}/usr/lib/aarch64-linux-gnu")
file(MAKE_DIRECTORY "${PACKAGE_NAME}/usr/lib/aarch64-linux-gnu/gstreamer-1.0")

message(STATUS "assembling package components")

# move latest build to package module for comitting to remote
file(COPY "${BIN_NAME}.so" DESTINATION "${PACKAGE_NAME}/usr/lib/aarch64-linux-gnu/gstreamer-1.0/")
file(COPY "control" DESTINATION "${PACKAGE_NAME}/DEBIAN")

execute_process(
  COMMAND bash -c "dpkg-deb --build ${PACKAGE_NAME}"
)

message(STATUS "Package generation complete")
