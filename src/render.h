#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camara.h"
#include "luz.h"
#include "render_info.h"
#include "runtime_config.h"
#include "shader.h"

#include <string>

class Render {
    double last_x = 0.0, last_y = 0.0;
    bool first_mouse = true;

  public:
    int screen_width, screen_height;

    Camara camara;
    Luz light;

    RuntimeConfig config;
    RenderInfo info;

    Render(int width, int height, glm::vec3 camara_position = glm::vec3(0.0f)) :
        screen_width {width}, screen_height {height}, camara {camara_position} {
        last_x = screen_width / 2.0;
        last_y = screen_height / 2.0;
        light = Luz(camara_position);
    }

    GLFWwindow* inicializarGlfw();

    void actualizarRenderInfo();

    void toggleMouseCapture(GLFWwindow* window, bool forced_state);
    void toggleMouseCapture(GLFWwindow* window) { toggleMouseCapture(window, !config.mouse_captured); }

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void processInput(GLFWwindow* window);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void drawImguiMenu();

    void capturarPantalla();

    glm::highp_mat4 getProjection() {
        return glm::perspective(glm::radians(config.fovy), static_cast<float>(screen_width) / screen_height, config.near_plane, config.far_plane);
    }
    static void glfwErrorCallback(int error, const char* description);

  private:
    void tomarCaptura(int x, int y, int width, int height);

    void drawStatsPopup();
    void drawStatsMenu();
    void drawMiscMenu();
    void drawAguaMenu();
    void drawColoresMenu();
    void drawLucesMenu();
    void drawNubesMenu();
    void drawNieblaMenu();
    void drawNormalesMenu();
    void drawDebugMenu();
    void drawGeneracionMenu();
};
