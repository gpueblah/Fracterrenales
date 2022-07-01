#include "generacion_base.h"

#include <random>

#include "utils.h"


void GeneracionBase::generateRegularGrid(std::vector<Vertice>& vertices, int xl, int zl, double spacing) {
    vertices.reserve(xl * zl);
    for (auto z = 0; z < zl; ++z) {
        for (auto x = 0; x < xl; ++x) {
            vertices.emplace_back(glm::vec3 {x * spacing, 0.0f, z * spacing},
                                  glm::vec3 {0.0f, 1.0f, 0.0f},
                                  glm::vec2 {x / static_cast<float>(xl), z / static_cast<float>(zl)});
        }
    }
}

void GeneracionBase::generateIndices(std::vector<vert_index_t>& indices, int xl, int zl) {
    indices.reserve(xl * xl * 6);
    for (auto fila = 0; fila < zl - 1; ++fila) {
        for (auto columna = 0; columna < xl - 1; ++columna) {
            // Triangulo superior
            indices.push_back(xl * fila + columna);
            indices.push_back(xl * fila + columna + 1);
            indices.push_back(xl * (fila + 1) + columna + 1);

            // Triangulo inferior
            indices.push_back(xl * fila + columna);
            indices.push_back(xl * (fila + 1) + columna + 1);
            indices.push_back(xl * (fila + 1) + columna);
        }
    }
}

void GeneracionBase::generarPlano(std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices, int xl, int zl, double spacing) {
    generateRegularGrid(vertices, xl, zl, spacing);
    generateIndices(indices, xl, zl);
}

void GeneracionBase::generarSemiesferaProyectada(
std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices, int xl, int zl, double spacing, glm::vec3 centro, double r) {
    GeneracionBase::generarPlano(vertices, indices, xl, zl, spacing);
    Utils::proyectarPlanoEnEsfera(vertices, centro, r);
}
