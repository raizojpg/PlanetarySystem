#include "Tree.h"
#define INSTANCE_COUNT 5

Tree::Tree(){}

void Tree::CreateVAO(){

	GLfloat Vertices[] =
	{                		
		0.0f,   0.0f,   0.0f,   1.0f,  
		20.0f,  0.0f,   0.0f,   1.0f,  
		20.0f,  0.0f, 400.0f,   1.0f,  
		0.0f,   0.0f, 400.0f,   1.0f,   
		0.0f,  20.0f,   0.0f,   1.0f,  
		20.0f, 20.0f,   0.0f,   1.0f, 
		20.0f, 20.0f, 400.0f,   1.0f,   
		0.0f,  20.0f, 400.0f,   1.0f,  

		-50.0f,  -50.0f, 0.0f, 1.0f,   
		 70.0f,  -50.0f, 0.0f, 1.0f,  
		 70.0f,   70.0f, 0.0f, 1.0f,  
		-50.0f,   70.0f, 0.0f, 1.0f,   
		-50.0f,  -50.0f, 80.0f, 1.0f,  
		 70.0f,  -50.0f, 80.0f, 1.0f,  
		 70.0f,   70.0f, 80.0f, 1.0f,  
		-50.0f,   70.0f, 80.0f, 1.0f
	};

	GLfloat Normals[] = {
		0, 0, -1,
		0, 0, -1,
		0, 0, -1,
		0, 0, -1,
		0, 0,  1,
		0, 0,  1,
		0, 0,  1,
		0, 0,  1,

		0, 0, -1,
		0, 0, -1,
		0, 0, -1,
		0, 0, -1,
		0, 0,  1,
		0, 0,  1,
		0, 0,  1,
		0, 0,  1,
	};

	GLfloat UVs[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1,
		0, 0,
		1, 0,
		1, 1,
		0, 1,

		0, 0,
		1, 0,
		1, 1,
		0, 1,
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	glm::vec3 Colors[INSTANCE_COUNT];
	for (int instID = 0; instID < INSTANCE_COUNT; instID++)
	{
		Colors[instID][0] = 0.7f + 0.1f * sinf(instID * 1.2f);
		Colors[instID][1] = 0.7f + 0.1f * sinf(instID * 1.5f + 1.0f); 
		Colors[instID][2] = 0.7f + 0.1f * sinf(instID * 1.8f + 2.0f);
	}

	glm::mat4 MatModel[INSTANCE_COUNT];
	for (int instID = 0; instID < INSTANCE_COUNT; instID++)
	{
		MatModel[instID] =
			glm::translate(glm::mat4(1.0f), glm::vec3(
				180 * instID * cos(instID * 1.7f), 
				180 * instID * sin(instID * 2.3f), 
				-400))
			* glm::rotate(glm::mat4(1.0f), ((instID + 1) * PI / 100), glm::vec3(1, 0, 0))
			* glm::rotate(glm::mat4(1.0f), (instID * PI / 60), glm::vec3(0, 1, 0))    
			* glm::rotate(glm::mat4(1.0f), (3 * instID * PI / 20), glm::vec3(0, 0, 1));
	}

	GLuint Indices[] =
	{
		1, 0, 2,   2, 0, 3,
		2, 3, 6,   6, 3, 7,
		7, 3, 4,   4, 3, 0,
		4, 0, 5,   5, 0, 1,
		1, 2, 5,   5, 2, 6,
		5, 6, 4,   4, 6, 7,
		
		9, 8, 10,   10, 8, 11,      
		10, 11, 14,  14, 11, 15,   
		15, 11, 12,  12, 11, 8,     
		12, 8, 13,   13, 8, 9,     
		9, 10, 13,   13, 10, 14, 
		13, 14, 12,  12, 14, 15
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glVertexAttribDivisor(1, 1);

	glGenBuffers(1, &VbNorm);
	glBindBuffer(GL_ARRAY_BUFFER, VbNorm);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glGenBuffers(1, &VbUV);
	glBindBuffer(GL_ARRAY_BUFFER, VbUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

	glGenBuffers(1, &VbMat);
	glBindBuffer(GL_ARRAY_BUFFER, VbMat);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MatModel), MatModel, GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		glEnableVertexAttribArray(4 + i);
		glVertexAttribPointer(4 + i,
			4, GL_FLOAT, GL_FALSE,
			sizeof(glm::mat4),
			(void*)(sizeof(glm::vec4) * i));
		glVertexAttribDivisor(4 + i, 1);
	}

	glGenBuffers(1, &EboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

void Tree::Draw(Shader* MyShader){
	this->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, 36 + 36, GL_UNSIGNED_INT, 0, INSTANCE_COUNT);
}

void Tree::DrawEdges(){
	this->Bind();
	glLineWidth(2.5);
	/*glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(36 * sizeof(GLuint)), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (void*)(40 * sizeof(GLuint)), INSTANCE_COUNT);
	glDrawElementsInstanced(GL_LINES, 8, GL_UNSIGNED_INT, (void*)(44 * sizeof(GLuint)), INSTANCE_COUNT);*/
}

Tree::~Tree(){
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glDeleteBuffers(1, &VbPos);
	glDeleteBuffers(1, &VbCol);
	glDeleteBuffers(1, &VbMat);
}
