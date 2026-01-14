#pragma once

#include <random>
#include <vector>

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class DebrisField {
public:
    explicit DebrisField(size_t maxChunks = 256);

    void CreateVAO();
    void Update(float dtSeconds);
    void Draw();
    void SpawnExplosion(const glm::vec3& center, int count, float minSpeed, float maxSpeed,
        float minSize, float maxSize);

private:
    struct Chunk {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 axis;
        float angularSpeed;
        float angle;
        float lifetime;
        float age;
        float size;
        glm::vec4 color;
    };

    std::vector<Chunk> chunks;
    std::vector<glm::mat4> instanceMatrices;
    std::vector<glm::vec4> instanceColors;

    size_t maxChunks;
    GLuint vao;
    GLuint vboPos;
    GLuint vboColor;
    GLuint vboMat;
    GLuint ebo;
    size_t instanceCount;
    std::mt19937 rng;

    void UploadInstances();
    glm::vec3 RandomUnitVector();
    float RandomRange(float minValue, float maxValue);
};
