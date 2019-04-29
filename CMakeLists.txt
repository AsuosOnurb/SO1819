cmake_minimum_required(VERSION 2.8)

project(SO1819 C)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)

# Common source files
file(GLOB COMMON_SOURCES src/common/*)
# MA source files
file(GLOB MA_SOURCES src/ma/*)
# SV source files
file(GLOB SV_SOURCES src/sv/*)
# CV source files
file(GLOB CV_SOURCES src/cv/*)
# AG source files
file(GLOB AG_SOURCES src/ag/*)

# Executable targets
add_executable(ma ${COMMON_SOURCES} ${MA_SOURCES})
add_executable(sv ${COMMON_SOURCES} ${SV_SOURCES})
add_executable(cv ${COMMON_SOURCES} ${CV_SOURCES})
add_executable(ag ${COMMON_SOURCES} ${AG_SOURCES})
