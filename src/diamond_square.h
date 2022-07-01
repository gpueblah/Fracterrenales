#pragma once

#include "modelo.h"
#include "render_info.h"
#include "runtime_config.h"

#include <memory>
#include <vector>

class DiamondSquare {
    int p = 0;
    int tamaño = 0;
    double h2 = 0;
    double spacing = 0.1;
    double persistencia = 0.5;

    std::unique_ptr<Modelo> modelo;

    double min_height, max_height;
    double q1;

    RuntimeConfig& config;
    RenderInfo& info;

    void crearModeloPlano();
    void aplicarElevacion(std::vector<Vertice>& vertices);
    void arreglaNormales(std::vector<Vertice>& vertices, const std::vector<vert_index_t>& indices, int l);

    void construirPlano(std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices);

  public:
    DiamondSquare(RuntimeConfig& runtime_config, RenderInfo& render_info) :
        p {runtime_config.p}, config {runtime_config}, info {render_info} {
        this->h2 = config.h2;
        tamaño = std::pow(2, p) + 1;
        crearModeloPlano();
    }

    int getP() { return p; }
    float getQ1() { return q1; }
    int getTam() { return tamaño; }
    double getSpacing() { return spacing; }
    double getCentro() { return getTam() * getSpacing() / 2; }
    Modelo* getModelo() { return modelo.get(); }
    double getMinHeight() { return min_height; }
    double getMaxHeight() { return max_height; }

    Texture mapaDeAlturas;

    std::optional<std::tuple<Vertice, Vertice, Vertice>> getVertexUnderCamera(glm::vec3 cameraPosition);

    void regenerar();
};
