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
    glm::mat4 r = glm::rotate(rotation.w, glm::vec3(rotation.x, rotation.y, rotation.z));
    glm::mat4 s = glm::scale(scale);
    glm::mat4 t = glm::translate(position);
    return t*r*s;
}


glm::mat4 Geometry::Transformation::getTMatrix() const {
    return glm::translate(position);
}

glm::mat4 Geometry::Transformation::getRMatrix() const {
    return glm::rotate(rotation.w, glm::vec3(rotation.x, rotation.y, rotation.z));
}

glm::mat4 Geometry::Transformation::getSMatrix() const {
    return glm::scale(scale);
}
