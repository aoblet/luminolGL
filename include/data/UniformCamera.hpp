#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Data{
    struct UniformCamera{
        glm::vec3 _pos;
        int _padding;
        glm::mat4 _screenToWorld;
        glm::mat4 _viewToWorld;

        UniformCamera(){}
        UniformCamera(glm::vec3 pos, glm::mat4 screenToWorld, glm::mat4 viewToWorld):
                _pos(pos), _screenToWorld(screenToWorld), _viewToWorld(viewToWorld){}
    };
}
