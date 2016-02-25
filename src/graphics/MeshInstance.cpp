//
// Created by mehdi on 18/02/16.
//

#include "graphics/MeshInstance.h"
#include "utils/utils.h"

namespace Graphics
{

    MeshInstance::MeshInstance(Mesh *referenceMesh) : _referenceMesh(referenceMesh) { }

    void MeshInstance::addInstance(const Geometry::Transformation &trans) {
        _transformations.push_back(trans);
    }

    void MeshInstance::addInstance(const std::vector<Geometry::Transformation> &transformations) {
        _transformations.insert(_transformations.begin(), transformations.begin(), transformations.end());
    }

    void MeshInstance::addInstance(const std::vector<glm::vec3> &positions, const std::vector<glm::vec4> &rotations) {
        if(positions.size() != rotations.size())
            throw std::runtime_error("MeshInstance::addInstance : Trying to load position and rotation vector with different size");

        for(size_t i = 0; i < positions.size(); ++i)
            _transformations.push_back(Geometry::Transformation(positions[i], rotations[i]));
    }

    void MeshInstance::addInstance(const std::vector<glm::vec3> &positions) {
        for(auto& position : positions)
            _transformations.push_back(Geometry::Transformation(position));
    }

    void MeshInstance::addInstance(const glm::vec3 &position, const glm::vec4 &rotation) {
        addInstance(Geometry::Transformation(position, rotation));
    }

    void MeshInstance::addInstance(float xpos, float ypos, float zpos, float angle, float xrot, float yrot, float zrot) {
        addInstance(Geometry::Transformation(xpos, ypos, zpos, angle, xrot, yrot, zrot));
    }


    const glm::vec3 &MeshInstance::getPosition(int index) {
        return _transformations.at(index).position;
    }

    const glm::vec4 &MeshInstance::getRotation(int index) {
        return _transformations.at(index).rotation;
    }

    int MeshInstance::getInstanceNumber() {
        return _transformations.size();
    }

    const Geometry::Transformation &MeshInstance::getTransformation(int index) const {
        return _transformations.at(index);
    }

    Geometry::BoundingBox MeshInstance::getBoundingBox(int index) const {
        return _transformations.at(index).getTransformationMatrix() * _referenceMesh->getBoundingBox();
    }
}