//
// Created by mehdi on 10/02/16.
//

#ifndef LUMINOLGL_INTERPOLATION_H
#define LUMINOLGL_INTERPOLATION_H

#include <glm/glm.hpp>

namespace Geometry
{
    float linearInterpolation(float y1, float y2, float t);

    glm::vec3 linearInterpolation(glm::vec3 v1, glm::vec3 v2, float t);

    float cosineInterpolation(float y1, float y2, float t);

    float cubicInterpolation(float y0,float y1, float y2,float y3, float t);

    glm::vec3 cubicInterpolation(glm::vec3 v0,glm::vec3 v1, glm::vec3 v2,glm::vec3 v3, float t);
}



#endif //LUMINOLGL_INTERPOLATION_H
