#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "vertice.h"

using pixel_t = std::array<std::uint8_t, 4>;
using bwpixel_t = std::uint8_t;

class Utils {
  public:
    // Escribe una imagen de colores en formato ppm
    static void writePpm(std::string path, int width, int height, const std::vector<pixel_t>& pixels);
    // Escribe una imagen en escala de grises en formato ppm
    static void writePpm(std::string path, int width, int height, const std::vector<bwpixel_t>& pixels);
    // Genera un nombre de archivo de fotografia único en base a la hora actual
    static std::string generatePictureFilename();
    // Proyecta un plano en una esfera, resultando en una semiesfera
    static void proyectarPlanoEnEsfera(std::vector<Vertice>& plano, glm::vec3 c, double r);
    // Cambia la orientación de los triangulos de un mesh
    static void flipTriangulos(std::vector<vert_index_t>& indices);
    // Calcula los 4 cuartiles de un conjunto de valores
    static float calcularCuartiles(std::vector<float> valores);
};
