#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Spline.h"
#include "geometry/Interpolation.h"
#include <glog/logging.h>

namespace Geometry
{
    template<class T>
    int Spline<T>::findIndex(float t) const{
        return int( t * ( _points.size() - 1 ) );
    }

    template<class T>
    float Spline<T>::findInterval(float t) const{
        return glm::mod(t * ( _points.size() - 1 ), 1.f);
    }

    template<class T>
    int Spline<T>::at(int index) const{
        return glm::clamp(index, 0, int(_points.size() - 1));
    }

    template<class T>
    void Spline<T>::add(const T& point){
        _points.push_back(point);
    }

    template<class T>
    void Spline<T>::remove(int index){
        _points.erase(_points.begin() + index);
    }

    template<class T>
    T Spline<T>::operator[](int index) const {
        return _points.at(index);
    }

    template<class T>
    T& Spline<T>::operator[](int index) {
        return _points.at(index);
    }

    template<class T>
    size_t Spline<T>::size() const{
        return _points.size();
    }

    template<class T>
    void Spline<T>::clear(){
        _points.clear();
    }

    template<class T>
    T Spline<T>::linearInterpolation(float t) const{
        int index = findIndex(t);
        return Geometry::linearInterpolation<T>(_points[at(index)], _points[at(index+1)], findInterval(t));
    }

    template<class T>
    T Spline<T>::cubicInterpolation(float t) const{
        int index = findIndex(t);
        return Geometry::cubicInterpolation<T>(_points[at(index-1)], _points[at(index)], _points[at(index+1)], _points[at(index+2)], findInterval(t));
    }

    template<class T>
    void Spline<T>::load(const std::string & filePath){
        std::ifstream file (filePath);

        if (!file.is_open())
            throw std::runtime_error("Unable to load spline from \"" + filePath + "\"");

        std::string line;
        std::string::size_type subStr, tmpOffset;

        _points.clear();

        int vecLength = T().length();

        while (std::getline(file, line)){
            subStr = tmpOffset =  0;
            T tmp;

            for(int i = 0; i<vecLength; ++i){
                tmp[i] = std::stof(line.substr(subStr), &tmpOffset);
                subStr += tmpOffset;
            }
            _points.push_back(std::move(tmp));
        }
    }

    template<class T>
    void Spline<T>::save(const std::string & filePath) const{
        std::ofstream file (filePath);

        if (!file.is_open())
            throw std::runtime_error("Unable to save spline at '" + filePath + "'");

        if(_points.empty())
            return;

        int vecLength = _points[0].length();
        for(auto point : _points){
            for(int i = 0; i<vecLength-1; ++i){
                file << point[i] << " ";
            }
            file << point[vecLength-1] << std::endl;
        }
    }
}

