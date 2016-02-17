//
// Created by mehdi on 12/02/16.
//

#include "graphics/TextureHandler.h"
#include <iostream>

namespace Graphics
{
    TextureHandler::TextureHandler() { }

    void TextureHandler::add(const Texture& texture, const std::string &textureName) {
        _map.insert(std::pair<std::string, Texture>(textureName, texture));
    }

    void TextureHandler::add(Texture&& texture, const std::string& textureName) {
        _map.insert(std::pair<std::string, Texture>(textureName, std::move(texture)));
    }

    const Texture& TextureHandler::operator[](const std::string &textureName) const {
        return _map.at(textureName);
    }

    Texture& TextureHandler::operator[](const std::string &textureName) {
        return _map[textureName];
    }
}
