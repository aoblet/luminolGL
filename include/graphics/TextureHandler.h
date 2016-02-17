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
        void add(const Texture& texture, const std::string & textureName);
        void add(Texture&& texture, const std::string & textureName);
        const Texture& operator[](const std::string& textureName) const;
        Texture& operator[](const std::string& textureName);
    };
}




#endif //LUMINOLGL_TEXTUREHANDLER_H
