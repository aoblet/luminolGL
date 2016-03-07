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
        ELEMENT_ARRAY_BUFFER,
        INSTANCE_BUFFER,                /** to store instance position or rotation or whatever you want */
        INSTANCE_TRANSFORMATION_BUFFER  /** to store transformations matrix */
    };

    class VertexBufferObject {
    private:
        GLuint _glId;
        DataType _dataType;
        GLenum _target;
        GLuint _attribArray;
        bool _isInGPU;
        void initVboVertexDescriptor();
        void initVboVec3();
        void initVboVec2();
        void initVboFloat();
        void initVboInt();
        void initVboInstanceVec3();
        void initVboInstanceMat4();
    public:
        VertexBufferObject(DataType dataType, GLuint attribArray = 0, bool initGL = true);
        VertexBufferObject(VertexBufferObject&& other);
        VertexBufferObject(const VertexBufferObject& other);
        ~VertexBufferObject();

        GLuint glId();

        /** Should not be used directly but inside VAO.init() */
        void init();
        void initGL();
        void bind();
        void updateData(const std::vector<VertexDescriptor>& data);
        void updateData(const std::vector<glm::vec3>& data);
        void updateData(const std::vector<glm::vec2>& data);
        void updateData(const std::vector<float>& data);
        void updateData(const std::vector<int>& data);
        void updateData(const std::vector<glm::mat4>& data);
        void setAttribArray(GLuint value);
        static void unbindAll();
    };
}



#endif //LUMINOLGL_VERTEXBUFFEROBJECT_H
