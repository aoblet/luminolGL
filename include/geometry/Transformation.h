//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_TRANSFORMATION_H
#define LUMINOLGL_TRANSFORMATION_H

#include <glm/glm.hpp>

namespace Geometry
{
    struct Transformation {
        glm::vec3 position;
        glm::vec4 rotation; /** x, y, z, angle in degrees */
        glm::vec3 scale;

        Transformation(const glm::vec3& position = glm::vec3(0,0,0), const glm::vec4& rotation = glm::vec4(1,1,1,0), const glm::vec3& scale = glm::vec3(1,1,1));
        Transformation(const glm::vec4& rotation);
        Transformation(float xpos, float ypos, float zpos);
        Transformation(float angle, float xrot, float yrot, float zrot);
        Transformation(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot);
        Transformation(float angle, const glm::vec3& orientation);

        glm::mat4 getTransformationMatrix() const;
    };
}




#endif //LUMINOLGL_TRANSFORMATION_H
