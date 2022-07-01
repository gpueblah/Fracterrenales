#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using vert_index_t = unsigned int;

struct Vertice {
    glm::vec3 Position;

    glm::vec3 Normal;

    glm::vec2 TexCoords;
};
