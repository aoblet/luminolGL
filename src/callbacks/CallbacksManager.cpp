#include "callbacks/CallbacksManager.hpp"
#include <glog/logging.h>

using namespace Callbacks;

const std::string CallbacksManager::ASSET_FOLDER_NAME   = "assets";
Graphics::Scene* CallbacksManager::_scene               = nullptr;
GLFWwindow* CallbacksManager::_window                   = nullptr;
Gui::ObjectPicker* CallbacksManager::_objectPicker      = nullptr;

void CallbacksManager::init(GLFWwindow *window, Graphics::Scene *scene, Gui::ObjectPicker* oPicker) {
    _window = window;
    _scene = scene;
    _objectPicker = oPicker;

    glfwSetDropCallback(window, &dropCallback);
    glfwSetKeyCallback(window, &keysCallback);
}

void CallbacksManager::dropCallback(GLFWwindow *window, int nbPaths, const char **paths) {
    DLOG(INFO) << "DropCallback triggered";

    for(int i=0; i<nbPaths; ++i){
        std::string path(paths[i]);
        size_t assetPos = path.find(ASSET_FOLDER_NAME);

        if(assetPos == std::string::npos){
            std::string errMsg = "DropCallback::callback cannot import " + path;
            errMsg += ". Must be inside luminolGL/assets folder";
            DLOG(WARNING) << errMsg;
            continue;
        }

        DLOG(INFO) << "DropCallback calls Scene::addModelMeshInstanced";
        _scene->addModelMeshInstanced("../" + ASSET_FOLDER_NAME + path.substr(assetPos+ASSET_FOLDER_NAME.size()));
    }
}

void CallbacksManager::keysCallback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
    if(action != GLFW_PRESS)
        return;

    if(key == GLFW_KEY_DELETE){
        DLOG(INFO) << "keysCallback delete pressed";
        _objectPicker->deletePickedObject();
    }


    if(key == GLFW_KEY_INSERT){
        DLOG(INFO) << "keysCallback duplicate pressed";
        _objectPicker->duplicatePickedObject();
    }
}
