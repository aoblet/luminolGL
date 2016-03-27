#pragma once

#include "geometry/Spline.h"
#include "view/CameraFreefly.hpp"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <map>


namespace Gui{

    /** Current SplinePicker writing state */
    enum class SplineState{ position, velocity, target};

    const std::map<SplineState, std::string> SplineStateString({
            {SplineState::position, "position"},
            {SplineState::velocity, "velocity"},
            {SplineState::target, "target"},
    });

    class SplinePicker {
        Geometry::Spline3D & _positions;
        Geometry::Spline3D & _targets;
        Geometry::Spline1D & _velocities;

        SplineState _state;
        bool _mouseDown;
        bool _mouseClick;
        float _yPlaneIntersection;

    public:
        SplinePicker(Geometry::Spline3D& positions, Geometry::Spline3D& targets, Geometry::Spline1D& velocities);
        void addPointFromUserClick(Geometry::Spline3D& spline, const glm::vec2& cursorPosition, const View::CameraFreefly &camera);
        void pick(const glm::vec2& cursorPosition, const View::CameraFreefly &camera, bool isClicked);
        float& yPlaneIntersection();
        void setState(const SplineState& state);
        SplineState state() const;

        Geometry::Spline3D& positions();
        Geometry::Spline3D& targets();
        Geometry::Spline1D& velocities();
    };

}