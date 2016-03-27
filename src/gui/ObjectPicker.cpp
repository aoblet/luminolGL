//
// Created by mehdi on 05/03/16.
//

#include <glog/logging.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include "gui/ObjectPicker.h"
#include "graphics/DebugDrawer.h"

namespace Gui
{

    ObjectPicker::ObjectPicker(float markerScale, Graphics::Scene* scene) :
            _picked(false), _pickedAnchorAxis(-1), _markerScale(markerScale), _longClick(false),
            _currentMeshPicked(nullptr), _scene(scene), _mode(PickerMode::TRANSLATION){

        // axis anchor scale
        float s = 0.1f * _markerScale;

        // Create bounding boxes for axis
        std::vector<Geometry::BoundingBox> trans;
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(markerScale,s,s)));
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(s,markerScale,s)));
        trans.push_back(Geometry::BoundingBox(glm::vec3(-s,-s,-s), glm::vec3(s,s,markerScale)));

        _pickerAnchors.push_back(trans);
        _pickerAnchors.push_back(trans);
        _pickerAnchors.push_back(trans);
    }

    void ObjectPicker::pickObject(const glm::vec2 &cursorPosition, const glm::vec2 &cursorSpeed, const View::CameraFreefly &camera, bool click) {

        if(!click && _longClick) _longClick = false;
        if(click && _picked) _longClick = true;

        glm::vec3 front = camera.getFront();
        float mouseNormX = ((cursorPosition.x / camera.getViewPort().x) * 2 * camera.getNormalizedViewPort().x - 1 * camera.getNormalizedViewPort().x);
        float mouseNormY = ((cursorPosition.y / camera.getViewPort().y) * 2 * camera.getNormalizedViewPort().y - 1 * camera.getNormalizedViewPort().y);

        glm::vec3 left = - camera.getLeft() * mouseNormX;
        glm::vec3 up = camera.getUp() * mouseNormY;
        glm::vec3 rayDir = glm::normalize(front + left + up);
        glm::vec3 rayOrigin = camera.getEye();

        float minDist = 100000;
        float dist = 0;

        if(!_longClick) _pickedAnchorAxis = -1;

        if(_picked && !_longClick){
            _camDistance = glm::length(camera.getEye() - _targetTransformation->position);

            for(size_t i = 0; i < _pickerAnchors[int(_mode)].size(); ++i){
                auto pickedAnchor = _pickerAnchors[int(_mode)][i];
                pickedAnchor.transformAAB(glm::scale(glm::vec3(_axisScale)));
                if(pickedAnchor.intersect(rayOrigin, rayDir, _targetTransformation->getTMatrix(), &dist)){
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


        // Pick test onto water and scene meshes
        auto meshInstance = &_scene->water();
        int i = -1;

        // if no object is currently picked & user has clicked, try to find a bounding box that intersect the camera ray
        do{
            for(auto& trans : meshInstance->getTransformations()){
                auto boxPtr = meshInstance->modelMeshGroup().getBoundingBoxPtr();
                auto box = meshInstance->modelMeshGroup().getBoundingBox();
                box.transformAAB(trans.getRMatrix()* trans.getSMatrix());

                if(box.intersect(rayOrigin, rayDir, trans.getTMatrix(), &dist)){
                    if(dist < minDist && dist > 0){
                        minDist = dist;
                        _currentMeshPicked      = meshInstance;
                        _targetTransformation   = &trans;
                        _targetBoundingBox      = boxPtr;
                        _picked = true;
                    }
                }
            }
            meshInstance = &_scene->meshInstances()[++i];
        }
        while(i < (int)_scene->meshInstances().size());
    }

    void ObjectPicker::drawPickedObject(Graphics::ShaderProgram& program) {
        if(!_picked) return;

        _axisScale = 0.05f * _camDistance;

        glm::mat4 axisTransformation = _targetTransformation->getTMatrix() * glm::scale(glm::vec3(_axisScale));

        if(_mode == PickerMode::TRANSLATION)
        {
            Graphics::DebugDrawer::drawTranslateAxis(axisTransformation, program, _markerScale, 5);
        }

        if(_mode == PickerMode::ROTATION)
        {
            Graphics::DebugDrawer::drawRotationAxis(axisTransformation, program, _markerScale, 5);
        }

        if(_mode == PickerMode::SCALE)
        {
            Graphics::DebugDrawer::drawScaleAxis(axisTransformation, program, _markerScale, 5);
        }

        // Draw Bounding box
        Graphics::DebugDrawer::drawBoundingBox(*_targetBoundingBox, _targetTransformation->getTRSMatrix(), program, glm::vec3(0.5,0.5,0));

        // Draw axis
        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][0], axisTransformation, program, glm::vec3(0.3,0,0));
        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][1], axisTransformation, program, glm::vec3(0,0.3,0));
        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][2], axisTransformation, program, glm::vec3(0,0,0.3));

        if(_pickedAnchorAxis < 0) return;

        Graphics::DebugDrawer::drawBoundingBox(_pickerAnchors[int(_mode)][_pickedAnchorAxis], axisTransformation, program, glm::vec3(1, 1, 1));
    }


    void ObjectPicker::transformPickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        switch(_mode){
            case PickerMode::TRANSLATION:
                translatePickedObject(cursorSpeed, axis, MVP);
                break;
            case PickerMode::ROTATION:
                rotatePickedObject(cursorSpeed, axis, MVP);
                break;
            case PickerMode::SCALE:
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
        trans *= 20.f * _axisScale;

        _targetTransformation->position[axis] += trans;
    }

    void ObjectPicker::transformPickedObject(const glm::vec3& translate, const glm::vec3& scale, const glm::vec4& rot) {
        if(!_targetTransformation)
            return;

        _targetTransformation->position += translate;
        _targetTransformation->scale    *= scale;
        _targetTransformation->rotation = rot;
    }

    void ObjectPicker::scalePickedObject(const glm::vec2 &cursorSpeed, int axis, const glm::mat4 &MVP) {
        glm::vec4 ax = axis == 0 ? glm::vec4(1,0,0,0) : axis == 1 ? glm::vec4(0,1,0,0) : glm::vec4(0,0,1,0);
        ax = MVP * ax;
        ax /= ax.w;
        ax = glm::normalize(ax);

        glm::vec2 speed(-cursorSpeed.x, cursorSpeed.y);

        float trans = glm::dot(speed, glm::vec2(ax));
        trans *= 10.f * _axisScale;

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

    void ObjectPicker::deletePickedObject() {
        assert(_scene != nullptr);

        if(!_picked || !_currentMeshPicked || !_targetTransformation)
            return;

        _scene->deleteMeshByPtr(_currentMeshPicked, _targetTransformation);
        reset();
    }

    void ObjectPicker::attachToScene(Graphics::Scene *scene) {
        _scene = scene;
    }

    void ObjectPicker::duplicatePickedObject() {
        assert(_scene != nullptr);

        if(!_picked || !_currentMeshPicked || !_targetTransformation)
            return;

        _targetTransformation = &_scene->duplicateMesh(*_currentMeshPicked, *_targetTransformation);
    }

    void ObjectPicker::reset(){
        _currentMeshPicked      = nullptr;
        _targetBoundingBox      = nullptr;
        _targetTransformation   = nullptr;
        _picked = false;
    }

    bool ObjectPicker::isPicked() {
        return _picked;
    }

    Geometry::Transformation *ObjectPicker::currentTransformation() {
        return _targetTransformation;
    }

}