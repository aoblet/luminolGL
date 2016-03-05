#include <glog/logging.h>
#include "graphics/UBO_keys.hpp"
#include "graphics/Skybox.hpp"

using namespace Graphics;

const std::string Skybox::VERTEX_SHADER_PATH    = "../shaders/skybox.vert";
const std::string Skybox::FRAGMENT_SHADER_PATH  = "../shaders/skybox.frag";
const std::string Skybox::CUBE_MODEL_PATH       = "../assets/models/primitives/cube.obj";

Skybox::Skybox(const CubeMapTexture& tex): _texture(tex), _shaderProgram(VERTEX_SHADER_PATH,"", FRAGMENT_SHADER_PATH),
                                           _cube(CUBE_MODEL_PATH){
    _cube.initGLBuffers();
}

Skybox::Skybox(CubeMapTexture &&tex): _texture(std::move(tex)), _shaderProgram(VERTEX_SHADER_PATH,"", FRAGMENT_SHADER_PATH),
                                      _cube(CUBE_MODEL_PATH){
    _cube.initGLBuffers();
}

void Skybox::draw(const glm::mat4 &MVP) {
    _shaderProgram.updateUniform(UBO_keys::MVP, MVP);
    _shaderProgram.updateUniform(UBO_keys::SKYBOX_CUBE_MAP, 0);
    _shaderProgram.useProgram();
    _texture.bind(GL_TEXTURE0);
    _cube.draw(1);
    _texture.unbind();
}
