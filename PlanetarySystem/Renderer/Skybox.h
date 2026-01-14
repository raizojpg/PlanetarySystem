#pragma once
#include "../Model.h"

class Cube : public Skybox {
public:
	Cube();
	void CreateVAO() override;
	void Draw() override;
	void DrawEdges();
	~Cube();

private:
	GLuint VbPos, VbCol, VbMat;
};

