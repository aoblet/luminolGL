#include "view/CameraFreefly.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

using namespace View;

void CameraFreefly::computeDirs() {
    // Spherical coordinates
    _front.x = glm::cos(_sphericalAngles.y) * glm::cos(_sphericalAngles.x);
    _front.y = glm::sin(_sphericalAngles.y);
    _front.z = glm::cos(_sphericalAngles.x) * glm::cos(_sphericalAngles.y);

    // Simply compute the left vector of front: rotate of _sphericalAngles.x + PI/2 with y equals to 0
    _frontLeft.x = glm::sin(_sphericalAngles.x + glm::half_pi<float>());
    _frontLeft.y = 0;
    _frontLeft.z = glm::cos(_sphericalAngles.x + glm::half_pi<float>());

    // Cross product to find _up from _front and _frontLeft
    _up = glm::cross(_front, _frontLeft);

}

const glm::mat4& CameraFreefly::getViewMatrix() const {
    return _matrix;
}

void CameraFreefly::moveFront(float speed) {
    _eye += (_front * speed);
}

void CameraFreefly::moveLeft(float speed) {
    _eye += (_frontLeft * speed);
}

void CameraFreefly::rotate(const glm::vec2& angles) {
    _sphericalAngles += angles;
}

void CameraFreefly::update(const glm::vec2 &angles, float speedFront, float speedLeft) {
    rotate(angles);
    computeDirs();
    moveFront(speedFront);
    moveLeft(speedLeft);
    _matrix = glm::lookAt(_eye, _eye + _front, _up);
}

void CameraFreefly::setEye(const glm::vec3 &eye) {
    _eye = eye;
}

glm::vec3 CameraFreefly::getEye() const {
    return _eye;
}

glm::vec3 CameraFreefly::getFront() const {
    return _front;
}

void CameraFreefly::setFront(const glm::vec3 &front) {
    _front = front;
}

void CameraFreefly::updateFromTarget(const glm::vec3 &target) {
    _front = glm::normalize(target - _eye);
    _frontLeft.x = _front.x * glm::cos(glm::half_pi<float>()) - _front.z * glm::sin(glm::half_pi<float>());
    _frontLeft.y = 0;
    _frontLeft.z = _front.x * glm::sin(glm::half_pi<float>()) + _front.z * glm::cos(glm::half_pi<float>());

    _frontLeft = glm::normalize(_frontLeft);
    _up = glm::cross(_front, _frontLeft);
    _matrix = glm::lookAt(_eye, _eye + _front, -_up);
}

std::ostream& View::operator<<(std::ostream& out, const CameraFreefly& c){
    out << "Camera" << std::endl;
    out << "----------------------" << std::endl;
    out << "Eye:\t\t "          << glm::to_string(c._eye)               << std::endl;
    out << "Angles:\t\t "       << glm::to_string(c._sphericalAngles)   << std::endl;
    out << "Front:\t\t "        << glm::to_string(c._front)             << std::endl;
    out << "Front Left:\t "     << glm::to_string(c._frontLeft)         << std::endl;
    out << "Up:\t\t\t "         << glm::to_string(c._up)                << std::endl;
    out << "----------------------" << std::endl << std::endl;
    return out;
}
