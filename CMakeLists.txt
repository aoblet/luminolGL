cmake_minimum_required (VERSION 2.8.11)
project (luminolGL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${GLM_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OpenGL_INCLUDE_DIR})
include_directories(include)

file(GLOB SRC_FILES "src/*.cpp")
add_executable(luminolGL ${SRC_FILES})
