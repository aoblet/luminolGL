//
// Created by mehdi on 12/02/16.
//

#include "graphics/TextureHandler.h"

#include <iostream>

namespace Graphics
{
    TextureHandler::TextureHandler() { }

    void TextureHandler::add(const Texture & tex, const std::string &textureName) {
        std::cout << "add " << textureName << std::endl;
        _map.insert(std::pair<std::string, Texture>(textureName, tex));
    }

    Texture& TextureHandler::operator[](const std::string &textureName) {
        return _map[textureName];
    }
}
