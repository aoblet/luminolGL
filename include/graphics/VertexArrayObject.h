//
// Created by mehdi on 16/02/16.
//

#ifndef LUMINOLGL_VERTEXARRAYOBJECT_H
#define LUMINOLGL_VERTEXARRAYOBJECT_H


#include <GL/glew.h>
#include <vector>

#include "graphics/VertexBufferObject.h"

namespace Graphics
{
    class VertexArrayObject {
    private:
        GLuint _glId;
        std::vector<VertexBufferObject*> _vbos;
        bool _isInGPU;

    public:
        VertexArrayObject(bool initGL = true);
        VertexArrayObject(VertexArrayObject&& other);
        VertexArrayObject(const VertexArrayObject& other);
        ~VertexArrayObject();

        void addVBO(VertexBufferObject* vbo);
        void initGL();
        void init();
        void bind();
        GLuint glId();
        const std::vector<VertexBufferObject*>& vbos() const;
        void clearVBOs();

        static void unbindAll();
    };
}




#endif //LUMINOLGL_VERTEXARRAYOBJECT_H
