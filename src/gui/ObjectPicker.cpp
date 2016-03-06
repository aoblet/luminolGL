//
// Created by mehdi on 05/03/16.
//

#include <glog/logging.h>
#include <glm/gtx/string_cast.hpp>
#include "gui/ObjectPicker.h"
#include "graphics/DebugDrawer.h"

namespace Gui
{

    ObjectPicker::ObjectPicker(float markerScale) : _picked(false), _pickedAnchorAxis(-1), _markerScale(markerScale), _longClick(false), _mode(TRANSLATION){
        float s = 0.1f * _markerScale; // axis anchor scale

        std::vector<Geometry::BoundingBox> trans;
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(markerScale,s,s)));
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(s,markerScale,s)));
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(s,s,markerScale)));

        _axisAnchors = trans;

        _pickerAnchors.push_back(trans);
        _pickerAnchors.push_back(trans);
        _pickerAnchors.push_back(trans);
    }

    void ObjectPicker::pickObject(const glm::vec2 &cursorPosition, const glm::vec2 &cursorSpeed, Graphics::Scene &scene, const View::CameraFreefly &camera, bool click) {

        if(!click && _longClick) _longClick = false;
        if(click && _picked) _longClick = true;

        glm::vec3 front = camera.getFocal() * camera.getFront();
        float mouseNormX = ((cursorPosition.x / camera.getViewPort().x) * 2 * camera.getNormalizedViewPort().x - 1 * camera.getNormalizedViewPort().x);
        float mouseNormY = ((cursorPosition.y / camera.getViewPort().y) * 2 * camera.getNormalizedViewPort().y - 1 * camera.getNormalizedViewPort().y);

        glm::vec3 left = - camera.getLeft() * camera.getFocal() * mouseNormX;
        glm::vec3 up = camera.getUp() * camera.getFocal() * mouseNormY;
        glm::vec3 rayDir = glm::normalize(front + left + up);
        glm::vec3 rayOrigin = camera.getEye();

        float minDist = 100000;
        float dist = 0;

        if(!_longClick) _pickedAnchorAxis = -1;

        if(_picked && !_longClick){
            for(size_t i = 0; i < _pickerAnchors[int(_mode)].size(); ++i){
                if(_pickerAnchors[int(_mode)][i].intersect(rayOrigin, rayDir, _targetTransformation->getTMatrix(), &dist)){
                    _pickedAnchorAxis = i;
                }
            }
        }

        if(_pickedAnchorAxis >= 0 && _longClick){
            transformPickedObject(cursorSpeed, _pickedAnchorAxis, camera.getProjectionMatrix() * camera.getViewMatrix());
            return;
        }

        if(!click) return;

        _picked = false;

        for(auto& meshInstance : scene.meshInstances()){
            for(auto& trans : meshInstance.getTransformations()){
                auto box = meshInstance.modelMeshGroup().getBoundingBoxPtr();
                if(box->intersect(rayOrigin, rayDir, trans.getTRSMatrix(), &dist)){
                    if(dist < minDist){
                        minDist = dist;
                        _targetTransformation = &trans;
                        _targetBoundingBox = box;
                        _picked = true;
                    }
                }
            }
        }
    }

    void ObjectPicker::drawPickedObject(Graphics::ShaderProgram& program) {
        if(!_picked) return;

        if(_mode == TRANSLATION)
        {
            Graphics::DebugDrawer::drawTranslateAxis(_targetTransformation->getTMatrix(), program, _markerScale, 5);
        }

        if(_mode == ROTATION)
        {
            Graphics::DebugDrawer::drawRotationAxis(_targetTransformation->getTMatrix(), program, _markerScale, 5);
        }

        if(_mode == SCALE)
        {
            Graphics::DebugDrawer::drawScaleAxis(_targetTransformation->getTMatrix(), program, _markerScale, 5);
        }

        Graphics::DebugDrawer::drawBoundingBox(*_targetBoundingBox, _targetTransformation->getTRSMatrix(), program, glm::vec3(0.5,0.5,0));

        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][0], _targetTransformation->getTMatrix(), program, glm::vec3(0.3,0,0));
        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][1], _targetTransformation->getTMatrix(), program, glm::vec3(0,0.3,0));
        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][2], _targetTransformation->getTMatrix(), program, glm::vec3(0,0,0.3));

        if(_pickedAnchorAxis < 0) return;

        Graphics::DebugDrawer::drawBoundingBox(_axisAnchors[_pickedAnchorAxis], _targetTransformation->getTMatrix(), program, glm::vec3(1, 1, 1));
    }


    void ObjectPicker::transformPickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        switch(_mode){
            case TRANSLATION:
                translatePickedObject(cursorSpeed, axis, MVP);
                break;
            case ROTATION:
                rotatePickedObject(cursorSpeed, axis, MVP);
                break;
            case SCALE:
                scalePickedObject(cursorSpeed, axis, MVP);
                break;
        }
    }

    void ObjectPicker::translatePickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        glm::vec4 ax = axis == 0 ? glm::vec4(1,0,0,0) : axis == 1 ? glm::vec4(0,1,0,0) : glm::vec4(0,0,1,0);
        ax = MVP * ax;
        ax /= ax.w;
        ax = glm::normalize(ax);

        glm::vec2 speed(-cursorSpeed.x, cursorSpeed.y);

        float trans = glm::dot(speed, glm::vec2(ax));
        trans *= 20.f;

        _targetTransformation->position[axis] += trans;
    }

    void ObjectPicker::scalePickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        glm::vec4 ax = axis == 0 ? glm::vec4(1,0,0,0) : axis == 1 ? glm::vec4(0,1,0,0) : glm::vec4(0,0,1,0);
        ax = MVP * ax;
        ax /= ax.w;
        ax = glm::normalize(ax);

        glm::vec2 speed(-cursorSpeed.x, cursorSpeed.y);

        float trans = glm::dot(speed, glm::vec2(ax));
        trans *= 10.f;

        _targetTransformation->scale[axis] += trans;
    }

    void ObjectPicker::rotatePickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        glm::vec4 ax = axis == 0 ? glm::vec4(1,0,0,0) : axis == 1 ? glm::vec4(0,1,0,0) : glm::vec4(0,0,1,0);
        ax = MVP * ax;
        ax /= ax.w;
        ax = glm::normalize(ax);

        glm::vec2 speed(-cursorSpeed.x, cursorSpeed.y);

        float trans = glm::dot(speed, glm::vec2(ax));

        trans *= 10.f;

        _targetTransformation->rotation[axis] += trans;
    }


    void ObjectPicker::switchMode(PickerMode mode) {
        _mode = mode;
    }
}