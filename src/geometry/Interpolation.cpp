//
// Created by mehdi on 10/02/16.
//

#include "geometry/Interpolation.h"

namespace Geometry
{
    float linearInterpolation(float y1, float y2, float t){
        return(y1*(1-t)+y2*t);
    }

    glm::vec3 linearInterpolation(glm::vec3 v1, glm::vec3 v2, float t){
        return glm::vec3(linearInterpolation(v1.x, v2.x, t),
                         linearInterpolation(v1.y, v2.y, t),
                         linearInterpolation(v1.z, v2.z, t));
    }

    float cosineInterpolation(float y1, float y2, float t){
        float t2;

        t2 = (1-glm::cos(t*M_PI))/2.;
        return(y1*(1-t2)+y2*t2);
    }

    float cubicInterpolation(float y0,float y1, float y2,float y3, float t){
        float a0,a1,a2,a3,t2;

        t2 = t*t;
        a0 = y3 - y2 - y0 + y1;
        a1 = y0 - y1 - a0;
        a2 = y2 - y0;
        a3 = y1;

        return(a0*t*t2+a1*t2+a2*t+a3);
    }

    glm::vec3 cubicInterpolation(glm::vec3 v0,glm::vec3 v1, glm::vec3 v2,glm::vec3 v3, float t){
        return glm::vec3(cubicInterpolation(v0.x, v1.x, v2.x, v3.x, t),
                         cubicInterpolation(v0.y, v1.y, v2.y, v3.y, t),
                         cubicInterpolation(v0.z, v1.z, v2.z, v3.z, t));
    }
}
