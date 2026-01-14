#include "DebrisField.h"

#include <algorithm>

DebrisField::DebrisField(size_t maxChunks)
    : maxChunks(maxChunks),
      vao(0),
      vboPos(0),
      vboColor(0),
      vboMat(0),
      ebo(0),
      instanceCount(0),
      rng(std::random_device{}()) {}

void DebrisField::CreateVAO() {
    const GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f, 1.0f,
         0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f
    };

    const GLuint indices[] = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        0, 4, 7, 7, 3, 0,
        1, 5, 6, 6, 2, 1,
        3, 2, 6, 6, 7, 3,
        0, 1, 5, 5, 4, 0
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vboPos);
    glBindBuffer(GL_ARRAY_BUFFER, vboPos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, maxChunks * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), reinterpret_cast<void*>(0));
    glVertexAttribDivisor(1, 1);

    glGenBuffers(1, &vboMat);
    glBindBuffer(GL_ARRAY_BUFFER, vboMat);
    glBufferData(GL_ARRAY_BUFFER, maxChunks * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            reinterpret_cast<void*>(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);
    }

    glBindVertexArray(0);
}

void DebrisField::Update(float dtSeconds) {
    if (vao == 0) {
        return;
    }

    const glm::vec3 gravity(0.0f, 0.0f, -900.0f);

    for (size_t i = 0; i < chunks.size();) {
        Chunk& chunk = chunks[i];
        chunk.velocity += gravity * dtSeconds;
        chunk.position += chunk.velocity * dtSeconds;
        chunk.angle += chunk.angularSpeed * dtSeconds;
        chunk.age += dtSeconds;

        if (chunk.age >= chunk.lifetime) {
            chunks[i] = chunks.back();
            chunks.pop_back();
            continue;
        }
        ++i;
    }

    instanceMatrices.clear();
    instanceColors.clear();
    instanceMatrices.reserve(chunks.size());
    instanceColors.reserve(chunks.size());

    for (const Chunk& chunk : chunks) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, chunk.position);
        model = glm::rotate(model, chunk.angle, chunk.axis);
        model = glm::scale(model, glm::vec3(chunk.size));

        instanceMatrices.push_back(model);
        instanceColors.push_back(chunk.color);
    }

    instanceCount = instanceMatrices.size();
    UploadInstances();
}

void DebrisField::Draw() {
    if (vao == 0 || instanceCount == 0) {
        return;
    }
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0,
        static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

void DebrisField::SpawnExplosion(const glm::vec3& center, int count, float minSpeed, float maxSpeed,
    float minSize, float maxSize) {
    if (count <= 0) {
        return;
    }

    for (int i = 0; i < count; ++i) {
        if (chunks.size() >= maxChunks) {
            chunks.erase(chunks.begin());
        }

        glm::vec3 dir = RandomUnitVector();
        float speed = RandomRange(minSpeed, maxSpeed);
        float size = RandomRange(minSize, maxSize);

        Chunk chunk;
        chunk.position = center + dir * RandomRange(0.0f, minSize);
        chunk.velocity = dir * speed + glm::vec3(0.0f, 0.0f, speed * 0.25f);
        chunk.axis = RandomUnitVector();
        chunk.angularSpeed = RandomRange(-4.0f, 4.0f);
        chunk.angle = RandomRange(0.0f, 6.28318f);
        chunk.lifetime = RandomRange(1.2f, 3.5f);
        chunk.age = 0.0f;
        chunk.size = size;
        float tint = RandomRange(0.85f, 1.15f);
        chunk.color = glm::vec4(0.34f * tint, 0.29f * tint, 0.22f * tint, 1.0f);

        chunks.push_back(chunk);
    }
}

void DebrisField::UploadInstances() {
    if (instanceCount == 0) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::vec4), instanceColors.data());

    glBindBuffer(GL_ARRAY_BUFFER, vboMat);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceCount * sizeof(glm::mat4), instanceMatrices.data());
}

glm::vec3 DebrisField::RandomUnitVector() {
    for (int i = 0; i < 8; ++i) {
        glm::vec3 v(
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f)
        );
        float len = glm::length(v);
        if (len > 0.001f) {
            return v / len;
        }
    }
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

float DebrisField::RandomRange(float minValue, float maxValue) {
    std::uniform_real_distribution<float> dist(minValue, maxValue);
    return dist(rng);
}
