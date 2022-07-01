#include "render.h"

#include "imgui/imgui.h"

#include "utils.h"

#include <iostream>
#include <numeric>

#include <filesystem>
#include <vector>

GLFWwindow* Render::inicializarGlfw() {
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) {
        std::cerr << "glfwInit() error, terminando\n";
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // TODO: Investigar un toggle para fullscreen
    // https://www.glfw.org/docs/3.3/window_guide.html#window_full_screen

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "TFG", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 80, 80);

    // VSYNC
    glfwSwapInterval(config.vsync ? 1 : 0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}

void Render::toggleMouseCapture(GLFWwindow* window, bool forced_state) {
    glfwSetInputMode(window, GLFW_CURSOR, forced_state ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    if (forced_state) {
        first_mouse = true;
    }
    config.mouse_captured = forced_state;
}

void Render::drawImguiMenu() {
    if (config.show_stats_popup_imgui) {
        ImGui::OpenPopup("StatsPopup");
        ImGui::SetNextWindowPos({0, 0});
        drawStatsPopup();
    }

    if (config.show_imgui) {
        ImGui::SetNextWindowSize({0.0f, 0.0f});
        ImGui::Begin("Configuracion");

        drawStatsMenu();
        // if (model != nullptr) {
        //     ImGui::Text("Extremos de alturas: (%.4f, %.4f)", model->meshes[0].min_height, model->meshes[0].max_height);
        // }

        drawMiscMenu();

        drawAguaMenu();

        drawColoresMenu();

        drawLucesMenu();

        drawNubesMenu();

        drawNieblaMenu();

        // drawNormalesMenu();

        drawDebugMenu();

        drawGeneracionMenu();

        // ImGui::Separator();

        config.wants_screenshot = ImGui::Button("Captura de pantalla");
        ImGui::SameLine();
        config.wants_heightmap = ImGui::Button("Guardar mapa de alturas");

        ImGui::End();
    }
}

void Render::drawStatsPopup() {
    // Se come las entradas al resto de los menus
    // if (ImGui::BeginPopup("StatsPopup")) {
    //     ImGui::Text("Media de %.3f ms/frame (%.1f FPS)", info.averageDeltaTime * 1000.0f, 1.f / info.averageDeltaTime);
    //     ImGui::Text("%d vertices, %d indices (%d triangulos)", info.vertexCount, info.indexCount, info.indexCount / 3);
    //     ImGui::EndPopup();
    // }
}

void Render::drawStatsMenu() {
    if (ImGui::CollapsingHeader("Stats")) {
        ImGui::PlotLines(
        "ms/frame", [](void* data, int idx) { return static_cast<float*>(data)[idx]; }, info.lastDeltaTimes.data(), info.lastDeltaTimes.size());
        ImGui::BeginDisabled();
        ImGui::Checkbox("Show stats popup", &config.show_stats_popup_imgui);
        ImGui::EndDisabled();
        ImGui::Text("Media de %.3f ms/frame (%.1f FPS)", info.averageDeltaTime * 1000.0f, 1.f / info.averageDeltaTime);
        ImGui::Text("%d vertices, %d indices (%d triangulos)", info.vertexCount, info.indexCount, info.indexCount / 3);
    }
}

void Render::drawMiscMenu() {
    if (ImGui::CollapsingHeader("Misc")) {
        ImGui::Checkbox("Mostrar modelo", &config.show_model);
        if (!config.show_model) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Andar", &config.walk_on_water);

        if (!config.walk_on_water) {
            ImGui::BeginDisabled();
        }
        ImGui::InputDouble("Velocidad andar", &config.walk_speed, 0.0, 0.0, "%.9f");
        ImGui::InputDouble("Altura andar", &config.walk_height, 0.0, 0.0, "%.4f");
        if (!config.walk_on_water) {
            ImGui::EndDisabled();
        }

        if (!config.show_model) {
            ImGui::EndDisabled();
        }
        ImGui::Checkbox("Wireframe", &config.show_wireframe);
        constexpr std::array<const char*, 3> modos_poligonos {"GL_POINT", "GL_LINE", "GL_FILL"};
        ImGui::Combo("Polygon mode", &config.polygon_mode, modos_poligonos.data(), modos_poligonos.size());

        ImGui::Checkbox("Face culling", &config.face_culling);

        ImGui::InputDouble("Camera speed", &config.camera_speed);
        camara.VelocidadMovimiento = config.camera_speed;
        ImGui::Checkbox("VSync", &config.vsync);
        ImGui::InputFloat("fovyº", &config.fovy);
        if (config.fovy <= 0.0f)
            config.fovy = 0.0001f;
        else if (config.fovy >= 180.0f)
            config.fovy = 179.99f;
        ImGui::InputFloat("Near plane", &config.near_plane);
        if (config.near_plane < 0.000001f) config.near_plane = 0.000001f;
        if (config.near_plane >= config.far_plane) config.near_plane = config.far_plane * 0.9999f;
        ImGui::InputFloat("Far plane", &config.far_plane);
        if (config.far_plane < 0.000001f) config.near_plane = 0.000001f;
        if (config.far_plane <= config.near_plane) config.far_plane = config.near_plane * 1.00001f;
    }
}

void Render::drawAguaMenu() {
    if (ImGui::CollapsingHeader("Agua")) {
        ImGui::Checkbox("Render Agua", &config.show_water);

        ImGui::DragFloat("Alpha agua", &config.water_alpha, 0.01, 0.0f, 1.0f);
        ImGui::DragFloat("Tiling agua", &config.water_tiling, 0.01f, 0, 256.0f);
        ImGui::DragFloat("Speed agua", &config.water_speed, 0.001f, 0.0f, 10.0f);
        ImGui::DragFloat("Strength agua", &config.water_strength, 0.001f, 0.0f, 10.0f);

        ImGui::SetNextItemWidth(120.0f);
        ImGui::ColorPicker3("Color agua", config.water_color);
    }
}

void Render::drawColoresMenu() {
    if (ImGui::CollapsingHeader("Colores")) {
        if (config.show_light) {
            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("Light color", config.light_color);
        }
        ImGui::Separator();
        ImGui::Checkbox("Mostrar colores en multiples capas", &config.show_mutilayer_colors);
        ImGui::Separator();

        if (config.show_mutilayer_colors) {
            // ImGui::SameLine();

            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("N4 color", config.level_four_colors);

            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("N3 color", config.level_three_colors);

            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("N2 color", config.level_two_colors);

            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("N1 color", config.level_one_colors);

            ImGui::DragFloat("Nivel del mar", &config.sea_level, 0.005f, 0.0f, 1.0f, "%.4f", 0);

        } else {
            ImGui::Checkbox("Mostrar pendiente como color", &config.show_color_de_pendiente);
            if (config.show_color_de_pendiente) {
                ImGui::BeginDisabled();
            }
            ImGui::SetNextItemWidth(120.0f);
            ImGui::ColorPicker3("Color solido", config.solid_color);
            if (config.show_color_de_pendiente) {
                ImGui::EndDisabled();
            }
        }
        ImGui::Separator();
        ImGui::SetNextItemWidth(120.0f);
        ImGui::ColorPicker3("Clear color", config.clear_color);
    }
}

void Render::drawLucesMenu() {
    if (ImGui::CollapsingHeader("Luces")) {
        ImGui::Checkbox("Mostrar luces", &config.show_light);
        ImGui::Checkbox("Pasar el dia", &config.pasar_el_dia);
        if (config.pasar_el_dia) {
            // ImGui::SameLine();
            auto sin2t = std::pow(std::sin(config.t_sol), 2);
            ImGui::Text("Hora: %.3f", sin2t);
            ImGui::Text("Color: %.3f", glm::mix(0.05f, 0.99f, sin2t));
        }
        if (!config.show_light || config.pasar_el_dia) {
            ImGui::BeginDisabled();
        }

        ImGui::DragFloat3("Posicion de luz", config.light_pos, 0.5f);
        if (!config.show_light || config.pasar_el_dia) {
            ImGui::EndDisabled();
        }

        ImGui::DragInt("PCF", &config.pcf, 1, 0, 16);
    }
}

void Render::drawNubesMenu() {
    if (ImGui::CollapsingHeader("Nubes")) {
        ImGui::Checkbox("Draw nubes", &config.show_nubes);
        ImGui::DragFloat("Nubes speed", &config.cloud_speed, 0.0001f, 0.0f, 50.0f);
        ImGui::DragFloat("Nubes scale", &config.clould_scale, 0.001f, 0.0f, 100.0f);
        ImGui::DragFloat("Overcast", &config.overcast, 0.01f, 0.0f, 1.0f);
    }
}

void Render::drawNieblaMenu() {
    if (!config.show_light) {
        ImGui::BeginDisabled();
    }
    if (ImGui::CollapsingHeader("Niebla")) {
        constexpr std::array<const char*, 4> tipos_niebla {"Ninguna", "Lineal", "Exp", "Exp2"};
        ImGui::Combo("Tipo niebla", &config.tipo_niebla, tipos_niebla.data(), tipos_niebla.size());

        switch (config.tipo_niebla) {
            case 1: {
                ImGui::DragFloat("FogMin", &config.fog_min, 0.25f, 0.0f, config.fog_max);
                ImGui::DragFloat("FogMax", &config.fog_max, 0.25f, config.fog_min, config.far_plane);
                break;
            }
            case 2:
            case 3: {
                ImGui::DragFloat("Densidad niebla", &config.fog_density, 0.001f, 0.0f, 2.0f);
                break;
            }
        }
    }
    if (!config.show_light) {
        ImGui::EndDisabled();
    }
}

void Render::drawNormalesMenu() {
    if (ImGui::CollapsingHeader("Normales")) {
        ImGui::Checkbox("Mostrar normales", &config.show_normals);
        if (!config.show_normals) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Normales en puntos medios", &config.normals_midpoint);
        if (!config.show_normals) {
            ImGui::EndDisabled();
        }
    }
}

void Render::drawDebugMenu() {
    if (ImGui::CollapsingHeader("Debug")) {
        if (!config.show_model) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Show position triangle", &config.debug_show_position_triangle);
        if (!config.show_model) {
            ImGui::EndDisabled();
        }

        if (!config.debug_show_position_triangle) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Freeze position triangle", &config.debug_freeze_position_triangle);
        if (!config.debug_show_position_triangle) {
            ImGui::EndDisabled();
        }

        if (config.debug_show_diamond_square_heightmap) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Show light debug", &config.debug_show_light_output);
        if (config.debug_show_diamond_square_heightmap) {
            ImGui::EndDisabled();
        }

        if (config.debug_show_light_output) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Show diamond square heightmap debug", &config.debug_show_diamond_square_heightmap);
        if (config.debug_show_light_output) {
            ImGui::EndDisabled();
        }

        ImGui::Checkbox("Attach light to camera", &config.debug_attach_light_to_camera);
        drawNormalesMenu();
    }
}

void Render::drawGeneracionMenu() {
    if (ImGui::CollapsingHeader("Generacion")) {
        ImGui::InputInt("p", &config.p, 1, 3);
        if (config.p < 1)
            config.p = 1;
        else if (config.p > 14)
            config.p = 14;

        auto old_h = config.h;
        ImGui::InputDouble("h", &config.h, 0.0f, 0.0f, "%.7f");
        if (old_h != config.h) {
            config.h2 = std::pow(2, -config.h);
        }

        ImGui::InputDouble("Spacing", &config.spacing);

        ImGui::Checkbox("Crear plano extra simple", &config.generate_extrasimple_plane);
        if (config.generate_extrasimple_plane) {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Crear plano simple", &config.generate_simple_plane);
        if (config.generate_extrasimple_plane) {
            ImGui::EndDisabled();
        }

        ImGui::Checkbox("Generate slope", &config.generate_slope);
        config.wants_new_model = ImGui::Button("Regenerar nuevo plano");
    }
}

void Render::tomarCaptura(int x, int y, int width, int height) {
    std::vector<pixel_t> pixels;
    pixels.resize(width * height);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    auto file_name = Utils::generatePictureFilename();
    Utils::writePpm(file_name.append(".ppm"), width, height, pixels);
}

void Render::capturarPantalla() {
    tomarCaptura(0, 0, screen_width, screen_height);
}

void Render::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F9 && action == GLFW_PRESS) {
        std::cout << "F9" << std::endl;
        tomarCaptura(0, 0, screen_width, screen_height);
    } else if (key == GLFW_KEY_G && action == GLFW_PRESS && mods & GLFW_MOD_ALT) {
        // tell GLFW to capture our mouse
        toggleMouseCapture(window);
    } else if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
        config.show_imgui = !config.show_imgui;
        toggleMouseCapture(window, !config.show_imgui);
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Render::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
    auto is_shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    camara.ModificadorVelocidad = is_shift ? 2.0f : 1.0f;
    auto is_alt = glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS;
    camara.ModificadorVelocidad = is_alt ? 0.1f : camara.ModificadorVelocidad;

    auto old_pos = camara.Posicion;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camara.processKeyboard(Camara::Movimiento::FORWARD, info.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camara.processKeyboard(Camara::Movimiento::BACKWARD, info.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camara.processKeyboard(Camara::Movimiento::LEFT, info.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camara.processKeyboard(Camara::Movimiento::RIGHT, info.deltaTime);

    if (old_pos != camara.Posicion) {
        // auto nueva_pos = camara.Posicion;
        //
        // auto mirada = light.getLookedAt();
        // auto offset = (light.getFarPlane() - light.getNearPlane()) / 2;
        // nueva_pos.y = 0.0f;
        // auto v = glm::normalize(nueva_pos - mirada) * offset;
        // nueva_pos.y = 10.0f;
        // light.moveTo(nueva_pos + v);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void Render::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    if (width != 0 && height != 0) {
        screen_width = width;
        screen_height = height;
        // make sure the viewport matches the new window dimensions; note that width
        // and height will be significantly larger than specified on retina
        // displays.
        glViewport(0, 0, width, height);
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Render::mouseCallback(GLFWwindow* window, double xpos_in, double ypos_in) {
    if (config.mouse_captured) {
        double xpos = xpos_in;
        double ypos = ypos_in;
        if (first_mouse) {
            last_x = xpos;
            last_y = ypos;
            first_mouse = false;
        }

        double xoffset = xpos - last_x;
        double yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

        last_x = xpos;
        last_y = ypos;

        camara.processMouseMovement(xoffset, yoffset);
    }
}

void Render::actualizarRenderInfo() {
    double current_frame = glfwGetTime();
    info.deltaTime = current_frame - info.lastFrame;
    info.lastDeltaTimes[info.currentIndexDeltaTime] = info.deltaTime;
    info.currentIndexDeltaTime++;
    if (info.currentIndexDeltaTime % info.lastDeltaTimes.size() == 0) {
        info.currentIndexDeltaTime = 0;
    }
    info.averageDeltaTime = std::accumulate(info.lastDeltaTimes.begin(), info.lastDeltaTimes.end(), 0.0) / info.lastDeltaTimes.size();
    info.lastFrame = current_frame;
}

void Render::glfwErrorCallback(int error, const char* description) {
    std::cerr << "Glfw Error " << error << ": " << description << '\n';
}