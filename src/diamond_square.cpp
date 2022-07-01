#include "diamond_square.h"

#include "generacion_base.h"
#include "ruidos.h"
#include "utils.h"
#include <random>


void DiamondSquare::arreglaNormales(std::vector<Vertice>& vertices, const std::vector<vert_index_t>& indices, int l) {
    std::vector<int> pesos;
    pesos.reserve(indices.size());
    for (auto i = 0; i < indices.size(); i++) {
        pesos.push_back(0);
    }
    for (auto i = 0; i < indices.size(); i += 3) {
        auto ai = indices[i];
        auto bi = indices[i + 1];
        auto ci = indices[i + 2];
        auto& a = vertices[ai];
        auto& b = vertices[bi];
        auto& c = vertices[ci];

        auto ab = b.Position - a.Position;
        auto ac = c.Position - a.Position;
        auto n = glm::normalize(glm::cross(ac, ab));
        // u' = (T * u + x) / (T + 1)
        // Multiplicación por elementos https://godbolt.org/z/bsqae5YWo
        a.Normal = (a.Normal * glm::vec3(pesos[ai]) + n) / (glm::vec3(pesos[ai] + 1));
        pesos[ai]++;

        b.Normal = (b.Normal * glm::vec3(pesos[bi]) + n) / (glm::vec3(pesos[bi] + 1));
        pesos[bi]++;

        c.Normal = (c.Normal * glm::vec3(pesos[ci]) + n) / (glm::vec3(pesos[ci] + 1));
        pesos[ci]++;
    }
}

std::pair<float, float> calcularExtremosAltura(const std::vector<Vertice>& vertices) {
    float min = std::numeric_limits<float>::infinity(), max = -std::numeric_limits<float>::infinity();

    for (auto& vertice : vertices) {
        if (vertice.Position.y < min) min = vertice.Position.y;
        if (vertice.Position.y > max) max = vertice.Position.y;
    }


    return std::make_pair(min, max);
}

void DiamondSquare::construirPlano(std::vector<Vertice>& vertices, std::vector<vert_index_t>& indices) {
    p = config.p;
    tamaño = static_cast<int>(glm::pow(2, p)) + 1;
    spacing = config.spacing;

    GeneracionBase::generarPlano(vertices, indices, tamaño, tamaño, spacing);

    info.vertexCount = vertices.size();
    info.indexCount = indices.size();
}

void DiamondSquare::aplicarElevacion(std::vector<Vertice>& vertices) {
    this->h2 = config.h2;
    auto elevaciones = Ruidos::generarDiamondSquare(p, this->h2, Ruidos::NORMAL);
    
    for (auto i = 0; i < elevaciones.size(); ++i) {
        vertices[i].Position.y += elevaciones[i];
    }
    mapaDeAlturas = Texture::fromData(elevaciones, getTam(), getTam(), "alturas");
}

void DiamondSquare::crearModeloPlano() {
    modelo = std::make_unique<Modelo>();

    modelo->meshes.emplace_back(std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {});
    auto& mesh = modelo->meshes.back();

    construirPlano(mesh.vertices, mesh.indices);

    aplicarElevacion(mesh.vertices);
    
    auto extremos = calcularExtremosAltura(mesh.vertices);
    
    std::vector<float> f;
    f.reserve(mesh.vertices.size());

    for (auto& vertice : mesh.vertices) {
        f.push_back(vertice.Position.y);
    }

    q1 = Utils::calcularCuartiles(f);

    arreglaNormales(mesh.vertices, mesh.indices, tamaño);

    mesh.xl = tamaño;
    mesh.zl = tamaño;

    min_height = extremos.first;
    max_height = extremos.second;

    mesh.refreshData();

    modelo->position = glm::vec3(0.0f, 0.0f, 0.0f);
}

void DiamondSquare::regenerar() {
    // Si los cambios en la config con los que hay que cambiar la forma del mesh, reconstruimso el plano 
    if (p != config.p || spacing != config.spacing) {
        modelo->meshes.clear();
        modelo->meshes.emplace_back(std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {});
        auto& mesh = modelo->meshes.back();
        // Construir plano actualiza los valores cambiados
        construirPlano(mesh.vertices, mesh.indices);
    }
    for (auto& mesh : modelo->meshes) {
        for (auto i = 0; i < mesh.vertices.size(); ++i) {
            mesh.vertices[i].Position.y = 0.0f;
        }
        aplicarElevacion(mesh.vertices);

        auto extremos = calcularExtremosAltura(mesh.vertices);
        std::vector<float> f;
        f.reserve(mesh.vertices.size());

        for (auto& vertice : mesh.vertices) {
            f.push_back(vertice.Position.y);
        }

        q1 = Utils::calcularCuartiles(f);
        arreglaNormales(mesh.vertices, mesh.indices, getTam());

        min_height = extremos.first;
        max_height = extremos.second;

        mesh.refreshData();
    }
}

// Si existe, devuelve la tupla de 3 vertices debajo de la camara
std::optional<std::tuple<Vertice, Vertice, Vertice>> DiamondSquare::getVertexUnderCamera(glm::vec3 camPos) {
    auto dx = getTam() * spacing;
    if (getModelo() != nullptr && !getModelo()->meshes.empty() && camPos.x >= 0.0f && camPos.x <= dx && camPos.z >= 0.0f && camPos.z <= dx) {
        auto& mesh = getModelo()->meshes[0];
        auto& vertices = mesh.vertices;
        auto& indices = mesh.indices;

        auto celdaX = camPos.x / spacing;
        auto celdaZ = camPos.z / spacing;

        auto cuadrado = (getTam() - 1) * int(celdaZ) + int(celdaX);

        auto esZgrX = glm::fract(celdaZ) > glm::fract(celdaX);

        auto triangulo = cuadrado * 2 + (esZgrX ? 1 : 0);
        auto indiceInicial = triangulo * 3;
        if (indiceInicial <= indices.size() - 3) {
            auto originalA = vertices[indices[indiceInicial]];
            auto originalB = vertices[indices[indiceInicial + 1]];
            auto originalC = vertices[indices[indiceInicial + 2]];

            return std::make_optional(std::make_tuple(originalA, originalB, originalC));
        }
    }
    return std::nullopt;
}
