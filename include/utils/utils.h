#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>


void printMat4(const glm::mat4 & matrix);
void printVec3(const glm::vec3 & vec);
void printVec4(const glm::vec4 & vec);

void checkErrorGL(const std::string& title);
void initContextWindowGL(const std::string& windowTitle, GLFWwindow*& window, int& DPI, const glm::ivec2& viewport);