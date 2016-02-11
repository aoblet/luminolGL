cmake_minimum_required (VERSION 2.8.11)
project (luminolGL)

set(PROJECT_EXE luminolGL)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLFW REQUIRED)

include_directories(${GLM_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${OpenGL_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
include_directories(include)


file(GLOB SRC_FILES "src/*.cpp")
add_executable(${PROJECT_EXE} ${SRC_FILES})

add_subdirectory(lib)
target_link_libraries(${PROJECT_EXE} ${GLFW_LIBRARY} ${OpenGL_LIBRARY} ${GLEW_LIBRARY})
target_link_libraries(${PROJECT_EXE} IMGUI_LIBRARY STB_LIBRARY)
