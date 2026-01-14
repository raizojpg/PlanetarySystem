#include "Skybox.h"

Skybox::Skybox() {}

void Skybox::CreateVAO() {

    GLfloat Vertices[] =
    {
        -500.0f, -500.0f, -500.0f, 1.0f,
         500.0f, -500.0f, -500.0f, 1.0f,
         500.0f,  500.0f, -500.0f, 1.0f,
        -500.0f,  500.0f, -500.0f, 1.0f,
        
        -500.0f, -500.0f,  500.0f, 1.0f,
         500.0f, -500.0f,  500.0f, 1.0f,
         500.0f,  500.0f,  500.0f, 1.0f,
        -500.0f,  500.0f,  500.0f, 1.0f
    };

    GLfloat Colors[] = {
        0.2f, 0.2f, 0.2f, 1.0f,
        0.18f, 0.18f, 0.18f, 1.0f,
        0.22f, 0.22f, 0.22f, 1.0f,
        0.16f, 0.16f, 0.16f, 1.0f,

        0.2f, 0.2f, 0.2f, 1.0f,
        0.18f, 0.18f, 0.18f, 1.0f,
        0.22f, 0.22f, 0.22f, 1.0f,
        0.16f, 0.16f, 0.16f, 1.0f
    };

    GLuint Indices[] =
    {
        0, 1, 2,
        2, 3, 0,
        4, 7, 6,
        6, 5, 4,
        0, 3, 7,
        7, 4, 0,
        1, 5, 6,
        6, 2, 1,
        0, 4, 5,
        5, 1, 0,
        3, 2, 6,
        6, 7, 3   
    };

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);

	glGenBuffers(1, &VbPos);
	glBindBuffer(GL_ARRAY_BUFFER, VbPos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glGenBuffers(1, &VbCol);
	glBindBuffer(GL_ARRAY_BUFFER, VbCol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

void Skybox::Draw(Shader* MyShader) {
	this->Bind();
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

Skybox::~Skybox() {
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VbPos);
	glDeleteBuffers(1, &VbCol);
	glDeleteBuffers(1, &VbMat);
}
