//
// Created by mehdi on 16/02/16.
//

#ifndef LUMINOLGL_BOUNDINGBOX_H
#define LUMINOLGL_BOUNDINGBOX_H

#include <vector>
#include <glm/detail/type_vec.hpp>

#include "graphics/VertexDescriptor.h"
#include "view/CameraFreefly.hpp"

namespace Geometry
{
    class BoundingBox {
    private:
        std::vector<glm::vec3> _points;
    public:
        bool isVisible(const View::CameraFreefly& camera) const;
        void compute(const std::vector<Graphics::VertexDescriptor>& vertices);
        const std::vector<glm::vec3>& getVector() const;
    };
}




#endif //LUMINOLGL_BOUNDINGBOX_H
