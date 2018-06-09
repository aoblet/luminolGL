cmake_minimum_required (VERSION 2.8.11)
project (luminolGL)

set(PROJECT_EXE luminolGL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ffast-math -Wall")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmakeModules/)
set(CMAKE_CXX_STANDARD 11)


# Find packages
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)
find_package(GLOG REQUIRED)
find_package(RapidJSON REQUIRED)

# Include external headers
include_directories(
    ${GLM_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${OpenGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${assimp_INCLUDE_DIRS}
    ${GLOG_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDES}
)

# Include internal headers
include_directories(include)

# Set project source files
file(GLOB_RECURSE SRC_FILES src/*.cpp include/*.hpp include/*.h)
add_executable(${PROJECT_EXE} ${SRC_FILES})

# Load libraries
add_subdirectory(lib)

# Link libraries with luminolGL
target_link_libraries(
    ${PROJECT_EXE}
    ${GLFW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${assimp_LIBRARIES}
    ${GLOG_LIBRARIES}
)

target_link_libraries(${PROJECT_EXE} IMGUI_LIBRARY STB_LIBRARY)
