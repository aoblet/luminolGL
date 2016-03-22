#pragma once

#include "view/CameraFreefly.hpp"
#include "geometry/Spline.h"
#include "gui/UserInput.hpp"

namespace View{
    class CameraController{
        CameraFreefly& _camera;
        GUI::UserInput& _userInput;

        Geometry::Spline<glm::vec3> _cameraPositions;
        Geometry::Spline<glm::tvec1<float>> _cameraSpeeds;
        Geometry::Spline<glm::vec3> _cameraViewTargets;
        bool _isSpectatorMode;
        float _velocitySplines;

        void updateFromSplines(float time);
        void updateFromInput(const GUI::UserInput& userInput);

    public:
        CameraController(CameraFreefly& camera, GUI::UserInput& userInput, float velocitySplines=1, bool isSpectator=true);

        void setCamera(CameraFreefly& c);
        void setUserInput(GUI::UserInput& u);
        void setVelocitySplines(float velocity);
        void update(float time);
        void setSpectator(bool isSpectator);

        float velocitySplines() const;
        float& velocitySplines();

        bool isSpectator() const;
        CameraFreefly& camera() const;
        GUI::UserInput& userInput() const;

        Geometry::Spline<glm::vec3> & positions();
        Geometry::Spline<glm::tvec1<float>> & speeds();
        Geometry::Spline<glm::vec3> & viewTargets();
    };
}