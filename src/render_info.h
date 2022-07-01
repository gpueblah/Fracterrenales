#pragma once

#include <array>

struct RenderInfo {
    double lastFrame = 0.0;
    double deltaTime = 0.0;

    std::array<double, 20> lastDeltaTimes = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double averageDeltaTime = 0.0;
    int currentIndexDeltaTime = 0;

    int indexCount = 0;
    int vertexCount = 0;
};
