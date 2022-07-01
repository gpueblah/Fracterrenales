#pragma once

#include "shader.h"
#include "texture.h"
#include "vertice.h"

using bwpixel_t = std::uint8_t;

class Mesh {
    bool moved_from = false;

    // Vertex Array Object (Metadatos de los vertices, atributos, etc.)
    unsigned int VAO;

    // Vertex Buffer Object (Buffer que contiene los vértices en si)
    unsigned int VBO;

    // Element Buffer Object (Buffer que contiene los indices del orden de vertices)
    unsigned int EBO;

    void setupMesh();

    void setVertexAtributePointers();

  public:
    // mesh Data
    std::vector<Vertice> vertices;
    std::vector<vert_index_t> indices;
    std::vector<Texture> textures;

    int xl = 0;
    int zl = 0;

    Mesh(std::vector<Vertice> vertices, std::vector<vert_index_t> indices, std::vector<Texture> textures);

    Mesh(const Mesh& mesh) = delete;
    Mesh& operator=(const Mesh& mesh) = delete;

    Mesh(Mesh&& mesh) noexcept;

    Mesh& operator=(Mesh&& mesh) noexcept;

    ~Mesh();

    void draw(Shader& shader);

    std::vector<bwpixel_t> generarHeightmap();

    void refreshData();
};
