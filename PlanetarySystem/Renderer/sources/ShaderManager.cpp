#include "ShaderManager.h"

ShaderManager::ShaderManager(){}

void ShaderManager::Init(){
	MyShader.Create("shaders/Shader.vert", "shaders/Shader.frag");
	MyLightShader.Create("shaders/LightShader.vert", "shaders/LightShader.frag");
	MyTerrainShaderHeightmap.Create("shaders/TerrainShaderHeightmap.vert", "shaders/TerrainShaderHeightmap.frag");
	MyPlanetShader.Create("shaders/PlanetShader.vert", "shaders/PlanetShader.frag");
	MyTerrainShaderNoise.Create("shaders/TerrainShaderNoise.vert", "shaders/TerrainShaderNoise.frag");
	MyVegetationShader.Create("shaders/VegetationShader.vert", "shaders/VegetationShader.frag");
	MySpaceSkyboxShader.Create("shaders/SpaceSkybox.vert", "shaders/SpaceSkybox.frag");
}

void ShaderManager::UpdateTerrainNoise(Terrain& MyTerrain, Camera& MyCamera, Light& MyLight){
	Shader& MyShader = MyTerrainShaderNoise;
	
	MyShader.updateLight(MyLight);
	MyShader.setUniformVec3("obsShader", MyCamera.getObs());
	MyShader.setUniformMat4("viewMatrix", MyCamera.getView());
	MyShader.setUniformMat4("projectionMatrix", MyCamera.getProjection());
	MyShader.setUniformVec3("viewPos", glm::vec4(MyCamera.getObs(), 0) - MyTerrain.getTerrainMat()[3]);
	
	MyShader.setUniformMat4("modelMatrix", MyTerrain.getTerrainMat());
	MyShader.setUniformInt("codCol", 0);
	MyShader.updateMaterial(MyTerrain.getMaterial());

	MyShader.setUniformInt("usingNoise", 1);
	MyShader.setUniformFloat("maxHeight", MyTerrain.getMaxHeight());
	MyShader.setUniformFloat("noiseScale", 0.00015f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, MyTerrain.getHeightmapTex());
	MyShader.setUniformInt("heightmap", 0);
	MyShader.setUniformFloat("heightmapScale", (MyTerrain.getPatchSize() - 1) * (MyTerrain.getPatchSize() - 1) * MyTerrain.getStep() / 2);

}

void ShaderManager::UpdateVegetation(Terrain& MyTerrain, Camera& MyCamera, Light& MyLight) {
	Shader& MyShader = MyVegetationShader;

	MyShader.updateLight(MyLight);
	MyShader.setUniformVec3("obsShader", MyCamera.getObs());
	MyShader.setUniformMat4("viewMatrix", MyCamera.getView());
	MyShader.setUniformMat4("projectionMatrix", MyCamera.getProjection());
	MyShader.setUniformVec3("viewPos", glm::vec4(MyCamera.getObs(), 0) - MyTerrain.getTerrainMat()[3]);

	MyShader.setUniformMat4("modelMatrix", MyTerrain.getTerrainMat());
	MyShader.setUniformInt("codCol", 0);
	MyShader.updateMaterial(MyTerrain.getMaterial());

	MyShader.setUniformInt("usingNoise", 1);
	MyShader.setUniformFloat("maxHeight", MyTerrain.getMaxHeight());
	MyShader.setUniformFloat("noiseScale", 0.00015f);
}


ShaderManager::~ShaderManager(){}
