#include "Cube.h"
#define INSTANCE_COUNT 50

Cube::Cube(){}

void Cube::CreateVAO(){

	GLfloat Vertices[] =
	{                		
		-50.0f, -50.0f, -50.0f,  1.0f,
		 50.0f, -50.0f, -50.0f,  1.0f,
		 10.0f,  50.0f, -50.0f,  1.0f,
		-10.0f,  50.0f, -50.0f,  1.0f,
                		
		-50.0f, -50.0f,  50.0f,  1.0f,
		 50.0f, -50.0f,  50.0f,  1.0f,
		 10.0f,  50.0f,  50.0f,  1.0f,
		-10.0f,  50.0f,  50.0f,  1.0f,
	};

	glm::vec4 Colors[INSTANCE_COUNT];
	for (int instID = 0; instID < INSTANCE_COUNT; instID++)
	{
		Colors[instID][0] = 0.35f + 0.30f * (sinf(instID / 4.0f + 2.0f) + 1.0f);
		Colors[instID][1] = 0.25f + 0.25f * (sinf(instID / 5.0f + 3.0f) + 1.0f);
		Colors[instID][2] = 0.25f + 0.35f * (sinf(instID / 6.0f + 4.0f) + 1.0f);
		Colors[instID][3] = 1.0f;
	}

	glm::mat4 MatModel[INSTANCE_COUNT];
	for (int instID = 0; instID < INSTANCE_COUNT; instID++)
	{
		MatModel[instID] =
			glm::translate(glm::mat4(1.0f), glm::vec3(90 * instID * cos(instID), 90 * instID * sin(instID), -90 * instID))
			* glm::rotate(glm::mat4(1.0f), (instID + 1) * PI / 10, glm::vec3(1, 0, 0))
			* glm::rotate(glm::mat4(1.0f), instID * PI / 6, glm::vec3(0, 1, 0))
			* glm::rotate(glm::mat4(1.0f), 3 * instID * PI / 2, glm::vec3(0, 0, 1));
	}

	GLuint Indices[] =
	{
		1, 0, 2,   2, 0, 3,
		2, 3, 6,   6, 3, 7,
		7, 3, 4,   4, 3, 0,
		4, 0, 5,   5, 0, 1,
		1, 2, 5,   5, 2, 6,
		5, 6, 4,   4, 6, 7,
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4,
		1, 5,
		2, 6,
		3, 7,
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
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);

	glGenBuffers(1, &VbMat);
	glBindBuffer(GL_ARRAY_BUFFER, VbMat);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i,
			4, GL_FLOAT, GL_FALSE,
			sizeof(glm::mat4),
			(void*)(sizeof(glm::vec4) * i));
		glVertexAttribDivisor(2 + i, 1);
	}

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

void Cube::Draw(Shader* MyShader){
	this->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, INSTANCE_COUNT);
}

void Cube::DrawEdges(){
	this->Bind();
	glLineWidth(2.5);
	glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(36 * sizeof(GLuint)), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(40 * sizeof(GLuint)), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(44 * sizeof(GLuint)), INSTANCE_COUNT);
}

Cube::~Cube(){
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VbPos);
	glDeleteBuffers(1, &VbCol);
	glDeleteBuffers(1, &VbMat);
}
