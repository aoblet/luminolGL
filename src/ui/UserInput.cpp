#include "ui/UserInput.hpp"

using namespace UI;

UserInput::UserInput(float _velocityRotate): velocityRotate(_velocityRotate) { }

void UserInput::update(GLFWwindow *window) {
    is_PRESSED_Q = glfwGetKey(window, GLFW_KEY_A ) == GLFW_PRESS;
    is_PRESSED_Z = glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS;
    is_PRESSED_S = glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS;
    is_PRESSED_D = glfwGetKey(window, GLFW_KEY_D ) == GLFW_PRESS;
    is_PRESSED_SPACE = glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS;
    is_PRESSED_ALT = glfwGetKey(window, GLFW_KEY_LEFT_ALT ) == GLFW_PRESS;
    velocityMovement = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 2.f : 1;

    if(glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS){
        glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
        rotate.x = glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS ? -1 : (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS ? 1 : 0);
        rotate.y = glfwGetKey(window, GLFW_KEY_UP ) == GLFW_PRESS ? 1 : (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS ? -1 : 0);
    }
    else{
        glm::dvec2 dXY, tmpDXY;
        glfwGetCursorPos(window, &dXY.x, &dXY.y);
        tmpDXY = mousePosition - dXY;
        rotate = tmpDXY;
        mousePosition = dXY;
    }
    rotate *= velocityRotate;

}

void UserInput::setVelocityRotate(float v) {
    velocityRotate = v;
}

float& UserInput::getVelocityRotate() {
    return velocityRotate;
}
