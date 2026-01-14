#pragma once
#include "../Model.h"

class Sphere : public Model
{
public:
	Sphere(int p, int m, int r);
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	~Sphere();

private:
	float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
	int const NR_PARR, NR_MERID, NR_VF;
	float const step_u, step_v, radius;
};

