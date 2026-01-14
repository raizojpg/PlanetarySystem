#include "CloudPlane.h"

CloudPlane::CloudPlane(float size) : size(size) {}

void CloudPlane::CreateVAO() {
	const float half = size * 0.5f;
	const GLfloat vertices[] = {
		-half, -half, 0.0f, 0.0f, 0.0f,
		 half, -half, 0.0f, 1.0f, 0.0f,
		 half,  half, 0.0f, 1.0f, 1.0f,
		-half,  half, 0.0f, 0.0f, 1.0f
	};

	const GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
}

void CloudPlane::Draw(Shader* MyShader) {
	this->Bind();
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

CloudPlane::~CloudPlane() {
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}
