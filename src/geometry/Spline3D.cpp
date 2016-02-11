//
// Created by mehdi on 10/02/16.
//

#include "geometry/Spline3D.h"
#include "geometry/Interpolation.h"

namespace Geometry
{
    int Spline3D::findIndex(float t){
        return int( t * ( points.size() - 1 ) );
    }

    float Spline3D::findInterval(float t){
        return glm::mod(t * ( points.size() - 1 ), 1.f);
    }

    int Spline3D::at(int index){
        return glm::clamp(index, 0, int(points.size() - 1));
    }

    void Spline3D::add(const glm::vec3 &point){
        points.push_back(point);
    }

    void Spline3D::remove(int index){
        points.erase(points.begin() + index);
    }

    glm::vec3 Spline3D::operator[](int index) const {
        return points.at(index);
    }

    glm::vec3& Spline3D::operator[](int index) {
        return points.at(index);
    }

    size_t Spline3D::size(){
        return points.size();
    }

    void Spline3D::clear(){
        points.clear();
    }

    glm::vec3 Spline3D::linearInterpolation(float t){
        int index = findIndex(t);
        return Geometry::linearInterpolation(points[at(index)], points[at(index+1)], findInterval(t));
    }

    glm::vec3 Spline3D::cubicInterpolation(float t){
        int index = findIndex(t);
        return Geometry::cubicInterpolation(points[at(index-1)], points[at(index)], points[at(index+1)], points[at(index+2)], findInterval(t));
    }
}

