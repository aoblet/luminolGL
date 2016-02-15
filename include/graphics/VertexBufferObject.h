//
// Created by mehdi on 14/02/16.
//

#ifndef LUMINOLGL_VERTEXBUFFEROBJECT_H
#define LUMINOLGL_VERTEXBUFFEROBJECT_H

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>
#include "graphics/VertexDescriptor.h"

namespace Graphics
{
    enum DataType{
        VERTEX_DESCRIPTOR,
        VEC3,
        VEC2,
        FLOAT,
        INT,
        ELEMENT_ARRAY_BUFFER
    };

    class VertexBufferObject {
    private:
        GLuint _glId;
        DataType _dataType;
        GLenum _target;
        GLuint _attribArray;
        void initVboVertexDescriptor();
        void initVboVec3();
        void initVboVec2();
        void initVboFloat();
        void initVboInt();
        void bind();
    public:
        VertexBufferObject(DataType dataType, GLuint attribArray = 0);
        GLuint glId();
        void init();
        void updateData(const std::vector<VertexDescriptor> data);
        void updateData(const std::vector<glm::vec3> data);
        void updateData(const std::vector<glm::vec2> data);
        void updateData(const std::vector<float> data);
        void updateData(const std::vector<int> data);
        void setAttribArray(GLuint value);
        static void unbindAll();
    };
}



#endif //LUMINOLGL_VERTEXBUFFEROBJECT_H
