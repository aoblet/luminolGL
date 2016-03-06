//
// Created by mehdi on 05/03/16.
//

#include <glog/logging.h>
#include <glm/gtx/string_cast.hpp>
#include "gui/ObjectPicker.h"
#include "graphics/DebugDrawer.h"

namespace Gui
{

    ObjectPicker::ObjectPicker() : _picked(false), _markerScale(5) { }

    void ObjectPicker::pickObject(const glm::vec2 &cursorPosition, Graphics::Scene &scene, const View::CameraFreefly &camera) {
        glm::vec3 front = camera.getFocal() * camera.getFront();
        float mouseNormX = ((cursorPosition.x / camera.getViewPort().x) * 2 * camera.getNormalizedViewPort().x - 1 * camera.getNormalizedViewPort().x);
        float mouseNormY = ((cursorPosition.y / camera.getViewPort().y) * 2 * camera.getNormalizedViewPort().y - 1 * camera.getNormalizedViewPort().y);

        float scale = camera.getFocal();

        glm::vec3 left = - camera.getLeft() * scale * mouseNormX;
        glm::vec3 up = camera.getUp() * scale * mouseNormY;
        glm::vec3 rayDir = glm::normalize(front + left + up);
        glm::vec3 rayOrigin = camera.getEye();

        float minDist = 100000;

        _picked = false;

        for(auto& meshInstance : scene.meshInstances()){
            for(auto& trans : meshInstance.getTransformations()){
                Geometry::BoundingBox box = meshInstance.modelMeshGroup().getBoundingBox();
                float dist = 0;
                if(box.intersect(rayOrigin, rayDir, trans.getTRSMatrix(), &dist)){
                    if(dist < minDist){
                        minDist = dist;
                        _targetTransformation = &trans;
                        _picked = true;
                        DLOG(INFO) << "object picked !";
                    }
                }
            }
        }
    }

    void ObjectPicker::drawPickedObject(Graphics::ShaderProgram& program) {
        if(!_picked) return;
        Graphics::DebugDrawer::drawAxis(_targetTransformation->getTMatrix(), program, _markerScale, 50);
    }

    void ObjectPicker::setMarkerScale(float scale) {
        _markerScale = scale;
    }
}