//
// Created by mehdi on 16/02/16.
//

#include <stdexcept>
#include "geometry/BoundingBox.h"

namespace Geometry
{
    bool BoundingBox::isVisible(const View::CameraFreefly &camera) const {
        //TODO:Check if bounding box is visible using MVP Matrix
        return true;
    }


    void BoundingBox::compute(const std::vector<Graphics::VertexDescriptor> &vertices) {

        float xmin, ymin, zmin;
        float xmax, ymax, zmax;

        xmin = xmax = vertices[0].position.x;
        ymin = ymax = vertices[0].position.y;
        zmin = zmax = vertices[0].position.z;

        for(auto& vertice : vertices){
            if (vertice.position.x < xmin) xmin = vertice.position.x;
            if (vertice.position.x > xmax) xmax = vertice.position.x;
            if (vertice.position.y < ymin) ymin = vertice.position.y;
            if (vertice.position.y > ymax) ymax = vertice.position.y;
            if (vertice.position.z < zmin) zmin = vertice.position.z;
            if (vertice.position.z > zmax) zmax = vertice.position.z;
        }

        _points = {
              glm::vec3(xmin,ymin,zmin),
              glm::vec3(xmin,ymin,zmax),
              glm::vec3(xmax,ymin,zmax),
              glm::vec3(xmax,ymin,zmin),
              glm::vec3(xmin,ymax,zmin),
              glm::vec3(xmin,ymax,zmax),
              glm::vec3(xmax,ymax,zmax),
              glm::vec3(xmax,ymax,zmin)
        };

    }

    const std::vector<glm::vec3> &BoundingBox::getVector() const{
        return _points;
    }
}