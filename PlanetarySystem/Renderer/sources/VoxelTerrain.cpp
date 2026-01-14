#include "VoxelTerrain.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "MarchingCubesTables.h"
#include "glm/gtc/noise.hpp"

namespace {
const glm::ivec3 kCornerOffsets[8] = {
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 0},
    {0, 1, 0},
    {0, 0, 1},
    {1, 0, 1},
    {1, 1, 1},
    {0, 1, 1}
};

const int kEdgeCorners[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

glm::vec3 VertexInterp(float iso, const glm::vec3& p1, const glm::vec3& p2, float v1, float v2) {
    const float eps = 0.00001f;
    if (std::abs(iso - v1) < eps) {
        return p1;
    }
    if (std::abs(iso - v2) < eps) {
        return p2;
    }
    if (std::abs(v1 - v2) < eps) {
        return p1;
    }
    float t = (iso - v1) / (v2 - v1);
    return p1 + t * (p2 - p1);
}
} // namespace

VoxelTerrain::VoxelTerrain(int cellsX, int cellsY, int cellsZ, float voxelSize)
    : cellsX(cellsX),
      cellsY(cellsY),
      cellsZ(cellsZ),
      pointsX(cellsX + 1),
      pointsY(cellsY + 1),
      pointsZ(cellsZ + 1),
      voxelSize(voxelSize),
      origin(-0.5f * cellsX * voxelSize, -0.5f * cellsY * voxelSize, -0.5f * cellsZ * voxelSize),
      vertexCount(0),
      vaoReady(false) {
    VaoId = 0;
    VboId = 0;
    EboId = 0;
    density.resize(pointsX * pointsY * pointsZ, 1.0f);
    setTransform(glm::translate(glm::mat4(1.0f), origin));
    InitializeDensity();
    RebuildMesh();
}

void VoxelTerrain::CreateVAO() {
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);

    vaoReady = true;
    UploadMesh();
}

void VoxelTerrain::Draw(Shader* MyShader) {
    if (vertexCount <= 0) {
        return;
    }
    this->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

bool VoxelTerrain::ExplodeRay(const glm::vec3& origin, const glm::vec3& direction,
    float maxDistance, float radius, float strength, glm::vec3& hitPos) {
    glm::vec3 hit;
    if (!Raycast(origin, direction, maxDistance, hit)) {
        return false;
    }
    CarveSphere(hit, radius, strength);
    RebuildMesh();
    hitPos = hit;
    return true;
}

void VoxelTerrain::CarveSphere(const glm::vec3& center, float radius, float strength) {
    if (radius <= 0.0f) {
        return;
    }
    glm::vec3 localCenter = (center - origin) / voxelSize;
    float radiusVox = radius / voxelSize;

    int minX = std::max(0, static_cast<int>(std::floor(localCenter.x - radiusVox)));
    int maxX = std::min(pointsX - 1, static_cast<int>(std::ceil(localCenter.x + radiusVox)));
    int minY = std::max(0, static_cast<int>(std::floor(localCenter.y - radiusVox)));
    int maxY = std::min(pointsY - 1, static_cast<int>(std::ceil(localCenter.y + radiusVox)));
    int minZ = std::max(0, static_cast<int>(std::floor(localCenter.z - radiusVox)));
    int maxZ = std::min(pointsZ - 1, static_cast<int>(std::ceil(localCenter.z + radiusVox)));

    for (int z = minZ; z <= maxZ; ++z) {
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                glm::vec3 worldPos = GridToWorld(x, y, z);
                float dist = glm::distance(worldPos, center);
                if (dist <= radius) {
                    float falloff = 1.0f - (dist / radius);
                    density[Index(x, y, z)] += strength * falloff;
                }
            }
        }
    }
}

void VoxelTerrain::RebuildMesh() {
    positions.clear();
    colors.clear();
    normals.clear();
    uvs.clear();

    const float iso = 0.0f;
    const glm::vec3 totalSize = getSize();

    std::array<glm::vec3, 12> edgeVertices;
    std::array<float, 8> cornerValues;
    std::array<glm::vec3, 8> cornerPositions;

    for (int z = 0; z < cellsZ; ++z) {
        for (int y = 0; y < cellsY; ++y) {
            for (int x = 0; x < cellsX; ++x) {
                for (int i = 0; i < 8; ++i) {
                    int gx = x + kCornerOffsets[i].x;
                    int gy = y + kCornerOffsets[i].y;
                    int gz = z + kCornerOffsets[i].z;
                    cornerPositions[i] = glm::vec3(
                        gx * voxelSize,
                        gy * voxelSize,
                        gz * voxelSize
                    );
                    cornerValues[i] = SampleGrid(gx, gy, gz);
                }

                int cubeIndex = 0;
                for (int i = 0; i < 8; ++i) {
                    if (cornerValues[i] < iso) {
                        cubeIndex |= (1 << i);
                    }
                }

                int edges = MarchingCubesTables::kEdgeTable[cubeIndex];
                if (edges == 0) {
                    continue;
                }

                for (int e = 0; e < 12; ++e) {
                    if (edges & (1 << e)) {
                        int c0 = kEdgeCorners[e][0];
                        int c1 = kEdgeCorners[e][1];
                        edgeVertices[e] = VertexInterp(
                            iso,
                            cornerPositions[c0],
                            cornerPositions[c1],
                            cornerValues[c0],
                            cornerValues[c1]
                        );
                    }
                }

                const int* tri = MarchingCubesTables::kTriTable[cubeIndex];
                for (int t = 0; tri[t] != -1; t += 3) {
                    glm::vec3 a = edgeVertices[tri[t]];
                    glm::vec3 b = edgeVertices[tri[t + 1]];
                    glm::vec3 c = edgeVertices[tri[t + 2]];
                    glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
                    if (glm::length(normal) < 0.0001f) {
                        normal = glm::vec3(0.0f, 0.0f, 1.0f);
                    }

                    glm::vec3 worldA = a + origin;
                    glm::vec3 worldB = b + origin;
                    glm::vec3 worldC = c + origin;

                    float uA = totalSize.x > 0.0f ? (a.x / totalSize.x) : 0.0f;
                    float vA = totalSize.y > 0.0f ? (a.y / totalSize.y) : 0.0f;
                    float uB = totalSize.x > 0.0f ? (b.x / totalSize.x) : 0.0f;
                    float vB = totalSize.y > 0.0f ? (b.y / totalSize.y) : 0.0f;
                    float uC = totalSize.x > 0.0f ? (c.x / totalSize.x) : 0.0f;
                    float vC = totalSize.y > 0.0f ? (c.y / totalSize.y) : 0.0f;

                    positions.push_back(glm::vec4(a, 1.0f));
                    colors.push_back(ColorFromHeight(worldA.z) * (0.75f + 0.25f * glm::clamp(normal.z, 0.0f, 1.0f)));
                    normals.push_back(normal);
                    uvs.push_back(glm::vec2(uA, vA));

                    positions.push_back(glm::vec4(b, 1.0f));
                    colors.push_back(ColorFromHeight(worldB.z) * (0.75f + 0.25f * glm::clamp(normal.z, 0.0f, 1.0f)));
                    normals.push_back(normal);
                    uvs.push_back(glm::vec2(uB, vB));

                    positions.push_back(glm::vec4(c, 1.0f));
                    colors.push_back(ColorFromHeight(worldC.z) * (0.75f + 0.25f * glm::clamp(normal.z, 0.0f, 1.0f)));
                    normals.push_back(normal);
                    uvs.push_back(glm::vec2(uC, vC));
                }
            }
        }
    }

    vertexCount = static_cast<GLsizei>(positions.size());
    if (vaoReady) {
        UploadMesh();
    }
}

glm::vec3 VoxelTerrain::getOrigin() const {
    return origin;
}

glm::vec3 VoxelTerrain::getSize() const {
    return glm::vec3(
        static_cast<float>(cellsX) * voxelSize,
        static_cast<float>(cellsY) * voxelSize,
        static_cast<float>(cellsZ) * voxelSize
    );
}

float VoxelTerrain::getVoxelSize() const {
    return voxelSize;
}

void VoxelTerrain::InitializeDensity() {
    const float noiseScale = 0.0014f;
    const float heightAmplitude = voxelSize * 6.0f;
    const float baseHeight = 0.0f;

    for (int z = 0; z < pointsZ; ++z) {
        for (int y = 0; y < pointsY; ++y) {
            for (int x = 0; x < pointsX; ++x) {
                glm::vec3 worldPos = GridToWorld(x, y, z);
                glm::vec2 samplePos(worldPos.x * noiseScale, worldPos.y * noiseScale);
                float noise = glm::perlin(samplePos);
                float height = baseHeight + noise * heightAmplitude;
                density[Index(x, y, z)] = worldPos.z - height;
            }
        }
    }
}

void VoxelTerrain::UploadMesh() {
    glBindVertexArray(VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);

    size_t positionsSize = positions.size() * sizeof(glm::vec4);
    size_t colorsSize = colors.size() * sizeof(glm::vec3);
    size_t normalsSize = normals.size() * sizeof(glm::vec3);
    size_t uvsSize = uvs.size() * sizeof(glm::vec2);
    size_t totalSize = positionsSize + colorsSize + normalsSize + uvsSize;

    if (totalSize == 0) {
        return;
    }

    glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positionsSize, positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, positionsSize, colorsSize, colors.data());
    glBufferSubData(GL_ARRAY_BUFFER, positionsSize + colorsSize, normalsSize, normals.data());
    glBufferSubData(GL_ARRAY_BUFFER, positionsSize + colorsSize + normalsSize, uvsSize, uvs.data());

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(positionsSize));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(positionsSize + colorsSize));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(positionsSize + colorsSize + normalsSize));
}

float VoxelTerrain::SampleDensity(const glm::vec3& worldPos) const {
    glm::vec3 local = (worldPos - origin) / voxelSize;

    if (local.x < 0.0f || local.y < 0.0f || local.z < 0.0f ||
        local.x > static_cast<float>(cellsX) || local.y > static_cast<float>(cellsY) ||
        local.z > static_cast<float>(cellsZ)) {
        return 1.0f;
    }

    int x0 = static_cast<int>(std::floor(local.x));
    int y0 = static_cast<int>(std::floor(local.y));
    int z0 = static_cast<int>(std::floor(local.z));
    int x1 = std::min(x0 + 1, pointsX - 1);
    int y1 = std::min(y0 + 1, pointsY - 1);
    int z1 = std::min(z0 + 1, pointsZ - 1);

    float fx = local.x - x0;
    float fy = local.y - y0;
    float fz = local.z - z0;

    float d000 = SampleGrid(x0, y0, z0);
    float d100 = SampleGrid(x1, y0, z0);
    float d010 = SampleGrid(x0, y1, z0);
    float d110 = SampleGrid(x1, y1, z0);
    float d001 = SampleGrid(x0, y0, z1);
    float d101 = SampleGrid(x1, y0, z1);
    float d011 = SampleGrid(x0, y1, z1);
    float d111 = SampleGrid(x1, y1, z1);

    float d00 = d000 + fx * (d100 - d000);
    float d10 = d010 + fx * (d110 - d010);
    float d01 = d001 + fx * (d101 - d001);
    float d11 = d011 + fx * (d111 - d011);
    float d0 = d00 + fy * (d10 - d00);
    float d1 = d01 + fy * (d11 - d01);
    return d0 + fz * (d1 - d0);
}

float VoxelTerrain::SampleGrid(int x, int y, int z) const {
    x = std::clamp(x, 0, pointsX - 1);
    y = std::clamp(y, 0, pointsY - 1);
    z = std::clamp(z, 0, pointsZ - 1);
    return density[Index(x, y, z)];
}

int VoxelTerrain::Index(int x, int y, int z) const {
    return (z * pointsY + y) * pointsX + x;
}

glm::vec3 VoxelTerrain::GridToWorld(int x, int y, int z) const {
    return origin + glm::vec3(x * voxelSize, y * voxelSize, z * voxelSize);
}

bool VoxelTerrain::Raycast(const glm::vec3& origin, const glm::vec3& direction,
    float maxDistance, glm::vec3& hitPos) const {
    if (glm::length(direction) < 0.0001f) {
        return false;
    }

    glm::vec3 dir = glm::normalize(direction);
    glm::vec3 boxMin = this->origin;
    glm::vec3 boxMax = this->origin + getSize();

    float tmin = 0.0f;
    float tmax = maxDistance;

    for (int axis = 0; axis < 3; ++axis) {
        float o = origin[axis];
        float d = dir[axis];
        float minA = boxMin[axis];
        float maxA = boxMax[axis];
        if (std::abs(d) < 0.00001f) {
            if (o < minA || o > maxA) {
                return false;
            }
            continue;
        }
        float t1 = (minA - o) / d;
        float t2 = (maxA - o) / d;
        if (t1 > t2) {
            std::swap(t1, t2);
        }
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);
        if (tmax < tmin) {
            return false;
        }
    }

    float t = std::max(0.0f, tmin);
    float step = voxelSize * 0.5f;
    float prevDensity = SampleDensity(origin + dir * t);

    while (t <= tmax && t <= maxDistance) {
        t += step;
        glm::vec3 pos = origin + dir * t;
        float currentDensity = SampleDensity(pos);
        if (prevDensity > 0.0f && currentDensity <= 0.0f) {
            float blend = prevDensity / (prevDensity - currentDensity);
            float hitT = (t - step) + blend * step;
            hitPos = origin + dir * hitT;
            return true;
        }
        prevDensity = currentDensity;
    }

    return false;
}

glm::vec3 VoxelTerrain::ColorFromHeight(float worldZ) const {
    float minZ = this->origin.z;
    float maxZ = this->origin.z + static_cast<float>(cellsZ) * voxelSize;
    float t = (worldZ - minZ) / std::max(0.0001f, maxZ - minZ);
    t = glm::clamp(t, 0.0f, 1.0f);
    glm::vec3 lowColor(0.26f, 0.22f, 0.18f);
    glm::vec3 highColor(0.35f, 0.45f, 0.28f);
    return glm::mix(lowColor, highColor, t);
}
