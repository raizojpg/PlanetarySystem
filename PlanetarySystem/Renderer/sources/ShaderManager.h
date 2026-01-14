#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Terrain.h"
#include "LightManager.h"

class ShaderManager{
public:
	ShaderManager();
	void Init();

	void UpdateTerrainNoise(Terrain& MyTerrain, Camera& MyCamera, Light& MyLight);
	void UpdateVegetation(Terrain& MyTerrain, Camera& MyCamera, Light& MyLight);

	~ShaderManager();
public:
	Shader MyShader;
	Shader MyLightShader;
	Shader MyTerrainShaderHeightmap;
	Shader MyTerrainShaderNoise;
	Shader MyVegetationShader;
	Shader MyPlanetShader;
	Shader MySpaceSkyboxShader;
};
