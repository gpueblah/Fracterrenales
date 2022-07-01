#include "camara.h"

glm::mat4 Camara::getViewMatrix() {
    return glm::lookAt(Posicion, Posicion + Front, Up);
}

void Camara::lookAt(glm::vec3 center) {
    Front = glm::normalize(center - Posicion);
    Right = glm::normalize(glm::cross(
    Front,
    WorldUp)); 
    // Tenemos que normalizar los vectores
    // Sus longitudes se acercan a 0 cuanto más miras arriba/abajo
    // lo que genera movimiento más lento
    Up = glm::normalize(glm::cross(Right, Front));

    Pitch = glm::degrees(glm::asin(Front.y));
    Yaw = glm::degrees(glm::asin(Front.z / glm::cos(glm::radians(Pitch))));
}

void Camara::processKeyboard(Camara::Movimiento direction, double delta_time) {
    float velocity = VelocidadMovimiento * delta_time * ModificadorVelocidad;
    if (direction == Camara::Movimiento::FORWARD) Posicion += Front * velocity;
    if (direction == Camara::Movimiento::BACKWARD) Posicion -= Front * velocity;
    if (direction == Camara::Movimiento::LEFT) Posicion -= Right * velocity;
    if (direction == Camara::Movimiento::RIGHT) Posicion += Right * velocity;
}

void Camara::constrainPitch() {
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;
}

void Camara::processMouseMovement(double xoffset, double yoffset, GLboolean constrain_pitch) {
    xoffset *= SensibilidadRaton;
    yoffset *= SensibilidadRaton;

    Yaw += xoffset;
    Pitch += yoffset;

    // Evitamos que la cámara se invierta si damos la vuelta completa
    if (constrain_pitch) {
        constrainPitch();
    }

    updateCameraVectors();
}

void Camara::updateCameraVectors() {
    glm::vec3 front(cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)), sin(glm::radians(Pitch)), sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)));
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
