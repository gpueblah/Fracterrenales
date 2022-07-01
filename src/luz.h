#pragma once


#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "modelo.h"
#include "shader.h"

class Luz {
    unsigned int depthMapFBO;
    unsigned int depthMapId;

    float near_plane = 0.1f, far_plane = 100.0f;

    glm::vec3 lightPos = {0.0f, 0.0f, 0.0f};
    glm::vec3 lookAt = {0.0f, 0.0f, 0.0f};

    glm::highp_mat4 lightProjection;
    glm::highp_mat4 lightView;
    glm::highp_mat4 lightSpaceMatrix;

    bool depthmap_actualizado = false;

  public:
    unsigned int getDepthMapFBO() { return depthMapFBO; }
    unsigned int getDepthMapId() { return depthMapId; }

    glm::vec3 getLightPos() { return lightPos; }
    glm::vec3 getLookedAt() { return lookAt; }

    float getNearPlane() { return near_plane; }
    float getFarPlane() { return far_plane; }

    glm::highp_mat4 getLightSpaceMatrix() { return lightSpaceMatrix; }

    glm::highp_mat4 getLightProjection() { return lightProjection; }

    glm::highp_mat4 getLightView() { return lightView; }

    void invalidarDepthmap() { depthmap_actualizado = false; }

    unsigned int width = 1024 * 8;
    unsigned int height = 1024 * 8;

    explicit Luz(glm::vec3 position) { moveTo(position); }

    Luz() { moveTo(glm::vec3(0.0f)); }

    // Mueve la luz a la posición y con la inclinación indicadas, invalidando su mapa de profundidad
    void moveTo(glm::vec3 position, glm::vec3 look_at = glm::vec3(0.0f));

    // Si se necesita dibujar más de 1 elemento para la luz, iniciar con una llamada a esta
    void beginDraw(Modelo& modelo, Shader& shader);

    // Añade un elemento a dibujar al stack de luces. Debe de antes haberse llamado a beginDraw()
    void addDraw(Modelo& modelo, Shader& shader);

    // Finaliza el proceso de dibujado de los modelos de luces.
    void endDraw(Modelo& modelo, Shader& shader);

    // Dibuja un único modelo para esta liz
    void draw(Modelo& modelo, Shader& shader, bool force = false);

    // Configura el estado de la luz
    void setupLight();
};
