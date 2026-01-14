#pragma once
#include "Model.h"
#include "models/Tree.h"

class Terrain : public Model{
public:
	Terrain(int w, int l, int s = 500);
	void CreateVAO() override;
	void Draw(Shader* MyShader = nullptr) override;
	void DrawVegetation(Shader* MyShader = nullptr, const glm::vec3& tileOffset = glm::vec3(0.0f));

	int getWidth();
	int getLength();
	int getStep();
	int getPatchSize();
	int getMaxHeight();
	int getHeightmapTex();
	glm::mat4 getTerrainMat();

	void loadHightmap();
	void updateLodMap(glm::vec3 obs, float lodDistanceScale, const glm::vec2& tileOffset = glm::vec2(0.0f), bool allowRecenter = true);
		
	~Terrain();

private:
	const int WIDTH, LENGTH, NR_VF;
	int step;
	float maxHeight;
	glm::mat4 terrainMat;
	unsigned char* heightData;
	int imgWidth, imgHeight;
	int patchSize, maxLod;
	GLuint heightmapTex;

	struct BufferInfo {
		int start;
		int count;
	};

	struct LodInfo {
		BufferInfo border[2][4];
		BufferInfo center;
	};

	std::vector<LodInfo> lods;
	std::vector<std::vector<short>> lodMap;

	Tree* tree;
};

