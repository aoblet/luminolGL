//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_TRANSFORMATION_H
#define LUMINOLGL_TRANSFORMATION_H

#include <glm/glm.hpp>

namespace Geometry
{
    struct Transformation {

        glm::vec3 position; /* position (or translation) */

        glm::vec4 rotation; /** x, y, z, angle in degrees */

        glm::vec3 scale;    /* scale applied for each coordinate */


        Transformation(const glm::vec3& position = glm::vec3(0,0,0), const glm::vec4& rotation = glm::vec4(0,0,0,0), const glm::vec3& scale = glm::vec3(1,1,1));
        Transformation(const glm::vec4& rotation);
        Transformation(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot);

        glm::mat4 getTRSMatrix() const; /* Returns the transformationMatrix of this Transform object.
                                         * Operations are made in the following order :
                                         * ResultMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix
                                         */

        glm::mat4 getTMatrix() const; /* Returns the translationMatrix of this Transform object.
                                       * Operations are made in the following order :
                                       * ResultMatrix = TranslationMatrix
                                       */

        glm::mat4 getRMatrix() const; /* Returns the rotationMatrix of this Transform object.
                                       * Operations are made in the following order :
                                       * ResultMatrix = RotationMatrix
                                       */

        glm::mat4 getSMatrix() const; /* Returns the scaleMatrix of this Transform object.
                                       * Operations are made in the following order :
                                       * ResultMatrix = ScaleMatrix
                                       */
    };
}




#endif //LUMINOLGL_TRANSFORMATION_H
