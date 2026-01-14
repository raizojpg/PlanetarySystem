#include "Sphere.h"

Sphere::Sphere(int p, int m, int r) : NR_PARR{ p }, NR_MERID{ m }, NR_VF{(p+1)*(m+1)}, 
	step_u{ (U_MAX - U_MIN) / p }, step_v{ (V_MAX - V_MIN) / m }, radius(r) {}

void Sphere::CreateVAO(){

	std::vector<glm::vec4> Vertices(NR_VF);
	std::vector<glm::vec3> Colors(NR_VF);
	std::vector<glm::vec3> Normals(NR_VF);
	std::vector<GLushort> Indices(4 * NR_VF);

	float u, v, x_vf, y_vf, z_vf;
	int index, index1, index2, index3, index4;

	for (int merid = 0; merid < NR_MERID + 1; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			u = U_MIN + parr * step_u;
			v = V_MIN + merid * step_v;
			x_vf = radius * cosf(u) * cosf(v);
			y_vf = radius * cosf(u) * sinf(v);
			z_vf = radius * sinf(u);

			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors[index] = glm::vec3(1, 1, 0);
			Normals[index] = glm::vec3(cosf(u) * cosf(v), cosf(u) * sinf(v), sinf(u));

			if ((parr + 1) % (NR_PARR + 1) != 0)
			{
				index1 = index;
				index2 = index + (NR_PARR + 1);
				index3 = index2 + 1;
				index4 = index + 1;
				if (merid == NR_MERID)
				{
					index2 = index;
					index3 = index;
					index4 = index;
				}
				Indices[4 * index] = index1;
				Indices[4 * index + 1] = index2;
				Indices[4 * index + 2] = index3;
				Indices[4 * index + 3] = index4;
			}

		}
	};

	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glGenBuffers(1, &VboId);
	glGenBuffers(1, &EboId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, NR_VF * sizeof(glm::vec4) + NR_VF * sizeof(glm::vec3) + NR_VF * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, NR_VF * sizeof(glm::vec4), Vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, NR_VF * sizeof(glm::vec4), NR_VF * sizeof(glm::vec3), Colors.data());
	glBufferSubData(GL_ARRAY_BUFFER, NR_VF * sizeof(glm::vec4) + NR_VF * sizeof(glm::vec3), NR_VF * sizeof(glm::vec3), Normals.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(GLushort), Indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(NR_VF * sizeof(glm::vec4)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(NR_VF * sizeof(glm::vec4) + NR_VF * sizeof(glm::vec3)));
}

void Sphere::Draw(Shader* MyShader){
	this->Bind();
	for (int patr = 0; patr < NR_VF; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0)
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((4 * patr) * sizeof(GLushort)));
	}
}

Sphere::~Sphere(){
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}
