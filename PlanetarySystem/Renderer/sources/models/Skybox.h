#pragma once
#include "../Model.h"

class Skybox : public Model {
public:
	Skybox();
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	~Skybox();

private:
	GLuint VbPos, VbCol, VbMat;
};

