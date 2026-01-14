#pragma once
#include "../Model.h"

class Tree : public Model{
public:
	Tree();
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	void DrawEdges();
	~Tree();

private:
	GLuint VbPos, VbCol, VbMat, VbNorm, VbUV;
};

