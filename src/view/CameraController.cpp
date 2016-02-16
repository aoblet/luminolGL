#include "view/CameraController.hpp"

using namespace View;

CameraController::CameraController(CameraFreefly &camera, UI::UserInput& userInput, float velocitySplines, bool isSpectator):
        _camera(camera), _userInput(userInput), _isSpectatorMode(isSpectator), _velocitySplines(velocitySplines){}

void CameraController::setCamera(CameraFreefly &c) {
    _camera = c;
}

void CameraController::update(float time) {
    _isSpectatorMode ? updateFromInput(_userInput) : updateFromSplines(time);
}

CameraFreefly &CameraController::camera() const {
    return _camera;
}

void CameraController::updateFromSplines(float time) {
    float indexSpline = glm::mod(time*0.005f, 1.f);
    float speed = _cameraSpeeds.size() ? _cameraSpeeds.cubicInterpolation(indexSpline).y : 1;
    speed *= _velocitySplines;
    _camera.setEye(_cameraPositions.cubicInterpolation(glm::mod(time*speed, 1.f)));
    _camera.updateFromTarget(_cameraViewTargets.cubicInterpolation(indexSpline));
}

void CameraController::updateFromInput(const UI::UserInput& userInput) {
    glm::vec3 movement;
    movement.z = userInput.is_PRESSED_Z ? 1 : (userInput.is_PRESSED_S ? -1 : 0) ;
    movement.x  = userInput.is_PRESSED_Q ? 1 : (userInput.is_PRESSED_D ? -1 : 0) ;
    movement.y  = userInput.is_PRESSED_SPACE ? 1 : (userInput.is_PRESSED_ALT ? -1 : 0) ;
    movement *= 0.05f;
    _camera.update(userInput.rotate * userInput.velocityRotate, movement * userInput.velocityMovement);
}

Geometry::Spline3D& CameraController::positions() {
    return _cameraPositions;
}

Geometry::Spline3D& CameraController::speeds() {
    return _cameraSpeeds;
}

Geometry::Spline3D& CameraController::viewTargets() {
    return _cameraViewTargets;
}

void CameraController::setSpectator(bool isSpectator) {
    _isSpectatorMode = isSpectator;
}

bool CameraController::isSpectator() const {
    return _isSpectatorMode;
}

void CameraController::setUserInput(UI::UserInput &u) {
    _userInput = u;
}

UI::UserInput &CameraController::userInput() const {
    return _userInput;
}

void CameraController::setVelocitySplines(float velocity) {
    _velocitySplines = velocity;
}

float CameraController::velocitySplines() const {
    return _velocitySplines;
}

float& CameraController::velocitySplines() {
    return _velocitySplines;
}
