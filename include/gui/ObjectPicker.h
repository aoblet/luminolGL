//
// Created by mehdi on 05/03/16.
//

#ifndef LUMINOLGL_OBJECTPICKER_H
#define LUMINOLGL_OBJECTPICKER_H

#include "graphics/ShaderProgram.hpp"
#include "graphics/Mesh.h"
#include "graphics/Scene.h"
#include "view/CameraFreefly.hpp"
#include "geometry/Transformation.h"

namespace Gui
{
    class ObjectPicker {
    private:
        Geometry::Transformation* _targetTransformation;
        bool _picked;
        float _markerScale;
    public:
        ObjectPicker();
        void pickObject(const glm::vec2 & cursorPosition, Graphics::Scene& scene, const View::CameraFreefly& camera);
        void drawPickedObject(Graphics::ShaderProgram& program);
        void setMarkerScale(float scale);
    };
}




#endif //LUMINOLGL_OBJECTPICKER_H
