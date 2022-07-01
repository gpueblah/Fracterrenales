#pragma once

#include <cmath>

struct RuntimeConfig {
    // Global state
    bool show_imgui = false;
    bool show_stats_popup_imgui = false;

    bool generate_extrasimple_plane = false;
    bool generate_simple_plane = false;
    bool generate_slope = false;
    double spacing = 2;
    int p = 10;
    double h = 0.005;
    double h2 = std::pow(2, -h);

    float light_pos[3] = {5.0f, 3.0f, 1.0f};
    bool mouse_captured = true;

    float light_color[3] = {1.0f, 1.0f, 1.0f};

    int pcf = 4;

    float clear_color[3] = {0.27f, 0.6f, 0.98f};
    //{0.1f, 0.1f, 0.1f}; // { 0.27f, 0.6f, 0.98f } { 0.53f, 0.8f, 0.98f };

    float level_one_colors[3] = {0.02f, 0.04f, 0.65f};
    float level_two_colors[3] = {0.08f, 0.64f, 0.15f};
    float level_three_colors[3] = {0.54f, 0.27f, 0.07f};
    float level_four_colors[3] = {1.0f, 1.0f, 1.0f};

    int tipo_niebla = 2;
    float fog_max = 5.0f;
    float fog_min = 1.0f;
    float fog_density = 0.005f;

    float sea_level = 0.25f;

    float solid_color[3] = {0.3f, 0.7f, 0.24f};

    bool pasar_el_dia = false;
    float t_sol = 0.0f;

    bool walk_on_water = false;
    double walk_speed = 0.00000001;
    double walk_height = 0.1;

    double camera_speed = 1.0;

    bool vsync = true;
    float fovy = 45.0f;
    float near_plane = 0.01f;
    float far_plane = 1000.0f;

    float water_alpha = 0.9650f;
    float water_tiling = 128.0f;
    float water_speed = 0.001f;
    float water_strength = 0.1f;
    float water_color[3] = {72 / 255.0, 186 / 255.0, 1.0};

    float cloud_speed = 0.001;
    float clould_scale = 0.200f;
    float overcast = 0.07f;

    int polygon_mode = 2; // GL_POINT, GL_LINE, GL_FILL

    // Renders
    bool show_normals = false;
    bool normals_midpoint = false;
    bool show_wireframe = false;
    bool show_model = true;
    bool show_light = true;
    bool show_mutilayer_colors = true;
    bool show_color_de_pendiente = false;
    bool show_water = true;
    bool show_nubes = true;

    bool face_culling = true;

    bool debug_show_light_output = false;
    bool debug_show_position_triangle = false;
    bool debug_show_diamond_square_heightmap = false;
    bool debug_freeze_position_triangle = false;
    bool debug_attach_light_to_camera = false;

    // Actions
    bool wants_new_model = false;
    bool wants_screenshot = false;
    bool wants_heightmap = false;
};
