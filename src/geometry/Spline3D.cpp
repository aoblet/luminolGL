//
// Created by mehdi on 10/02/16.
//

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "geometry/Spline3D.h"
#include "geometry/Interpolation.h"

namespace Geometry
{
    int Spline3D::findIndex(float t){
        return int( t * ( _points.size() - 1 ) );
    }

    float Spline3D::findInterval(float t){
        return glm::mod(t * ( _points.size() - 1 ), 1.f);
    }

    int Spline3D::at(int index){
        return glm::clamp(index, 0, int(_points.size() - 1));
    }

    void Spline3D::add(const glm::vec3 &point){
        _points.push_back(point);
    }

    void Spline3D::remove(int index){
        _points.erase(_points.begin() + index);
    }

    glm::vec3 Spline3D::operator[](int index) const {
        return _points.at(index);
    }

    glm::vec3& Spline3D::operator[](int index) {
        return _points.at(index);
    }

    size_t Spline3D::size(){
        return _points.size();
    }

    void Spline3D::clear(){
        _points.clear();
    }

    glm::vec3 Spline3D::linearInterpolation(float t){
        int index = findIndex(t);
        return Geometry::linearInterpolation(_points[at(index)], _points[at(index+1)], findInterval(t));
    }

    glm::vec3 Spline3D::cubicInterpolation(float t){
        int index = findIndex(t);
        return Geometry::cubicInterpolation(_points[at(index-1)], _points[at(index)], _points[at(index+1)], _points[at(index+2)], findInterval(t));
    }

    void Spline3D::load(const std::string & filePath){
        std::string line;
        std::ifstream file (filePath);
        std::string::size_type sz;

        _points.clear();

        if (!file.is_open())
            throw std::runtime_error("Unable to load spline from \"" + filePath + "\"");

        while ( getline (file,line) )
        {
            float x = std::stof (line,&sz);
            float y = std::stof (line.substr(sz), &sz);
            float z = std::stof (line.substr(sz));

            _points.push_back(glm::vec3(x,y,z));
        }
    }

    void Spline3D::save(const std::string & filePath) const{
        std::ofstream file (filePath);

        if (!file.is_open())
           throw std::runtime_error("Unable to save spline at \"" + filePath + "\"");


        for(auto point : _points){
            file << point.x << " ";
            file << point.y << " ";
            file << point.z << std::endl;
        }
    }
}

