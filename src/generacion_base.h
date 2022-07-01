#pragma once

#include "vertice.h"

#include <vector>
#include <glm/glm.hpp>

class GeneracionBase {
  private:
    // Genera una mesh de triangulos equivalentes
    static void generateRegularGrid(std::vector<Vertice>& vertices, int xl, int zl, double spacing);
    // Genera los indices necesarios para la grid regular
    static void generateIndices(std::vector<vert_index_t>& indices, int xl, int zl);
  public:
    // Genera un plano con triangulos a espacios regulares
    static void generarPlano(std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices, int xl, int zl, double spacing);
    // Genera una semiesfera mediante la proyección de un plano a la esfera
    static void generarSemiesferaProyectada(std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices, int xl, int zl, double spacing, glm::vec3 centro, double r);
};
