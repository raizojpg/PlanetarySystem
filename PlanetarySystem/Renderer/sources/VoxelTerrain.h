#pragma once

#include <vector>

#include "Model.h"

class VoxelTerrain : public Model {
public:
    VoxelTerrain(int cellsX, int cellsY, int cellsZ, float voxelSize);

    void CreateVAO() override;
    void Draw(Shader* MyShader = nullptr) override;

    bool ExplodeRay(const glm::vec3& origin, const glm::vec3& direction, float maxDistance,
        float radius, float strength, glm::vec3& hitPos);
    void CarveSphere(const glm::vec3& center, float radius, float strength);
    void RebuildMesh();

    glm::vec3 getOrigin() const;
    glm::vec3 getSize() const;
    float getVoxelSize() const;

private:
    int cellsX;
    int cellsY;
    int cellsZ;
    int pointsX;
    int pointsY;
    int pointsZ;
    float voxelSize;
    glm::vec3 origin;

    std::vector<float> density;
    std::vector<glm::vec4> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    GLsizei vertexCount;
    bool vaoReady;

    void InitializeDensity();
    void UploadMesh();

    float SampleDensity(const glm::vec3& worldPos) const;
    float SampleGrid(int x, int y, int z) const;
    int Index(int x, int y, int z) const;
    glm::vec3 GridToWorld(int x, int y, int z) const;
    bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance,
        glm::vec3& hitPos) const;
    glm::vec3 ColorFromHeight(float worldZ) const;
};
