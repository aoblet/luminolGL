#pragma once
#include <GL/glew.h>
#include <GL/gl.h>

namespace Graphics{

    /**
     * Uniform buffer object wrapper.
     * As we are limited in number of binding point on GPU, one UBO could be re-used for other purposes with other objects.
     * That's why this class is data uncorrelated.
     */
    class UBO{
        GLuint _bufferId;
        GLuint _bindingPointIndex;
        int _sizeBuffer;
        void reserveBuffer(int size);
    public:
        UBO(GLuint bindingPointIndex, int sizeBuffer=0);
        ~UBO();

        void updateBuffer(const GLvoid* data, int sizeofObject);
        GLuint bindingPoint() const;
    };
}

