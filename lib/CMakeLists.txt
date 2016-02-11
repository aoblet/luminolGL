target_include_directories(${PROJECT_EXE} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB IMGUI_SRC imgui/*.cpp)
file(GLOB STB_SRC stb/*.c)

add_library(IMGUI_LIBRARY STATIC ${IMGUI_SRC})
add_library(STB_LIBRARY STATIC ${STB_SRC})
