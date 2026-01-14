#pragma once
#include "../Model.h"

class CloudPlane : public Model {
public:
	explicit CloudPlane(float size = 60000.0f);
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	~CloudPlane() override;

private:
	float size;
};
