// Color Space es experimental, necesitamos activarla
#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

#include "imgui/imgui_impl_glfw.h"

#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>

#include "diamond_square.h"
#include "generacion_base.h"
#include "luz.h"
#include "render.h"
#include "ruidos.h"
#include "shader.h"
#include "texture.h"
#include "utils.h"

// Para tomar fotos, estamos usando:
Render render(1027, 728, glm::vec3(-1.5f, 2.0f, -1.5f));

//Render render(1700, 920, glm::vec3(-1.5f, 2.0f, -1.5f));

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    render.framebufferSizeCallback(window, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    render.mouseCallback(window, xpos, ypos);
}

void processInput(GLFWwindow* window) {
    render.processInput(window);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    render.keyCallback(window, key, scancode, action, mods);
}

int main() {
    GLFWwindow* window = render.inicializarGlfw();

    if (window == nullptr) {
        return -1;
    }

    auto version = glGetString(GL_VERSION);

    std::cout << "OpenGL version: " << version << '\n';

    // Los callbacks necesitan ser estaticos
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);

    render.toggleMouseCapture(window, true);

    // Estados globales de OpenGL iniciales
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    if (render.config.face_culling) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }

    glLineWidth(2.0f);

    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glPointSize(4.0f);

    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Luz
    render.light.setupLight();

    // Shaders

    // color_solido
    Shader plano_color_solido_shader("shaders\\color_solido.vert", "shaders\\color_solido.frag");
    // plano, color_solido
    Shader wireframe_shader("shaders\\color_solido.vert", "shaders\\color_solido.frag");
    // visualizacion
    Shader visualizacion_normales_shader(
    "shaders\\normales\\visualizacion.vert", "shaders\\normales\\visualizacion.frag", "shaders\\normales\\visualizacion.geom");
    // mapa_sombras_profundidad
    Shader mapa_profunidad_shader("shaders\\luces\\mapa_sombras_profundidad.vert", "shaders\\luces\\mapa_sombras_profundidad.frag");
    // mapa_sombras
    Shader plano_iluminado_shader("shaders\\luces\\plano_iluminado.vert", "shaders\\luces\\plano_iluminado.frag");
    // debug_luz
    Shader debug_luz_shader("shaders\\luces\\debug_luz.vert", "shaders\\luces\\debug_luz.frag");
    // debug_heightmap
    Shader debug_heightmap_shader("shaders\\debug_heightmap.vert", "shaders\\debug_heightmap.frag");
    // agua
    Shader agua_shader("shaders\\agua\\agua.vert", "shaders\\agua\\agua.frag");
    // cielo
    Shader cielo_shader("shaders\\cielo\\cielo.vert", "shaders\\cielo\\cielo.frag");


    // Modelos
    auto diamond_square = DiamondSquare(render.config, render.info);
    auto centro = diamond_square.getCentro();

    Modelo agua;
    auto textura_agua = Texture::fromFile("texturas\\agua\\water.png", "agua");
    // auto textura_flow = Texture::fromFile("texturas\\agua\\flow.png", "flow");
    auto texutra_flow_con_reset = Texture::fromFile("texturas\\agua\\resetnoise.png", "flow");
    auto textura_normal_agua = Texture::fromFile("texturas\\agua\\water-normal.png", "normal");
    agua.meshes.emplace_back(
    std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {textura_agua, texutra_flow_con_reset, textura_normal_agua});

    GeneracionBase::generarPlano(
    agua.meshes[0].vertices, agua.meshes[0].indices, diamond_square.getTam(), diamond_square.getTam(), diamond_square.getSpacing());
    agua.meshes[0].refreshData();

    Modelo cielo;
    auto cielo_size = 200;
    auto cielo_spacing = 0.1;
    auto cielo_centro_radio = cielo_size * cielo_spacing / 2;
    auto cielo_centro = glm::vec3(cielo_centro_radio, -1, cielo_centro_radio);
    auto cielo_radio = cielo_centro_radio * 10;
    auto cielo_data = Ruidos::generarDiamondSquare(10, std::pow(2, -0.01), Ruidos::UNIFORME);
    Texture textura_cielo = Texture::fromData(cielo_data, 1024, 1024, "cielo");
    // Texture textura_cielo = Texture::fromFile("texturas\\grumos.png", "cielo");
    cielo.meshes.emplace_back(std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {textura_cielo});
    GeneracionBase::generarSemiesferaProyectada(cielo.meshes[0].vertices,
                                                cielo.meshes[0].indices,
                                                cielo_size,
                                                cielo_size,
                                                cielo_spacing, 
                                                cielo_centro,
                                                cielo_radio);
    Utils::flipTriangulos(cielo.meshes[0].indices);
    cielo.meshes[0].refreshData();
    cielo.position = glm::vec3(0, -5, 0);


    //auto q1 = glm::mix(diamond_square.getMinHeight(), diamond_square.getMaxHeight(), 0.25);
    auto q1 = diamond_square.getQ1();
    agua.position.y = q1;
 
    render.camara.Posicion.x = centro;
    render.camara.Posicion.z = centro;


    {
        render.light.moveTo({centro, 10, centro}, {centro * 3 / 4, 0, centro * 3 / 4});
        auto mirada = render.light.getLookedAt();
        auto offset = (render.light.getFarPlane() - render.light.getNearPlane()) / 2;
        auto c = glm::vec3(centro, 0, centro);
        auto v = glm::normalize(c - mirada) * offset;
        c.y = 10.0f;
        render.light.moveTo(c + v, {centro * 3 / 4, 0, centro * 3 / 4});

        render.config.light_pos[0] = render.light.getLightPos().x;
        render.config.light_pos[1] = render.light.getLightPos().y;
        render.config.light_pos[2] = render.light.getLightPos().z;
    }

    Mesh debug_position_mesh(
    {
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    },
    {0, 1, 3, 1, 2, 3, 2, 0, 3},
    {});

    /*
    (-1,  1)    (1,  1)

    (-1, -1)    (1, -1)
    */
    Mesh debug_light_quad_mesh(
    {
    { {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {  {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    { {1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    },
    // Strip para los pobres
    // 0 1 2 3
    {0, 2, 1, 1, 2, 3},
    {});

    // render.config.light_pos[0] = diamond_square.getTam() * render.config.spacing;
    // // p / 10 para el maximo rango, / 2 para el centro -> / 20
    // render.config.light_pos[1] = diamond_square.getModelo()->meshes[0].max_height * 4;
    // render.config.light_pos[2] = diamond_square.getTam() * render.config.spacing;

    // Posición media para p = 5. Siempre asi para tener las fotos tomadas desde la misma perspectiva
    render.camara.lookAt({16.0f, -8.0f, 16.0f});

    auto atlas_ruido_uniforme = Texture::fromData(Ruidos::generarRuidoUniforme(diamond_square.getTam(), diamond_square.getTam(), 0.0f, 1.0f),
                                                  diamond_square.getTam(),
                                                  diamond_square.getTam(),
                                                  "ruido");

    plano_iluminado_shader.use();
    plano_iluminado_shader.setInt("diffuseTexture", 0);
    plano_iluminado_shader.setInt("shadowMap", 1);
    // lightPlaneShader.setInt("noiseMap", 2);

    debug_luz_shader.use();
    debug_luz_shader.setInt("depthMap", 0);

    debug_heightmap_shader.use();
    debug_heightmap_shader.setInt("altura", 0);

    unsigned int depthMapFBO, depthMapId, backgroundFBO, backgroundId;
    // configure depth map FBO
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    glGenTextures(1, &depthMapId);
    glBindTexture(GL_TEXTURE_2D, depthMapId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, render.screen_width, render.screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color[] = {1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // configure background map FBO
    glGenFramebuffers(1, &backgroundFBO);

    glGenTextures(1, &backgroundId);
    glBindTexture(GL_TEXTURE_2D, backgroundId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, render.screen_width, render.screen_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float border_color2[] = {1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color2);

    // attach texture as FBO's buffer
    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_RGB, GL_TEXTURE_2D, backgroundId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Mesh testMesh = Mesh(planeModel.meshes[0].vertices,
    // planeModel.meshes[0].indices, {}); render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        render.actualizarRenderInfo();
        glfwPollEvents();
        // input
        // -----
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool old_culling = render.config.face_culling;
        bool old_vsync = render.config.vsync;
        render.drawImguiMenu();
        if (old_culling != render.config.face_culling) {
            if (render.config.face_culling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
        if (old_vsync != render.config.vsync) {
            glfwSwapInterval(render.config.vsync ? 1 : 0);
        }

        // PRE-DRAW WANTS
        if (render.config.wants_new_model) {
            render.config.wants_new_model = false;
            render.light.invalidarDepthmap();

            diamond_square.regenerar();
            centro = diamond_square.getCentro();

            agua.meshes.clear();
            agua.meshes.emplace_back(
            std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {textura_agua, texutra_flow_con_reset});
            auto& agua_mesh = agua.meshes[0];
            GeneracionBase::generarPlano(
            agua_mesh.vertices, agua_mesh.indices, diamond_square.getTam(), diamond_square.getTam(), diamond_square.getSpacing());
            //auto q1 = glm::mix(diamond_square.getMinHeight(), diamond_square.getMaxHeight(), 0.25);
            q1 = diamond_square.getQ1();
            agua_mesh.refreshData();
            agua.position.y = q1;

            // NO hay necesidad de regenerar el cielo, porque está pensado para quedar bien a cualquier tamaño

            // cielo.meshes.clear();
            // 
            // cielo_size = diamond_square.getTam();
            // cielo_spacing = diamond_square.getSpacing(); 
            // cielo.meshes.emplace_back(std::vector<Vertice> {}, std::vector<vert_index_t> {}, std::vector<Texture> {textura_cielo});
            // GeneracionBase::generarSemiesferaProyectada(
            // cielo.meshes[0].vertices, cielo.meshes[0].indices, cielo_size, cielo_size, cielo_spacing, glm::vec3(centro, -1.0, centro), centro * 2);
            // Utils::flipTriangulos(cielo.meshes[0].indices);
            // cielo.meshes[0].refreshData();
        }

        // render
        // ------
        glClearColor(render.config.clear_color[0], render.config.clear_color[1], render.config.clear_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = render.getProjection();
        glm::mat4 view = render.camara.getViewMatrix();
        // dibujar_eje_coordenadas(ejesCoordenads, projection, view);

        if (render.config.show_model) {
            constexpr std::array polygon_modes {GL_POINT, GL_LINE, GL_FILL};
            glPolygonMode(GL_FRONT_AND_BACK, polygon_modes[render.config.polygon_mode]);
            if (render.config.show_light) {
                if (render.config.debug_show_light_output) {
                    render.light.invalidarDepthmap();
                }

                auto new_pos = glm::vec3(render.config.light_pos[0], render.config.light_pos[1], render.config.light_pos[2]);
                if (new_pos != render.light.getLightPos()) {
                    render.light.moveTo(new_pos);
                }

                render.light.beginDraw(agua, mapa_profunidad_shader);
                render.light.endDraw(*diamond_square.getModelo(), mapa_profunidad_shader);

                glViewport(0, 0, render.screen_width, render.screen_height);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                if (render.config.debug_show_light_output) {
                    // No podemos dibujar el FBO de la luz como color usando blit porque lo copia al buffer de profunidad, no del color.
                    // Hay que hacerlo a la vieja usanza
                    // glBindFramebuffer(GL_READ_FRAMEBUFFER, light.getDepthMapFBO());
                    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    // glBlitFramebuffer(0, 0, light.width, light.height, 0, 0, render.screen_width, render.screen_height, GL_DEPTH_BUFFER_BIT,
                    // GL_NEAREST); glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    debug_luz_shader.use();
                    debug_luz_shader.setFloat("near_plane", render.config.near_plane);
                    debug_luz_shader.setFloat("far_plane", render.config.far_plane);
                    debug_luz_shader.setFloat("is_linear", false);
                    glActiveTexture(GL_TEXTURE0);
                    // glBindTexture(GL_TEXTURE_2D, textura_cielo.id);
                    glBindTexture(GL_TEXTURE_2D, render.light.getDepthMapId());
                    // glBindTexture(GL_TEXTURE_2D, diamond_square.mapaDeAlturas.id);
                    debug_light_quad_mesh.draw(debug_luz_shader);
                } else if (render.config.debug_show_diamond_square_heightmap) {
                    debug_heightmap_shader.use();
                    debug_heightmap_shader.setFloat("min_value", diamond_square.getMinHeight());
                    debug_heightmap_shader.setFloat("max_value", diamond_square.getMaxHeight());
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, diamond_square.mapaDeAlturas.id);
                    debug_light_quad_mesh.draw(debug_heightmap_shader);
                } else {
                    // set light uniforms
                    plano_iluminado_shader.use();
                    plano_iluminado_shader.setMat4("projection", projection);
                    plano_iluminado_shader.setMat4("view", view);
                    plano_iluminado_shader.setMat4("model", glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position));

                    plano_iluminado_shader.setVec3("viewPos", render.camara.Posicion);
                    plano_iluminado_shader.setVec3("lightPos", render.light.getLightPos());
                    plano_iluminado_shader.setMat4("lightSpaceMatrix", render.light.getLightSpaceMatrix());

                    plano_iluminado_shader.setInt("tipoNiebla", render.config.tipo_niebla);
                    plano_iluminado_shader.setVec3("FogColor", render.config.clear_color);
                    plano_iluminado_shader.setFloat("FogMax", render.config.fog_max);
                    plano_iluminado_shader.setFloat("FogMin", render.config.fog_min);
                    plano_iluminado_shader.setFloat("FogDensity", render.config.fog_density);

                    plano_iluminado_shader.setFloat("minHeight", diamond_square.getMinHeight());
                    plano_iluminado_shader.setFloat("maxHeight", diamond_square.getMaxHeight());

                    plano_iluminado_shader.setVec3("lightColor", render.config.light_color);
                    plano_iluminado_shader.setBool("iluminar", true);

                    plano_iluminado_shader.setVec3("l1Color", render.config.level_one_colors);
                    plano_iluminado_shader.setVec3("l2Color", render.config.level_two_colors);
                    plano_iluminado_shader.setVec3("l3Color", render.config.level_three_colors);
                    plano_iluminado_shader.setVec3("l4Color", render.config.level_four_colors);

                    plano_iluminado_shader.setFloat("q1", diamond_square.getQ1());

                    plano_iluminado_shader.setInt("pcf", render.config.pcf);

                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, render.light.getDepthMapId());

                    diamond_square.getModelo()->draw(plano_iluminado_shader);

                    if (render.config.show_nubes) {
                        cielo.position = glm::vec3(render.camara.Posicion.x - cielo_centro_radio, -7.5, render.camara.Posicion.z - cielo_centro_radio);
                        cielo_shader.use();

                        cielo_shader.setFloat("time", render.info.lastFrame);
                        cielo_shader.setFloat("velocity", render.config.cloud_speed);
                        cielo_shader.setFloat("scale", render.config.clould_scale);

                        cielo_shader.setFloat("overcast", render.config.overcast);
                        cielo_shader.setVec3("offset", cielo_centro - render.camara.Posicion);

                        cielo_shader.setMat4("projection", projection);
                        cielo_shader.setMat4("view", view);
                        cielo_shader.setMat4("model", glm::translate(glm::mat4(1.0f), cielo.position));

                        cielo_shader.setVec3("viewPos", render.camara.Posicion);
                        cielo_shader.setInt("tipoNiebla", render.config.tipo_niebla);
                        cielo_shader.setVec3("FogColor", render.config.clear_color);
                        cielo_shader.setFloat("FogMax", render.config.fog_max);
                        cielo_shader.setFloat("FogMin", render.config.fog_min);
                        cielo_shader.setFloat("FogDensity", render.config.fog_density);

                        cielo.draw(cielo_shader);
                    }

                    if (render.config.show_water) {
                        // AGUA
                        agua_shader.use();
                        agua_shader.setFloat("time", static_cast<float>(render.info.lastFrame));

                        agua_shader.setMat4("projection", projection);
                        agua_shader.setMat4("view", view);
                        agua_shader.setMat4("model", glm::translate(glm::mat4(1.0f), agua.position));

                        agua_shader.setFloat("alpha", render.config.water_alpha);

                        agua_shader.setFloat("near_plane", render.config.near_plane);
                        agua_shader.setFloat("far_plane", render.config.far_plane);

                        agua_shader.setVec3("color", render.config.water_color);
                        agua_shader.setFloat("tiling", render.config.water_tiling);
                        agua_shader.setFloat("speed", render.config.water_speed);
                        agua_shader.setFloat("strength", render.config.water_strength);

                        agua_shader.setBool("iluminar", true);

                        agua_shader.setVec3("lightPos", render.light.getLightPos());
                        agua_shader.setVec3("lightColor", render.config.light_color);

                        agua_shader.setVec3("viewPos", render.camara.Posicion);
                        agua_shader.setInt("tipoNiebla", render.config.tipo_niebla);
                        agua_shader.setVec3("FogColor", render.config.clear_color);
                        agua_shader.setFloat("FogMax", render.config.fog_max);
                        agua_shader.setFloat("FogMin", render.config.fog_min);
                        agua_shader.setFloat("FogDensity", render.config.fog_density);

                        agua.draw(agua_shader);
                    }
                }
            } else {
                if (render.config.show_mutilayer_colors) {

                    plano_iluminado_shader.use();
                    plano_iluminado_shader.setMat4("projection", projection);
                    plano_iluminado_shader.setMat4("view", view);
                    plano_iluminado_shader.setMat4("model", glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position));

                    plano_iluminado_shader.setVec3("viewPos", render.camara.Posicion);
                    plano_iluminado_shader.setVec3("lightPos", render.light.getLightPos());
                    plano_iluminado_shader.setMat4("lightSpaceMatrix", render.light.getLightSpaceMatrix());

                    plano_iluminado_shader.setInt("tipoNiebla", render.config.tipo_niebla);
                    plano_iluminado_shader.setVec3("FogColor", render.config.clear_color);
                    plano_iluminado_shader.setFloat("FogMax", render.config.fog_max);
                    plano_iluminado_shader.setFloat("FogMin", render.config.fog_min);
                    plano_iluminado_shader.setFloat("FogDensity", render.config.fog_density);

                    plano_iluminado_shader.setFloat("minHeight", diamond_square.getMinHeight());
                    plano_iluminado_shader.setFloat("maxHeight", diamond_square.getMaxHeight());

                    plano_iluminado_shader.setVec3("lightColor", render.config.light_color);
                    plano_iluminado_shader.setBool("iluminar", false);

                    plano_iluminado_shader.setVec3("l1Color", render.config.level_one_colors);
                    plano_iluminado_shader.setVec3("l2Color", render.config.level_two_colors);
                    plano_iluminado_shader.setVec3("l3Color", render.config.level_three_colors);
                    plano_iluminado_shader.setVec3("l4Color", render.config.level_four_colors);

                    plano_iluminado_shader.setFloat("q1", diamond_square.getQ1());

                    diamond_square.getModelo()->draw(plano_iluminado_shader);

                    if (render.config.show_nubes) {
                        cielo_shader.use();

                        cielo_shader.setFloat("time", render.info.lastFrame);
                        cielo_shader.setFloat("velocity", render.config.cloud_speed);
                        cielo_shader.setFloat("scale", render.config.clould_scale);

                        cielo_shader.setFloat("overcast", render.config.overcast);

                        cielo_shader.setMat4("projection", projection);
                        cielo_shader.setMat4("view", view);
                        cielo_shader.setMat4("model", glm::translate(glm::mat4(1.0f), cielo.position));

                        cielo_shader.setVec3("viewPos", render.camara.Posicion);
                        cielo_shader.setInt("tipoNiebla", render.config.tipo_niebla);
                        cielo_shader.setVec3("FogColor", render.config.clear_color);
                        cielo_shader.setFloat("FogMax", render.config.fog_max);
                        cielo_shader.setFloat("FogMin", render.config.fog_min);
                        cielo_shader.setFloat("FogDensity", render.config.fog_density);

                        cielo.draw(cielo_shader);
                    }
                    if (render.config.show_water) {
                        // AGUA
                        agua_shader.use();

                        agua_shader.setFloat("time", static_cast<float>(render.info.lastFrame));

                        agua_shader.setMat4("projection", projection);
                        agua_shader.setMat4("view", view);
                        agua_shader.setMat4("model", glm::translate(glm::mat4(1.0f), agua.position));

                        agua_shader.setFloat("alpha", render.config.water_alpha);

                        agua_shader.setFloat("near_plane", render.config.near_plane);
                        agua_shader.setFloat("far_plane", render.config.far_plane);
                        agua_shader.setBool("iluminar", false);

                        agua_shader.setVec3("color", render.config.water_color);
                        agua_shader.setFloat("tiling", render.config.water_tiling);
                        agua_shader.setFloat("speed", render.config.water_speed);
                        agua_shader.setFloat("strength", render.config.water_strength);

                        agua.draw(agua_shader);
                    }
                } else {
                    plano_color_solido_shader.use();

                    plano_color_solido_shader.setBool("colorDePendiente", render.config.show_color_de_pendiente);
                    plano_color_solido_shader.setVec3("color", render.config.solid_color);

                    plano_color_solido_shader.setMat4("projection", projection);
                    plano_color_solido_shader.setMat4("view", view);
                    plano_color_solido_shader.setMat4("model", glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position));

                    glViewport(0, 0, render.screen_width, render.screen_height);
                    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    diamond_square.getModelo()->draw(plano_color_solido_shader);

                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    debug_luz_shader.use();
                    debug_luz_shader.setFloat("near_plane", render.config.near_plane);
                    debug_luz_shader.setFloat("far_plane", render.config.far_plane);
                    debug_luz_shader.setFloat("is_linear", true);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, depthMapId);
                    debug_light_quad_mesh.draw(debug_luz_shader);

                    // diamond_square.getModelo()->draw(plano_color_solido_shader);
                }
            }

            if (render.config.debug_show_position_triangle) {
                auto vertices = diamond_square.getVertexUnderCamera(render.camara.Posicion);
                if (vertices.has_value()) {
                    if (!render.config.debug_freeze_position_triangle) {
                        const auto& [va, vb, vc] = vertices.value();
                        debug_position_mesh.vertices[0] = va;
                        debug_position_mesh.vertices[1] = vb;
                        debug_position_mesh.vertices[2] = vc;

                        debug_position_mesh.vertices[3].Position = render.camara.Posicion;

                        // auto areaGrande = Utils::areaTriangulo(va.Position, vb.Position, vc.Position);
                        //
                        // auto areaABD = Utils::areaTriangulo(va.Position, vb.Position, render.camara.Posicion);
                        // auto areaBCD = Utils::areaTriangulo(vb.Position, vc.Position, render.camara.Posicion);
                        // auto areaCAD = Utils::areaTriangulo(vc.Position, va.Position, render.camara.Posicion);
                        //
                        // auto pesoA = areaBCD / areaGrande;
                        // auto pesoB = areaCAD / areaGrande;
                        // auto pesoC = areaABD / areaGrande;
                        //
                        // auto altura = (pesoA * va.Position.y + pesoB * vb.Position.y + pesoC * vc.Position.y) / 3;
                        auto a = va.Position;

                        auto ac = (vc.Position - a);
                        auto ab = (vb.Position - a);
                        auto normal = normalize(glm::cross(ab, ac));
                        auto d = render.camara.Posicion;

                        debug_position_mesh.vertices[3].Position.y =
                        render.config.walk_height + a.y - ((d.x - a.x) * normal.x + ((d.z - a.z) * normal.z)) / normal.y;

                        debug_position_mesh.refreshData();
                    }
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    wireframe_shader.use();

                    wireframe_shader.setVec3("color", glm::vec3(1.0f, 0.0f, 1.0f));
                    wireframe_shader.setMat4("projection", projection);
                    wireframe_shader.setMat4("view", view);
                    wireframe_shader.setMat4("model",
                                             glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position + glm::vec3(0.0f, 0.003f, 0.0f)));

                    debug_position_mesh.draw(wireframe_shader);
                }
            }

            if (render.config.walk_on_water) {
                auto vertices = diamond_square.getVertexUnderCamera(render.camara.Posicion);
                if (vertices.has_value()) {
                    const auto& [va, vb, vc] = vertices.value();

                    auto a = va.Position;

                    auto ac = (vc.Position - a);
                    auto ab = (vb.Position - a);
                    auto normal = normalize(glm::cross(ab, ac));
                    auto d = render.camara.Posicion;
                    // p = A * ac + B * ab;
                    render.camara.Posicion.y = render.config.walk_height + a.y - ((d.x - a.x) * normal.x + ((d.z - a.z) * normal.z)) / normal.y;

                    render.camara.ModificadorVelocidad = render.config.walk_speed;
                }
            } else {
                render.camara.ModificadorVelocidad = 1.0f;
            }
        }

        if (render.config.show_normals) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            visualizacion_normales_shader.use();

            visualizacion_normales_shader.setBool("showMidpoint", render.config.normals_midpoint);
            visualizacion_normales_shader.setMat4("projection", projection);
            visualizacion_normales_shader.setMat4("view", view);
            visualizacion_normales_shader.setMat4("model", glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position));
            diamond_square.getModelo()->draw(visualizacion_normales_shader);
        }

        if (render.config.show_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            wireframe_shader.use();

            wireframe_shader.setFloat("time", render.info.lastFrame);

            wireframe_shader.setFloat("minHeight", diamond_square.getMinHeight());
            wireframe_shader.setFloat("maxHeight", diamond_square.getMaxHeight());
            wireframe_shader.setFloat("seaLevel", render.config.sea_level);

            wireframe_shader.setVec3("color", glm::vec3(1.0f));
            wireframe_shader.setMat4("projection", projection);
            wireframe_shader.setMat4("view", view);
            wireframe_shader.setMat4("model", glm::translate(glm::mat4(1.0f), diamond_square.getModelo()->position + glm::vec3(0.0f, 0.0003f, 0.0f)));

            diamond_square.getModelo()->draw(wireframe_shader);
        }

        // POST-DRAW WANTS
        if (render.config.wants_screenshot) {
            render.config.wants_screenshot = false;
            render.capturarPantalla();
        }

        if (render.config.wants_heightmap) {
            render.config.wants_heightmap = false;
            if (diamond_square.getModelo() != nullptr) {
                auto filename = Utils::generatePictureFilename();
                for (auto i = 0; i < diamond_square.getModelo()->meshes.size(); ++i) {
                    auto pixeles = diamond_square.getModelo()->meshes[i].generarHeightmap();
                    int xl = diamond_square.getModelo()->meshes[i].xl;
                    int zl = diamond_square.getModelo()->meshes[i].zl;
                    Utils::writePpm(filename.append("-heightmap-").append(std::to_string(i)).append(".ppm"), xl, zl, pixeles);
                }
            }
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
