#pragma once

#include "graphics/Scene.h"
#include "gui/ObjectPicker.h"
#include <GLFW/glfw3.h>
#include <string>

namespace Callbacks{
    struct CallbacksManager{
        static const std::string ASSET_FOLDER_NAME;
        static Graphics::Scene* _scene;
        static GLFWwindow* _window;
        static Gui::ObjectPicker* _objectPicker;


        static void init(GLFWwindow * window, Graphics::Scene * scene, Gui::ObjectPicker* oPicker);
        static void dropCallback(GLFWwindow* window, int nbPaths, const char** paths);
        static void keysCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
    };
}