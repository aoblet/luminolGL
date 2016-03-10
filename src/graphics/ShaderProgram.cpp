#include "graphics/ShaderProgram.hpp"
#include <stdexcept>
#include <iostream>


using namespace Graphics;

ShaderProgram::ShaderProgram(const std::string &vShader, const std::string& gShader, const std::string& fShader):
    _vertexShader(std::make_shared<Shader>(GL_VERTEX_SHADER, vShader)),
    _geometryShader(std::make_shared<Shader>(GL_GEOMETRY_SHADER, gShader)),
    _fragShader(std::make_shared<Shader>(GL_FRAGMENT_SHADER, fShader)) {

    compile();
}

void ShaderProgram::compile() {
    _programId = glCreateProgram();
    glAttachShader(_programId, _vertexShader.get()->id());
    glAttachShader(_programId, _fragShader.get()->id());

    if(_geometryShader.get() && _geometryShader.get()->id() != 0)
        glAttachShader(_programId, _geometryShader.get()->id());

    glLinkProgram(_programId);
    checkLinkErrors();
}

ShaderProgram::ShaderProgram(const std::shared_ptr<Shader>& vShader, const std::shared_ptr<Shader>& fShader):
        _vertexShader(vShader),
        _geometryShader(),
        _fragShader(fShader){
    compile();
}

ShaderProgram::ShaderProgram(const std::shared_ptr<Shader>& vShader, const std::string& fShader):
        _vertexShader(vShader),
        _geometryShader(),
        _fragShader(std::make_shared<Shader>(GL_FRAGMENT_SHADER, fShader)){
    compile();
}

ShaderProgram::ShaderProgram(const std::string& vShader, const std::shared_ptr<Shader>& fShader):
        _vertexShader(std::make_shared<Shader>(GL_VERTEX_SHADER, vShader)),
        _geometryShader(),
        _fragShader(fShader){
    compile();
}

void ShaderProgram::checkLinkErrors() const {
    // Get link error log size and print it eventually
    int logLength;
    glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 1) {
        std::string log(logLength, '0');
        glGetProgramInfoLog(_programId, logLength, &logLength, &(log[0]));
        std::cerr << log;
    }

    int status;
    glGetProgramiv(_programId, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
        throw std::runtime_error("ShaderProgram::checkLinkErrors errror while linking shaders");
}

GLuint ShaderProgram::id() const {
    return _programId;
}

void ShaderProgram::updateUniform(const std::string& uniformName, float v){
    glProgramUniform1f(_programId, glGetUniformLocation(_programId, uniformName.c_str()), v);
}

void ShaderProgram::updateUniform(const std::string& uniformName, int v){
    glProgramUniform1i(_programId, glGetUniformLocation(_programId, uniformName.c_str()), v);
}

void ShaderProgram::updateUniform(const std::string& uniformName, const glm::vec2 & v){
    glProgramUniform2fv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), 1, glm::value_ptr(v));
}

void ShaderProgram::updateUniform(const std::string& uniformName, const glm::ivec2 & v){
    glProgramUniform2iv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), 1, glm::value_ptr(v));
}

void ShaderProgram::updateUniform(const std::string& uniformName, const glm::vec3 & v){
    glProgramUniform3fv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), 1, glm::value_ptr(v));
}

void ShaderProgram::updateUniform(const std::string &uniformName, const glm::mat3 &v) {
    glProgramUniformMatrix3fv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), 1, 0, glm::value_ptr(v));
}

void ShaderProgram::updateUniform(const std::string& uniformName, const glm::mat4 & v){
    glProgramUniformMatrix4fv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), 1, 0, glm::value_ptr(v));
}

void ShaderProgram::updateBindingPointUBO(const std::string &uniformName, GLuint uboBindingPoint){
    glUniformBlockBinding(_programId, glGetUniformBlockIndex(_programId, uniformName.c_str()), uboBindingPoint);
}

void ShaderProgram::updateUniform(const std::string& uniformName, const std::vector<glm::vec3> & v){
    glProgramUniform3fv(_programId, glGetUniformLocation(_programId, uniformName.c_str()), v.size(), (float*)v.data());
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_programId);
    _programId = 0;
}

void ShaderProgram::useProgram() const {
    glUseProgram(_programId);
}

const std::shared_ptr<Shader> ShaderProgram::vShader() const {return _vertexShader; }
const std::shared_ptr<Shader> ShaderProgram::gShader() const {return _geometryShader; }
const std::shared_ptr<Shader> ShaderProgram::fShader() const {return _fragShader; }
