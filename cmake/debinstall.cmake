message(STATUS "Installing package")


# Read version sring created by version.cmake
file(READ .version_string VERSION_STRING)

set(BIN_NAME "libgstobjecttrack")
set(DEB_PACKAGE_NAME "${BIN_NAME}-${VERSION_STRING}-aarch64-main-release.deb")

execute_process(
  COMMAND bash -c "dpkg -i ${DEB_PACKAGE_NAME}"
)

message(STATUS "Package generation complete")
