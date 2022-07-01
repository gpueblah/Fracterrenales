#pragma once
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Valores por defecto de la camara
const float DEFAULT_yaw = -90.0f;
const float DEFAULT_pitch = 0.0f;
const float DEFAULT_velocidad = 2.5f;
const float DEFAULT_sensibilidad = 0.1f;

// Camara que procesa los eventos de entrada y los traduce a angulos Euler para trabajar en OpenGL
class Camara {
  public:
    // Los sentidos en los que la cámara se puede mover
    enum class Movimiento { FORWARD, BACKWARD, LEFT, RIGHT };

    // Posición de la cámara en el mundo
    glm::vec3 Posicion;
    // Vector frontal de la cámara (hacia donde mira)
    glm::vec3 Front;
    // Vector arriba de la cámara (Front x Right)
    glm::vec3 Up;
    // Vector derecho de la cámara
    glm::vec3 Right;
    // Vector global arriba del mundo (0, 1, 0)
    glm::vec3 WorldUp;
    // Rotación sobre el eje up
    double Yaw;
    // Cabeceo
    double Pitch;
    // Velocidad de movimiento general de la cámara
    double VelocidadMovimiento;
    // Multiplicador de velocidad de la cámara (Para casos como "esprintar")
    double ModificadorVelocidad = 1.0;
    // Multiplicador de sensibilidad de la vista de ratón
    double SensibilidadRaton;

    Camara(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = DEFAULT_yaw,
           float pitch = DEFAULT_pitch) :
        Front(glm::vec3(1.0f, 0.0f, 0.0f)),
        VelocidadMovimiento(DEFAULT_velocidad), SensibilidadRaton(DEFAULT_sensibilidad) {
        Posicion = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Devuelve la matriz de vista a partir de los angulos Euler y WorldUp
    glm::mat4 getViewMatrix();

    // Fuerza a la cámara a mirar a un punto en especifico en coordenadas globales
    void lookAt(glm::vec3 center);

    // Procesa las entradas por teclado. Acepta la dirección cardinal de movimiento
    void processKeyboard(Camara::Movimiento direction, double delta_time);

    // Asegura que los valores de Pitch estén en [-90, 90]
    void constrainPitch();

    // Procesa la entrada de ratón, como función del desplazamiento del ratón con respecto a la última llamada
    void processMouseMovement(double xoffset, double yoffset, GLboolean constrain_pitch = true);

  private:
    // Calcula los vectores de la cámara a partir de los angulos de Euler
    void updateCameraVectors();
};
