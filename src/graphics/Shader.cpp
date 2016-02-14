#include "graphics/Shader.hpp"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace Graphics;

Shader::Shader(GLenum type, const std::string &path): _idShader(0), _type(type), _path(path) {
    if(_path.empty())
        return;
    compile();
}


void Shader::compile() {
    std::ifstream shaderFile(_path);

    if(!shaderFile.is_open())
        throw std::runtime_error(std::string("Shader::compile error: Unable to load the file " + _path).c_str());

    std::stringstream shaderContent;
    shaderContent << shaderFile.rdbuf();
    std::string t = shaderContent.str();
    const char * shaderString = t.c_str();

    _idShader = glCreateShader(_type);
    glShaderSource(_idShader, 1, &shaderString, 0);
    glCompileShader(_idShader);
    checkCompile();
}

void Shader::checkCompile() const {
    // Get error log size and print it eventually
    int logLength;
    glGetShaderiv(_idShader, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 1) {
        std::string log;
        log.reserve(logLength);
        glGetShaderInfoLog(_idShader, logLength, &logLength, &(log[0]));
        std::cerr << log.c_str() << std::endl ;
    }

    int status;
    glGetShaderiv(_idShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
        throw std::runtime_error(std::string("Shader::checkCompile error while compiling the shader" + _path).c_str());
}

Shader::~Shader() {
    if(_idShader == 0)
        return;

    glDeleteShader(_idShader);
}

std::string Shader::getPath() const {
    return _path;
}

void Shader::changeProperties(GLenum type, const std::string &path) {
    _type = type;
    _path = path;
    compile();
}

GLuint Shader::id() const{
    return _idShader;
}