//
// Created by mehdi on 12/02/16.
//

#ifndef LUMINOLGL_TEXTUREHANDLER_H
#define LUMINOLGL_TEXTUREHANDLER_H

#include "graphics/Texture.h"

#include <unordered_map>
#include <string>

namespace Graphics
{


    class TextureHandler {
    private:
        std::unordered_map<std::string, Texture> _map;
    public:
        TextureHandler();
        void add(const Texture& tex, const std::string & textureName);
        Texture& operator[](const std::string& textureName);
    };
}




#endif //LUMINOLGL_TEXTUREHANDLER_H
