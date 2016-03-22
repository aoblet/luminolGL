#include "geometry/ScalarInterpolation.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Geometry{
    constexpr float scalarLinearInterpolation(float y1, float y2, float t){
        return(y1*(1-t) + y2*t);
    }

    float scalarCosineInterpolation(float y1, float y2, float t){
        float t2 = (1-glm::cos(t*glm::pi<float>()))/2.f;
        return scalarLinearInterpolation(y1, y2, t2);
    }

    float scalarCubicInterpolation(float y0,float y1, float y2,float y3, float t){
        float a0,a1,a2,a3,t2;

        t2 = t*t;
        a0 = y3 - y2 - y0 + y1;
        a1 = y0 - y1 - a0;
        a2 = y2 - y0;
        a3 = y1;

        return(a0*t*t2+a1*t2+a2*t+a3);
    }
}