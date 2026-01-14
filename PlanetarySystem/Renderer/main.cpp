#include <GL/glew.h>
#include <GL/freeglut.h>

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "sources/ModelManager.h"
#include "sources/ShaderManager.h"
#include "sources/InputManager.h"
#include "sources/SolarSystem.h"

ModelManager models;
SolarSystem solarSystem;
ShaderManager shaders;
LightManager lights;

Camera MyCamera;
InputManager inputs(MyCamera);

GLint winWidth = 1000, winHeight = 600;
bool renderMode = 0;
constexpr float kTerrainToSpaceHeight = 20000.0f;
glm::vec3 lastTerrainRef(0.0f);
glm::vec3 lastTerrainVert(0.0f, 0.0f, 1.0f);
float lastTerrainAlpha = 0.0f;
float lastTerrainBeta = 0.0f;
float lastTerrainDist = 1500.0f;
bool hasLastTerrainState = false;
float renderDistanceScale = 1.0f;
constexpr float kRenderDistanceMin = 0.25f;
constexpr float kRenderDistanceMax = 4.0f;
constexpr float kRenderDistanceStep = 0.1f;
constexpr int kMaxTileRadius = 8;

static std::string FormatFloat(double value, int precision) {
	std::ostringstream out;
	out.setf(std::ios::fixed);
	out << std::setprecision(precision) << value;
	return out.str();
}

static int GetTileRadius(float scale) {
	if (scale <= 1.0f) {
		return 0;
	}
	int radius = static_cast<int>(std::ceil(scale)) - 1;
	if (radius < 0) {
		radius = 0;
	}
	if (radius > kMaxTileRadius) {
		radius = kMaxTileRadius;
	}
	return radius;
}

static void DrawText(int x, int y, const std::string& text) {
	glRasterPos2i(x, y);
	for (unsigned char c : text) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
	}
}

static void RenderHud(double fps, float height, float renderDistance) {
	glUseProgram(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, winWidth, 0.0, winHeight, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(1.0f, 0.9f, 0.25f);

	int x = 10;
	int y = winHeight - 20;
	int lineHeight = 16;

	DrawText(x, y, "FPS: " + FormatFloat(fps, 1));
	y -= lineHeight;
	DrawText(x, y, "Height: " + FormatFloat(height, 1));
	y -= lineHeight;
	DrawText(x, y, "Render distance: " + FormatFloat(renderDistance, 1));
	y -= lineHeight * 2;
	DrawText(x, y, "Controls: WASD move, +/- zoom");
	y -= lineHeight;
	DrawText(x, y, "Mouse: drag orbit, wheel render distance");
	y -= lineHeight;
	DrawText(x, y, "Q/E: wireframe/fill");

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void ProcessNormalKeys(unsigned char key, int x, int y) {
	inputs.ProcessNormalKeys(key, x, y);
}

void ProcessSpecialKeys(int key, int x, int y) {
	inputs.ProcessSpecialKeys(key, x, y);
}

void MouseButton(int button, int state, int x, int y) {
	inputs.MouseButton(button, state, x, y);
}

void MouseMotion(int x, int y) {
	inputs.MouseMotion(x, y);
}

void MouseWheel(int wheel, int direction, int x, int y) {
	if (direction > 0) {
		renderDistanceScale += kRenderDistanceStep;
	}
	else if (direction < 0) {
		renderDistanceScale -= kRenderDistanceStep;
	}
	if (renderDistanceScale < kRenderDistanceMin) {
		renderDistanceScale = kRenderDistanceMin;
	}
	else if (renderDistanceScale > kRenderDistanceMax) {
		renderDistanceScale = kRenderDistanceMax;
	}
}

void ReshapeFunction(GLint newWidth, GLint newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	winWidth = newWidth;
	winHeight = newHeight;
	MyCamera.widthR() = winWidth / 10, MyCamera.heightR() = winHeight / 10;
}

void Cleanup(void){
}

void Initialize(void)
{
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
	shaders.Init();
	models.Initialize();
	solarSystem.Initialize();

}
void RenderFunction(void)
{
	static auto lastFpsTime = std::chrono::steady_clock::now();
	static int frameCount = 0;
	static double currentFps = 0.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW); 
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	if (renderMode == 0) {

		MyCamera.Update();

		Shader& MyShader = shaders.MyTerrainShaderNoise;
		MyShader.Bind();

		models.MyTerrain->updateLodMap(MyCamera.getObs(), renderDistanceScale);
		shaders.UpdateTerrainNoise(*models.MyTerrain, MyCamera, lights.myLight);
		float tileSize = static_cast<float>(models.MyTerrain->getWidth() - 1) * models.MyTerrain->getStep();
		glm::mat4 baseTerrainMat = models.MyTerrain->getTerrainMat();
		int tileRadius = GetTileRadius(renderDistanceScale);

		for (int ty = -tileRadius; ty <= tileRadius; ++ty) {
			for (int tx = -tileRadius; tx <= tileRadius; ++tx) {
				glm::vec3 tileOffset(tileSize * tx, tileSize * ty, 0.0f);
				models.MyTerrain->updateLodMap(MyCamera.getObs(), renderDistanceScale, glm::vec2(tileOffset.x, tileOffset.y), false);
				glm::mat4 tileMatrix = baseTerrainMat * glm::translate(glm::mat4(1.0f), tileOffset);
				glm::vec3 viewPos = glm::vec3(glm::vec4(MyCamera.getObs(), 0.0f) - tileMatrix[3]);
				MyShader.setUniformMat4("modelMatrix", tileMatrix);
				MyShader.setUniformVec3("viewPos", viewPos);
				models.MyTerrain->Draw();
			}
		}

		MyShader.setUniformInt("usingNoise", 0);

		glm::mat4 skyboxMat = glm::translate(glm::mat4(1.0f), MyCamera.getObs()) * glm::scale(glm::mat4(1.0f), glm::vec3(50, 50, 100));
		MyShader.setUniformMat4("modelMatrix", skyboxMat);
		MyShader.setUniformInt("codCol", 2);
		MyShader.updateMaterial(models.MySkybox->getMaterial());
		models.MySkybox->Draw();

		shaders.MyVegetationShader.Bind();
		shaders.UpdateVegetation(*models.MyTerrain, MyCamera, lights.myLight);
		for (int ty = -tileRadius; ty <= tileRadius; ++ty) {
			for (int tx = -tileRadius; tx <= tileRadius; ++tx) {
				glm::vec3 tileOffset(tileSize * tx, tileSize * ty, 0.0f);
				glm::mat4 tileMatrix = baseTerrainMat * glm::translate(glm::mat4(1.0f), tileOffset);
				glm::vec3 viewPos = glm::vec3(glm::vec4(MyCamera.getObs(), 0.0f) - tileMatrix[3]);
				shaders.MyVegetationShader.setUniformVec3("viewPos", viewPos);
				models.MyTerrain->DrawVegetation(&shaders.MyVegetationShader, tileOffset);
			}
		}

	}
	else {
		MyCamera.Update();
		Shader& skyboxShader = shaders.MySpaceSkyboxShader;
		skyboxShader.Bind();
		glm::mat4 skyboxView = glm::mat4(glm::mat3(MyCamera.getView()));
		skyboxShader.setUniformMat4("viewMatrix", skyboxView);
		skyboxShader.setUniformMat4("projectionMatrix", MyCamera.getProjection());
		glm::mat4 skyboxMat = glm::scale(glm::mat4(1.0f), glm::vec3(50000.0f));
		skyboxShader.setUniformMat4("modelMatrix", skyboxMat);

		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);
		models.MySkybox->Draw();
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glDepthMask(GL_TRUE);

		Shader& MyShader = shaders.MyPlanetShader;
		MyShader.Bind();
		MyShader.setUniformMat4("projection", MyCamera.getProjection());
		MyShader.setUniformMat4("viewMatrix", MyCamera.getView());
		MyShader.setUniformVec3("viewPos", MyCamera.getObs());
		solarSystem.Update(MyShader);
		models.Update(MyShader);
	}
	int collisionIndex = (renderMode == 1) ? solarSystem.DetectCollision(MyCamera.getObs()) : -1;
	if (renderMode == 1 && collisionIndex != -1) {
		renderMode = 0;
		if (hasLastTerrainState) {
			float returnHeight = -kTerrainToSpaceHeight;
			if (collisionIndex == 0) {
				returnHeight = -kTerrainToSpaceHeight * 0.8f;
			}
			MyCamera.setRef(lastTerrainRef.x, lastTerrainRef.y, returnHeight);
			MyCamera.setVert(lastTerrainVert.x, lastTerrainVert.y, lastTerrainVert.z);
			MyCamera.alphaR() = lastTerrainAlpha;
			MyCamera.betaR() = lastTerrainBeta;
			MyCamera.distR() = lastTerrainDist;
		}
		else {
			MyCamera.setRef(0, 0, 0);
			MyCamera.setVert(0, 0, 1);
		}
		MyCamera.Update();
	}
	float currentHeight = std::abs(MyCamera.getRef().z);
	if (renderMode == 0 && currentHeight > kTerrainToSpaceHeight) {
		lastTerrainRef = MyCamera.getRef();
		lastTerrainVert = MyCamera.getVert();
		lastTerrainAlpha = MyCamera.alphaR();
		lastTerrainBeta = MyCamera.betaR();
		lastTerrainDist = MyCamera.distR();
		hasLastTerrainState = true;
		renderMode = 1;
		MyCamera.setRef(1000, 1000, 0);
		MyCamera.setVert(0, 0, 1);
		MyCamera.Update();
	}
	float displayHeight = std::abs(MyCamera.getRef().z);
	float renderDistance = models.MyTerrain->getStep()
		* models.MyTerrain->getPatchSize() * 2.0f * renderDistanceScale;
	++frameCount;
	auto now = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFpsTime);
	if (elapsedMs.count() >= 1000) {
		double seconds = elapsedMs.count() / 1000.0;
		double fps = frameCount / seconds;
		currentFps = fps;
		frameCount = 0;
		lastFpsTime = now;
	}
	RenderHud(currentFps, displayHeight, renderDistance);
	glutSwapBuffers();
	glFlush();
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(winWidth, winHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Render");
	glewInit();
	Initialize();
	glutReshapeFunc(ReshapeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(RenderFunction);
	glutKeyboardFunc(ProcessNormalKeys);
	glutSpecialFunc(ProcessSpecialKeys);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);
	glutCloseFunc(Cleanup);

	glutMainLoop();

	return 0;
}
