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
    class Spline3D{
    private:
        std::vector<glm::vec3> _points;

        int findIndex(float t);

        float findInterval(float t);

        int at(int index);

    public:
        void add(const glm::vec3 &point);

        void remove(int index);

        glm::vec3 operator[](int index) const;

        glm::vec3& operator[](int index);

        size_t size();

        void clear();

        glm::vec3 linearInterpolation(float t);

        glm::vec3 cubicInterpolation(float t);

        void load(const std::string & filePath);
        void save(const std::string & filePath) const;
    };
}

#endif //LUMINOLGL_SPLINE3D_H
