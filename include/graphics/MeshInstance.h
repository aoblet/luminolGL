//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_MESHINSTANCE_H
#define LUMINOLGL_MESHINSTANCE_H

#include "graphics/Mesh.h"
#include "geometry/Transformation.h"

namespace Graphics
{
    class MeshInstance {
    private:
        Mesh* _referenceMesh;
        std::vector<Geometry::Transformation> _transformations;
    public:
        MeshInstance(Mesh* referenceMesh);
        void addInstance(const Geometry::Transformation &trans);
        void addInstance(const std::vector<Geometry::Transformation>& transformations);
        void addInstance(const std::vector<glm::vec3> &positions, const std::vector<glm::vec4> &rotations);
        void addInstance(const std::vector<glm::vec3> &positions);
        void addInstance(const glm::vec3& position, const glm::vec4& rotation = glm::vec4(0,0,0,0));
        void addInstance(float xpos, float ypos, float zpos, float angle = 0, float xrot = 0, float yrot = 0, float zrot = 0);
        const glm::vec3& getPosition(int index);
        const glm::vec4& getRotation(int index);
        const Geometry::Transformation& getTransformation(int index) const;
        Geometry::BoundingBox getBoundingBox(int index) const;
        int getInstanceNumber();
    };
}



#endif //LUMINOLGL_MESHINSTANCE_H
