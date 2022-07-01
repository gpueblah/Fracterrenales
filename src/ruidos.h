#pragma once

#include <vector>

namespace Ruidos {
    std::vector<float> generarRuidoUniforme(int xl, int zl, float min, float max);

    enum DiamondSquareDistrib {
        UNIFORME,
        NORMAL
    };
    std::vector<float> generarDiamondSquare(int p, double h2, DiamondSquareDistrib distrib);
} // namespace Ruidos