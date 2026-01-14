#include "Shader.h"
#include "loadShaders.h"

Shader::Shader(){}

void Shader::Create(const char* vs, const char* fs){
	ProgramId = LoadShaders(vs, fs);
	glUseProgram(ProgramId);
}

void Shader::Bind() {
	glUseProgram(ProgramId);
}

void Shader::updateMaterial(Material& material){
	setUniformVec3("materialShader.emission", material.emission);
	setUniformVec3("materialShader.ambient", material.ambient);
	setUniformVec3("materialShader.diffuse", material.diffuse);
	setUniformVec3("materialShader.specular", material.specular);
	setUniformFloat("materialShader.shininessValue", material.shininessValue);
}

void Shader::updateLight(Light& light){
	setUniformVec4("lightShader.position", light.position);
	setUniformVec3("lightShader.ambient", light.ambient);
	setUniformVec3("lightShader.diffuse", light.diffuse);
	setUniformVec3("lightShader.specular", light.specular);
	setUniformVec3("lightShader.attenuation", light.attenuation);
}

void Shader::setUniformVec3(const char* loc, glm::vec3 value){
	GLuint location = glGetUniformLocation(ProgramId, loc);
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniformVec4(const char* loc, glm::vec4 value){
	GLuint location = glGetUniformLocation(ProgramId, loc);
	glUniform4fv(location, 1, glm::value_ptr(value));
}

void Shader::setUniformMat4(const char* loc, glm::mat4 value) {
	GLuint location = glGetUniformLocation(ProgramId, loc);
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void Shader::setUniformInt(const char* loc, int value) {
	GLuint location = glGetUniformLocation(ProgramId, loc);
	glUniform1i(location, value);
}

void Shader::setUniformFloat(const char* loc, float value){
	GLuint location = glGetUniformLocation(ProgramId, loc);
	glUniform1f(location, value);
}

Shader::~Shader()
{
	glDeleteProgram(ProgramId);
}
