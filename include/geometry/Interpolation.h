//
// Created by mehdi on 10/02/16.
//

#pragma once

#include <glm/glm.hpp>
#include <typeinfo>
#include "geometry/ScalarInterpolation.hpp"

namespace Geometry
{
    // For 1d use glm::tvec1

    template<class glmVector>
    glmVector linearInterpolation(const glmVector & v1, const glmVector & v2, float t){
        // assert(typeid(glmVector) == typeid(glm::vec2) || typeid(glmVector) == typeid(glm::vec3) || typeid(glmVector) == typeid(glm::vec4));
        glmVector res;
        for(int i = 0; i < v1.length(); ++i)
            res[i] = scalarLinearInterpolation(v1[i], v2[i], t);
        return res;
    }


    template<class glmVector>
    glmVector cubicInterpolation(const glmVector & v0, const glmVector & v1, const glmVector & v2, const glmVector & v3, float t){
        // assert(typeid(glmVector) == typeid(glm::vec2) || typeid(glmVector) == typeid(glm::vec3) || typeid(glmVector) == typeid(glm::vec4));
        glmVector res;
        for(int i = 0; i < v1.length(); ++i)
            res[i] = scalarCubicInterpolation(v0[i], v1[i], v2[i], v3[i], t);
        return res;
    }
}
