#pragma once
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "MaterialManager.h"
#include "LightManager.h"

class Shader
{
public:
	Shader();
	void Create(const char* vs, const char* fs);
	void Bind();

	void updateMaterial(Material& material);
	void updateLight(Light& light);

	void setUniformVec3(const char* loc, glm::vec3 value);
	void setUniformVec4(const char* loc, glm::vec4 value);
	void setUniformMat4(const char* loc, glm::mat4 value);
	void setUniformInt(const char* loc, int value);
	void setUniformFloat(const char* loc, float value);
	~Shader();

private:
	GLuint ProgramId;
};

