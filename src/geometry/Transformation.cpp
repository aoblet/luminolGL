//
// Created by mehdi on 18/02/16.
//
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include "geometry/Transformation.h"
#include "utils/utils.h"

#include <iostream>

Geometry::Transformation::Transformation(const glm::vec3 &pos, const glm::vec4 &rot, const glm::vec3& scal)
        : position(pos),
          rotation(rot),
          scale(scal)

{ }

Geometry::Transformation::Transformation(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot)
        : Transformation(glm::vec3(xpos,ypos,zpos), glm::vec4(xrot, yrot, zrot, angle))
{ }

Geometry::Transformation::Transformation(const glm::vec4 &rot)
        : Transformation(glm::vec3(0,0,0), rot)
{ }


glm::mat4 Geometry::Transformation::getTRSMatrix() const {
    glm::mat4 rx = glm::rotate(rotation.x, glm::vec3(1, 0, 0));
    glm::mat4 ry = glm::rotate(rotation.y, glm::vec3(0, 1, 0));
    glm::mat4 rz = glm::rotate(rotation.z, glm::vec3(0, 0, 1));
    glm::mat4 s = glm::scale(scale);
    glm::mat4 t = glm::translate(position);
    return t*rz*ry*rx*s;
}


glm::mat4 Geometry::Transformation::getTMatrix() const {
    return glm::translate(position);
}

glm::mat4 Geometry::Transformation::getRMatrix() const {
    glm::mat4 rx = glm::rotate(rotation.x, glm::vec3(1, 0, 0));
    glm::mat4 ry = glm::rotate(rotation.y, glm::vec3(0, 1, 0));
    glm::mat4 rz = glm::rotate(rotation.z, glm::vec3(0, 0, 1));

    return rz*ry*rx;
}

glm::mat4 Geometry::Transformation::getSMatrix() const {
    return glm::scale(scale);
}
