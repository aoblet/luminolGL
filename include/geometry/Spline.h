//
// Created by mehdi on 10/02/16.
//

#ifndef LUMINOLGL_SPLINE3D_H
#define LUMINOLGL_SPLINE3D_H

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Geometry
{
    /**
     * Templated Spline class.
     * @template T must be of type glm::vecN or tvec1 (1d): needed by interpolation module.
     */
    template<class T> class Spline {
    private:
        std::vector<T> _points;
        int findIndex(float t) const;
        float findInterval(float t) const;
        int at(int index) const;

    public:
        void add(const T& points);
        void remove(int index);
        T operator[](int index) const;
        T& operator[](int index);
        size_t size() const;
        void clear();
        T linearInterpolation(float t) const;
        T cubicInterpolation(float t) const;
        void erase(typename std::vector<T>::iterator it);
        typename std::vector<T>::iterator begin();

        void load(const std::string & filePath);
        void save(const std::string & filePath) const;
    };



    using Spline1D = Spline<glm::tvec1<float, glm::precision::highp>>;
    using Spline2D = Spline<glm::vec2>;
    using Spline3D = Spline<glm::vec3>;
}

#include "Spline.tpp"

#endif //LUMINOLGL_SPLINE3D_H
