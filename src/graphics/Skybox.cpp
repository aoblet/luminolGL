#include <glog/logging.h>
#include "graphics/UBO_keys.hpp"
#include "graphics/Skybox.hpp"

using namespace Graphics;

const std::string Skybox::VERTEX_SHADER_PATH    = "../shaders/skybox.vert";
const std::string Skybox::FRAGMENT_SHADER_PATH  = "../shaders/skybox.frag";

Skybox::Skybox(const CubeMapTexture& tex): _texture(tex), _shaderProgram(VERTEX_SHADER_PATH,"", FRAGMENT_SHADER_PATH){}
Skybox::Skybox(CubeMapTexture &&tex): _texture(std::move(tex)), _shaderProgram(VERTEX_SHADER_PATH,"", FRAGMENT_SHADER_PATH){}

ShaderProgram &Skybox::shaderProgram() {
    return _shaderProgram;
}

void Skybox::updateUniforms(const glm::mat4 &screenToWorldNoTranslate, int unitTexSky, int unitTexDepth, int unitTexBeauty) {
    _shaderProgram.updateUniform(UBO_keys::SKYBOX_CUBE_MAP, unitTexSky);
    _shaderProgram.updateUniform(UBO_keys::SKYBOX_DEPTH_BUFFER, unitTexDepth);
    _shaderProgram.updateUniform(UBO_keys::SKYBOX_BEAUTY, unitTexBeauty);
    _shaderProgram.updateUniform(UBO_keys::SKYBOX_SCREEN_TO_WORLD_NO_TRANSLATE, screenToWorldNoTranslate);
}

void Skybox::bindTexture(GLenum unitTex) {
    _texture.bind(unitTex);
}

void Skybox::useProgramShader() {
    _shaderProgram.useProgram();
}
