//
// Created by mehdi on 18/02/16.
//

#ifndef LUMINOLGL_SCENE_H
#define LUMINOLGL_SCENE_H

#include <vector>
#include <string>
#include <glm/detail/type_vec.hpp>

#include "graphics/MeshInstance.h"

namespace Graphics
{
    class Scene {
    private:
        std::map<std::string, MeshInstance*> _meshInstances;
        std::vector<glm::vec3> _visiblePositions;
        std::vector<glm::vec4> _visibleRotations;
        std::string _currentInstance;
    public:
        void addMeshInstance(MeshInstance *instance, const std::string& name);

        void setCurrentInstance(const std::string& name);

        MeshInstance* getInstance();

        const std::vector<glm::vec3>& computeVisiblePositions(const glm::mat4 & VP);
        const std::vector<glm::vec4>& computeVisibleRotations(const glm::mat4 & VP);

        const std::vector<glm::vec3>& getVisiblePositions();
        const std::vector<glm::vec4>& getVisibleRotations();

        int getInstanceNumber();
    };
}




#endif //LUMINOLGL_SCENE_H
