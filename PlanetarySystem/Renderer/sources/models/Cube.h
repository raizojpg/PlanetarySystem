#pragma once
#include "../Model.h"

class Cube : public Model{
public:
	Cube();
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	void DrawEdges();
	~Cube();

private:
	GLuint VbPos, VbCol, VbMat;
};

