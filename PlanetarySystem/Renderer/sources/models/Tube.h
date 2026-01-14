#pragma once
#include "../Model.h"
class Tube : public Model{
public:
	Tube(int p, int m, int r);
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	~Tube();
private:
	float const U_MIN = -200, U_MAX = 0, V_MIN = 0, V_MAX = 2 * PI;
	int const NR_PARR, NR_MERID, NR_VF;
	float const step_u, step_v, radius;
};

