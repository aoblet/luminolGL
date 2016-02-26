//
// Created by mehdi on 14/02/16.
//

#ifndef LUMINOLGL_VERTEXDESCRIPTOR_H
#define LUMINOLGL_VERTEXDESCRIPTOR_H

#include <glm/glm.hpp>

namespace Graphics
{
    /** Describe a 3d vertex :
     * Position, Normal, & Texture Coordinates
     */
    struct VertexDescriptor {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;

        VertexDescriptor(){}

        VertexDescriptor(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv) :
                position(pos),
                normal(norm),
                texcoord(uv)
        {}

        VertexDescriptor(float pos_x, float pos_y, float pos_z,
                         float norm_x, float norm_y, float norm_z,
                         float uv_x, float uv_y)
        {
            position = glm::vec3(pos_x, pos_y, pos_z);
            normal = glm::vec3(norm_x, norm_y, norm_z);
            texcoord = glm::vec2(uv_x, uv_y);
        }

    };
}




#endif //LUMINOLGL_VERTEXDESCRIPTOR_H
