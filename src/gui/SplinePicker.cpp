#include "gui/SplinePicker.hpp"

using namespace Gui;

SplinePicker::SplinePicker(Geometry::Spline3D &positions, Geometry::Spline3D &targets, Geometry::Spline1D &velocities):
    _positions(positions), _targets(targets), _velocities(velocities), _state(SplineState::position), _yPlaneIntersection(10){}

void SplinePicker::addPointFromUserClick(Geometry::Spline3D &spline, const glm::vec2 &cursorPosition, const View::CameraFreefly &camera) {
    glm::vec3 front = camera.getFront();
    float mouseNormX = ((cursorPosition.x / camera.getViewPort().x) * 2 * camera.getNormalizedViewPort().x - 1 * camera.getNormalizedViewPort().x);
    float mouseNormY = ((cursorPosition.y / camera.getViewPort().y) * 2 * camera.getNormalizedViewPort().y - 1 * camera.getNormalizedViewPort().y);

    glm::vec3 left = - camera.getLeft() * mouseNormX;
    glm::vec3 up = camera.getUp() * mouseNormY;
    glm::vec3 rayDir = glm::normalize(front + left + up);
    glm::vec3 rayOrigin = camera.getEye();
    glm::vec3 point = ((rayOrigin.y - _yPlaneIntersection) / (glm::dot(glm::vec3(0, -1, 0), rayDir))) * rayDir ; // pythagore
    point += rayOrigin;
    spline.add(point);
}

void SplinePicker::pick(const glm::vec2 &cursorPosition, const View::CameraFreefly &camera, bool isClicked) {

    if(!_mouseDown && isClicked){
        if(_state == SplineState::position)
            addPointFromUserClick(_positions, cursorPosition, camera);
        else if(_state == SplineState::target)
            addPointFromUserClick(_targets, cursorPosition, camera);
        else if(_state == SplineState::velocity){
            //TODO
        }
    }

    _mouseDown = isClicked;
}

float &SplinePicker::yPlaneIntersection() {
    return _yPlaneIntersection;
}

void SplinePicker::setState(const SplineState &state) {
    _state = state;
}

SplineState SplinePicker::state() const {
    return _state;
}

Geometry::Spline3D &SplinePicker::positions() {
    return _positions;
}

Geometry::Spline3D &SplinePicker::targets() {
    return _targets;
}

Geometry::Spline1D &SplinePicker::velocities() {
    return _velocities;
}
