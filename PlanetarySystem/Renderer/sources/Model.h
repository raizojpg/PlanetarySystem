#pragma once
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

#include "Shader.h"
#include "MaterialManager.h"

class Model{
public:
	Model();
	void Bind();
	void Unbind();

	void setMaterial(Material other);
	Material& getMaterial();

	void setTransform(glm::mat4 other);
	glm::mat4& getTransform();

	virtual void CreateVAO() = 0;
	virtual void Draw(Shader* MyShader = nullptr) = 0;
	virtual ~Model();

protected:
	float const PI = 3.141592f;
	GLuint VaoId, VboId, EboId;
	Material material;
	glm::mat4 transform;
};

