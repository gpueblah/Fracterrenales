#pragma once

#include <glm/glm.hpp>

#include "mesh.h"
#include "shader.h"

class Modelo {
  public:
    std::vector<Mesh> meshes = {};
    glm::vec3 position = {};

    explicit Modelo(std::vector<Mesh>&& initial_meshes) : position {glm::vec3(0.0f)} { meshes = std::move(initial_meshes); }

    Modelo() = default;

    ~Modelo() = default;

    Modelo(const Modelo& modelo) = delete;
    Modelo& operator=(const Modelo& modelo) = delete;

    Modelo(Modelo&& modelo) = default;
    Modelo& operator=(Modelo&& modelo) = default;

    void draw(Shader& shader);
};
