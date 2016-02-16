#pragma once

#include "view/CameraFreefly.hpp"
#include "geometry/Spline3D.h"
#include "ui/UserInput.hpp"

namespace View{
    class CameraController{
        CameraFreefly& _camera;
        UI::UserInput& _userInput;

        Geometry::Spline3D _cameraPositions;
        Geometry::Spline3D _cameraSpeeds;
        Geometry::Spline3D _cameraViewTargets;
        bool _isSpectatorMode;
        float _velocitySplines;

        void updateFromSplines(float time);
        void updateFromInput(const UI::UserInput& userInput);

    public:
        CameraController(CameraFreefly& camera, UI::UserInput& userInput, float velocitySplines=1, bool isSpectator=false);

        void setCamera(CameraFreefly& c);
        void setUserInput(UI::UserInput& u);
        void setVelocitySplines(float velocity);
        void update(float time);
        void setSpectator(bool isSpectator);

        float velocitySplines() const;
        float& velocitySplines();

        bool isSpectator() const;
        CameraFreefly& camera() const;
        UI::UserInput& userInput() const;

        Geometry::Spline3D& positions();
        Geometry::Spline3D& speeds();
        Geometry::Spline3D& viewTargets();
    };
}