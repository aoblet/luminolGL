#pragma once

namespace Geometry{
    constexpr float scalarLinearInterpolation(float y1, float y2, float t);
    float scalarCosineInterpolation(float y1, float y2, float t);
    float scalarCubicInterpolation(float y0,float y1, float y2,float y3, float t);
}
