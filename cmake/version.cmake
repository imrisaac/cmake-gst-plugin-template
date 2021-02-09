message(STATUS "Incrementing Version")

set(_commit_hash "unknown")

# Build number incrementing
file(READ ${top_dir}/src/version.h.in ver)

string(REGEX MATCH "VERSION_RELEASE ([0-9]*)" _ ${ver})
set(ver_release ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MAJOR ([0-9]*)" _ ${ver})
set(ver_major ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR ([0-9]*)" _ ${ver})
set(ver_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH ([0-9]*)" _ ${ver})
set(ver_patch ${CMAKE_MATCH_1})

set(prev_patch_ver ${ver_patch})

if(AUTO_VERSION)
  # increment the build number
  #MATH(EXPR ver_patch "${ver_patch}+1")
endif()

#STRING(REGEX REPLACE <pattern> <replacement string> <target variable> <source string>)

string(REGEX REPLACE "VERSION_PATCH ${prev_patch_ver}" "VERSION_PATCH ${ver_patch}" new_ver ${ver})

file(WRITE ${top_dir}/src/version.h.in ${new_ver})

message(STATUS "This version: ${ver_release}.${ver_major}.${ver_minor}.${ver_patch}")
file(WRITE .version_string "${ver_release}.${ver_major}.${ver_minor}.${ver_patch}")

# Version variables
set(_version_release ${ver_release})
set(_version_major ${ver_major})
set(_version_minor ${ver_minor})
set(_version_patch ${ver_patch})

message(STATUS "Resolving git hash")
find_package(Git)

if(GIT_FOUND)

  # get commit hash
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _commit_hash
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # get commit date and time
  execute_process(
    COMMAND ${GIT_EXECUTABLE} show -s --format=%ci
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _commit_date
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  # get branch name
  execute_process(
    COMMAND ${GIT_EXECUTABLE} symbolic-ref --short HEAD
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _build_branch
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  
  # get username
  execute_process(
    COMMAND ${GIT_EXECUTABLE} config user.name
    WORKING_DIRECTORY "${local_dir}"
    OUTPUT_VARIABLE _user_name
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  message( STATUS "Commit hash  ${_commit_hash}")
  message( STATUS "Commit date  ${_commit_date}")
  message( STATUS "Build branch ${_build_branch}")
  message( STATUS "Built by     ${_user_name}")

else()
  message(STATUS "GIT not found")
endif()

string(TIMESTAMP _time_stamp)

configure_file(${top_dir}/src/version.h.in ${build_dir}/version.h @ONLY)
configure_file(${top_dir}/control.in ${build_dir}/control @ONLY)
