#include "luz.h"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "modelo.h"
#include "shader.h"


void Luz::moveTo(glm::vec3 position, glm::vec3 look_at) {
    lightPos = position;
    lookAt = look_at;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPos, look_at, glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrix = lightProjection * lightView;
    invalidarDepthmap();
}


void Luz::beginDraw(Modelo& modelo, Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), modelo.position));
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    modelo.draw(shader);
}

void Luz::addDraw(Modelo& modelo, Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), modelo.position));
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    modelo.draw(shader);
}

void Luz::endDraw(Modelo& modelo, Shader& shader) {
    shader.use();
    shader.setMat4("model", glm::translate(glm::mat4(1.0f), modelo.position));
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

    modelo.draw(shader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Luz::draw(Modelo& modelo, Shader& shader, bool force) {
    if (force || !depthmap_actualizado) {
        shader.use();
        shader.setMat4("model", glm::translate(glm::mat4(1.0f), modelo.position));
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        modelo.draw(shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        depthmap_actualizado = true;
    }
}

void Luz::setupLight() {
    // Configura el FBO de la textura de profundidad
    glGenFramebuffers(1, &depthMapFBO);
    // Crea la textura de profundidad
    glGenTextures(1, &depthMapId);
    glBindTexture(GL_TEXTURE_2D, depthMapId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // Junta la textura de profundidad al buffer de profunidad del FBO
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

