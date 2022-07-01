#include "mesh.h"

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"

#include <iostream>
#include <string>
#include <vector>

Mesh::Mesh(std::vector<Vertice> vertices, std::vector<vert_index_t> indices, std::vector<Texture> textures) {
    // std::cout << "Mesh" << std::endl;
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

Mesh::Mesh(Mesh&& mesh) noexcept {
    vertices = std::move(mesh.vertices);
    indices = std::move(mesh.indices);
    textures = std::move(mesh.textures);
    VAO = mesh.VAO;
    VBO = mesh.VBO;
    EBO = mesh.EBO;

    xl = mesh.xl;
    zl = mesh.zl;

    mesh.moved_from = true;
}

Mesh& Mesh::operator=(Mesh&& mesh) noexcept {
    if (this != &mesh) {
        vertices = std::move(mesh.vertices);
        indices = std::move(mesh.indices);
        textures = std::move(mesh.textures);
        VAO = mesh.VAO;
        VBO = mesh.VBO;
        EBO = mesh.EBO;

        xl = mesh.xl;
        zl = mesh.zl;

        mesh.moved_from = true;
    }
    return *this;
}

Mesh::~Mesh() {
    if (!moved_from) {
        // std::cout << "~Mesh\n";
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void Mesh::draw(Shader& shader) {
    // Para cada textura, bindeamos su id al uniform
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        glUniform1i(glGetUniformLocation(shader.ID, textures[i].name.c_str()), i);

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // Dibuja el mesh con los indices necesarios
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<vert_index_t>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Vuelve a dejar la textura 0 como la activada, buena praxis
    glActiveTexture(GL_TEXTURE0);
}

std::vector<bwpixel_t> Mesh::generarHeightmap() {
    std::vector<bwpixel_t> pixeles;
    float min = std::numeric_limits<float>::infinity(), max = -std::numeric_limits<float>::infinity();

    pixeles.reserve(vertices.size());

    for (auto& vertice : vertices) {
        if (vertice.Position.y < min) min = vertice.Position.y;
        if (vertice.Position.y > max) max = vertice.Position.y;
    }

    for (auto& vertice : vertices) {
        pixeles.push_back(glm::smoothstep(min, max, vertice.Position.y) * 255.0f);
    }
    return pixeles;
}

void Mesh::refreshData() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertice), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(vert_index_t), indices.data(), GL_DYNAMIC_DRAW);

    setVertexAtributePointers();

    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    refreshData();
}

void Mesh::setVertexAtributePointers() {
    // Atributos de vertices
    // Posicion
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)0);
    // Normales
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, Normal));
    // Coordenadas de texturas
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertice), (void*)offsetof(Vertice, TexCoords));
}
