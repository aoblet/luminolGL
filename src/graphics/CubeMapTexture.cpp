#include "graphics/CubeMapTexture.hpp"
#include "stb/stb_image.h"
#include <memory>
#include <glog/logging.h>

using namespace Graphics;

CubeMapTexture::CubeMapTexture(const std::string &folderTextures, const std::vector<std::string> &files,
                               const std::string& extension):
        _texId(0), _folder(folderTextures), _files(files){

    if(!files.empty() && files.size() != 6)
        throw std::runtime_error("CubeMapTexture error: given files size != 6");

    glGenTextures(1, &_texId);

    int width, height, bitDepth;
    unsigned char* image;

    if(_files.empty()){
        _files = {"right", "left", "top", "bottom", "back", "front"};
        for(auto& f: _files)
            f += extension;
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, _texId);

    std::string _folder = folderTextures;
    if(_folder.find_last_of('/') != _folder.size()-1)
        _folder += '/';

    for(GLuint i = 0; i < _files.size(); ++i){
        image = nullptr;
        std::string filename = _folder + _files[i];
        image = stbi_load(filename.c_str(), &width, &height, &bitDepth, 3);
        if(!image){
            DLOG(ERROR) << "Error throwing exception";
            throw std::runtime_error("CubeMapTexture error when loading image " + filename +" : data empty");
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMapTexture::CubeMapTexture(CubeMapTexture &&other) {
    std::swap(_texId, other._texId);
    std::swap(_folder, other._folder);
    std::swap(_files, other._files);
    other._texId = 0;
}

CubeMapTexture::CubeMapTexture(const CubeMapTexture &other):CubeMapTexture(other._folder, other._files) {}

CubeMapTexture::~CubeMapTexture() {
    glDeleteTextures(1, &_texId);
}

void CubeMapTexture::bind(GLenum unitTextureTarget) {
    glActiveTexture(unitTextureTarget);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _texId);
}

void CubeMapTexture::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
