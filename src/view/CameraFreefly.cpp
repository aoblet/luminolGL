#include "view/CameraFreefly.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

using namespace View;

void CameraFreefly::computeDirs() {
    // Spherical coordinates
    _front.x = glm::cos(_sphericalAngles.y) * glm::sin(_sphericalAngles.x);
    _front.y = glm::sin(_sphericalAngles.y);
    _front.z = glm::cos(_sphericalAngles.y) * glm::cos(_sphericalAngles.x);

    // Simply compute the left vector of front: rotate of _sphericalAngles.x + PI/2 with y equals to 0
    _left.x = glm::sin(_sphericalAngles.x + glm::half_pi<float>());
    _left.y = 0;
    _left.z = glm::cos(_sphericalAngles.x + glm::half_pi<float>());

    // Cross product to find _up from _front and _frontLeft
    _up = glm::normalize(glm::cross(_front, _left));

}

const glm::mat4& CameraFreefly::getViewMatrix() const {
    return _viewMatrix;
}

const glm::mat4& CameraFreefly::getProjectionMatrix() const {
    return _projectionMatrix;
}

void CameraFreefly::move(const glm::vec3& movement) {
    _eye += _front * movement.z + _left * movement.x + _up * movement.y;
}

void CameraFreefly::rotate(const glm::vec2& angles) {
    _sphericalAngles.x += angles.x;

    // Block y rotate
    if(glm::abs(_sphericalAngles.y + angles.y) < glm::half_pi<float>())
        _sphericalAngles.y += angles.y;
}
void CameraFreefly::update(const glm::vec2 &angles, const glm::vec3& movement) {
    rotate(angles);
    computeDirs();
    move(movement);
    _viewMatrix = glm::lookAt(_eye, _eye + _front, _up);
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

glm::vec3 CameraFreefly::getUp() const {
    return _up;
}

glm::vec3 CameraFreefly::getLeft() const {
    return _left;
}

void CameraFreefly::setFront(const glm::vec3 &front) {
    _front = front;
}

void CameraFreefly::updateFromTarget(const glm::vec3 &target) {
    _front = glm::normalize(target - _eye);
    _left.x = _front.x * glm::cos(glm::half_pi<float>()) - _front.z * glm::sin(glm::half_pi<float>());
    _left.y = 0;
    _left.z = _front.x * glm::sin(glm::half_pi<float>()) + _front.z * glm::cos(glm::half_pi<float>());

    _left = glm::normalize(_left);
    _up = glm::cross(_front, _left);
    _viewMatrix = glm::lookAt(_eye, _eye + _front, -_up);
}

std::ostream& View::operator<<(std::ostream& out, const CameraFreefly& c){
    out << "Camera" << std::endl;
    out << "----------------------" << std::endl;
    out << "Eye:\t\t "          << glm::to_string(c._eye)               << std::endl;
    out << "Angles:\t\t "       << glm::to_string(c._sphericalAngles)   << std::endl;
    out << "Front:\t\t "        << glm::to_string(c._front)             << std::endl;
    out << "Front Left:\t " << glm::to_string(c._left) << std::endl;
    out << "Up:\t\t\t "         << glm::to_string(c._up)                << std::endl;
    out << "----------------------" << std::endl << std::endl;
    return out;
}

CameraFreefly::CameraFreefly(const glm::vec2 &viewPort, const glm::vec2 &nearFar, float fov):
        _fov(fov), _viewPort(viewPort), _nearFar(nearFar){
    updateProjection();
}

void CameraFreefly::updateProjection(){
    _projectionMatrix = glm::perspective(_fov, _viewPort.x / _viewPort.y, _nearFar.x, _nearFar.y);
}

void CameraFreefly::updateProjectionProperties(const glm::vec2 &viewPort, const glm::vec2 &nearFar, float fov) {
    _viewPort = viewPort;
    _nearFar = nearFar;
    _fov = fov;
    updateProjection();
}

glm::vec2 CameraFreefly::getViewPort() const {
    return _viewPort;
}

glm::vec2 CameraFreefly::getNormalizedViewPort() const {
    float max = _viewPort.x > _viewPort.y ? _viewPort.x : _viewPort.y;
    return glm::vec2(_viewPort.x / max, _viewPort.y / max);
}

float CameraFreefly::getFOV() const {
    return _fov;
}

glm::vec2 CameraFreefly::getNearFar() const {
    return _nearFar;
}


float CameraFreefly::getFocal() const {
    return 1.f / (2.f * glm::tan(glm::radians(_fov) / 2.f));
}
