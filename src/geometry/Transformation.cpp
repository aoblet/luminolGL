//
// Created by mehdi on 18/02/16.
//
#include <glm/gtx/transform.hpp>
#include "geometry/Transformation.h"
#include "utils/utils.h"

Geometry::Transformation::Transformation(const glm::vec3 &pos, const glm::vec4 &rot)
        : position(pos),
          rotation(rot)
{ }

Geometry::Transformation::Transformation(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot)
        : Transformation(glm::vec3(xpos,ypos,zpos), glm::vec4(xrot, yrot, zrot, angle))
{ }

Geometry::Transformation::Transformation(const glm::vec4 &rot)
        : Transformation(glm::vec3(0,0,0), rot)
{ }

Geometry::Transformation::Transformation(float xpos, float ypos, float zpos)
        : Transformation(glm::vec3(xpos,ypos,zpos))
{ }

Geometry::Transformation::Transformation(float angle, float xrot, float yrot, float zrot)
        : Transformation(glm::vec4(xrot,yrot,zrot,angle))
{ }

Geometry::Transformation::Transformation(float angle, const glm::vec3 &orientation)
        : Transformation(glm::vec4(orientation,angle))
{ }


glm::mat4 Geometry::Transformation::getTransformationMatrix() const {
    glm::mat4 rot = glm::rotate(rotation.w, glm::vec3(rotation.x, rotation.y, rotation.z));
    glm::mat4 trans = glm::translate(position);
    return trans;
}
