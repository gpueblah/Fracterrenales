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
    // Los sentidos en los que la c�mara se puede mover
    enum class Movimiento { FORWARD, BACKWARD, LEFT, RIGHT };

    // Posici�n de la c�mara en el mundo
    glm::vec3 Posicion;
    // Vector frontal de la c�mara (hacia donde mira)
    glm::vec3 Front;
    // Vector arriba de la c�mara (Front x Right)
    glm::vec3 Up;
    // Vector derecho de la c�mara
    glm::vec3 Right;
    // Vector global arriba del mundo (0, 1, 0)
    glm::vec3 WorldUp;
    // Rotaci�n sobre el eje up
    double Yaw;
    // Cabeceo
    double Pitch;
    // Velocidad de movimiento general de la c�mara
    double VelocidadMovimiento;
    // Multiplicador de velocidad de la c�mara (Para casos como "esprintar")
    double ModificadorVelocidad = 1.0;
    // Multiplicador de sensibilidad de la vista de rat�n
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

    // Fuerza a la c�mara a mirar a un punto en especifico en coordenadas globales
    void lookAt(glm::vec3 center);

    // Procesa las entradas por teclado. Acepta la direcci�n cardinal de movimiento
    void processKeyboard(Camara::Movimiento direction, double delta_time);

    // Asegura que los valores de Pitch est�n en [-90, 90]
    void constrainPitch();

    // Procesa la entrada de rat�n, como funci�n del desplazamiento del rat�n con respecto a la �ltima llamada
    void processMouseMovement(double xoffset, double yoffset, GLboolean constrain_pitch = true);

  private:
    // Calcula los vectores de la c�mara a partir de los angulos de Euler
    void updateCameraVectors();
};
