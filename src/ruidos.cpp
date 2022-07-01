#include "ruidos.h"

#include "generacion_base.h"

#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


std::mt19937 diamondGen(0);
std::uniform_real_distribution<> diamondReal_distrib(-0.25f, 0.25f);

std::normal_distribution<> diamondNormal_distrib(0, 0.7);


std::vector<float> Ruidos::generarRuidoUniforme(int xl, int zl, float min, float max) {
    // Siempre a 0 para resultados consistentes. TODO: std:.random_device cuando sea necesario
    std::mt19937 gen(0);
    std::uniform_real_distribution<> real_distrib(min, max);
    auto ruido = std::vector<float>();
    ruido.reserve(xl * zl);
    for (auto z = 0; z < zl; ++z) {
        for (auto x = 0; x < xl; ++x) {
            float v = glm::mix(min, max, real_distrib(gen));
            ruido.push_back(v);
        }
    }
    return ruido;
}

// TODO: Pesos distintos por cada vertice
// Lewis, J. P. (1 July 1987). "Generalized stochastic subdivision"

/*
    Para un índice d, devuelve la suma de todos sus "diamantes" h vecinos, teniendo en cuenta que algunos pueden no existir
    El primer valor es el número de vecinos que existen, y el segundo es la suma de estos
*/
std::pair<int, float> sumatorioPesosVecinos(const std::vector<float>& vertices, int d, int longitud, int tamañoSubCuadrado) {
    /*
            h0

        h3  d   h1

            h2
    */

    float total = 0.0f;
    int usadas = 0;
    // Existe h0?
    if (d > longitud) {
        int h0 = d - longitud;
        total += vertices[h0];
        ++usadas;
    }

    // Existe h1?
    if (d % longitud != longitud - 1) {
        int h1 = d + (tamañoSubCuadrado / 2);
        total += vertices[h1];
        ++usadas;
    }

    // Existe h2?
    if (d < (longitud * (longitud - 1))) {
        int h2 = d + longitud;
        total += vertices[h2];
        ++usadas;
    }

    // Existe h3?
    if (d % longitud > 0) {
        int h3 = d - (tamañoSubCuadrado / 2);
        total += vertices[h3];
        ++usadas;
    }
    return std::make_pair(usadas, total);
}

double random(Ruidos::DiamondSquareDistrib distrib) {
    if (distrib == Ruidos::NORMAL) {
        return diamondNormal_distrib(diamondGen);
    } else if (distrib == Ruidos::UNIFORME) {
        return diamondReal_distrib(diamondGen);
    }
    return 0.0;
}

void diamanteCuadradoBien(
std::vector<float>& vertices, int iteracion, int actual, int longitud, double h2, Ruidos::DiamondSquareDistrib distrib) {
    if (iteracion > 0) {

        float pow2n = std::pow(2, actual);
        // La longitud del subcuadrado, que se reduce cada iteración en un factor de 2.
        int tamañoCuadrado = std::ceil(longitud / pow2n);

        // Número de subcuadrados para esta iteración en fila/columa
        // de forma que tenemos que pasar por delta * delta cuadrados en esta iteración
        int delta = std::floor(static_cast<float>(longitud) / (tamañoCuadrado - 1));

        // En lugar de reducir por 0.5, reducimos por un factor de (1/2^-h)^actual
       double reduccion = glm::pow(h2, actual + 1);

        for (int z = 0; z < delta; z++) {
            for (int x = 0; x < delta; ++x) {
                // Coordenadas del subcuadrado actual
                int sx = x * (tamañoCuadrado - 1);
                int sz = z * (tamañoCuadrado - 1);

                // Índice en el vector de alturas del primer valor del subcuadrado actual
                int s = sz * longitud + sx;
                /* Layout del orden de las esquinas del subcuadrado
                    s      1

                       c

                    2      3
                */
                int e0 = s;
                int e1 = s + (tamañoCuadrado - 1);
                int e2 = s + (tamañoCuadrado - 1) * longitud;
                int e3 = e2 + (tamañoCuadrado - 1);

                // Índice del centro del subcuadrado
                int c = s + (longitud + 1) * (tamañoCuadrado / 2);

                // nueva altura para centro, paso del cuadrado
                vertices[c] = (vertices[e0] + vertices[e1] + vertices[e2] + vertices[e3]) / 4 + random(distrib) * reduccion;
            }
        }

        for (int z = 0; z < delta; z++) {
            for (int x = 0; x < delta; ++x) {
                // Coordenadas del subcuadrado actual
                int sx = x * (tamañoCuadrado - 1);
                int sz = z * (tamañoCuadrado - 1);

                // Índice en el vector de alturas del primer valor del subcuadrado actual
                int s = sz * longitud + sx;
                /* Layout del orden de los "diamantes" del subcuadrado
                    s   0

                    1   c   2

                        3
                */
                int d0 = s + tamañoCuadrado / 2;
                int d1 = s + tamañoCuadrado / 2 * longitud;
                int d2 = d1 + (tamañoCuadrado - 1);
                int d3 = d0 + (tamañoCuadrado - 1) * longitud;
                

                /* Orden de las h para el caso de 0. EL resto siguen el mismo orden horario
                        h0

                    h3  0    h1

                    1   h2   2

                        3
                */

                // Nueva altura de cada diamante en base a sus centros más cercanos
                // El primer valor es el número de vecinos que existen, y el segundo es la suma de estos
                const auto v0 = sumatorioPesosVecinos(vertices, d0, longitud, tamañoCuadrado);
                const auto v1 = sumatorioPesosVecinos(vertices, d1, longitud, tamañoCuadrado);
                const auto v2 = sumatorioPesosVecinos(vertices, d2, longitud, tamañoCuadrado);
                const auto v3 = sumatorioPesosVecinos(vertices, d3, longitud, tamañoCuadrado);

                vertices[d0] = v0.second / v0.first + random(distrib) * reduccion;
                vertices[d1] = v1.second / v1.first + random(distrib) * reduccion;
                vertices[d2] = v2.second / v2.first + random(distrib) * reduccion;
                vertices[d3] = v3.second / v3.first + random(distrib) * reduccion;
            }
        }
        // Nos llamamos recursivamente, actualizando adecuadamente el numero de iteraciones restantes
        diamanteCuadradoBien(vertices, iteracion - 1, actual + 1, longitud, h2, distrib);
    }
}

// Generación de números aleatoria.
// Para asegurarnos resultados consistentes mientras desarrollamos, la semilla está fijada en 0



// p es el exponente que usamos para generar el plano, tal que su longitud sea 2^p + 1
// h2 es std::pow(2, -h), precalculado
std::vector<float> Ruidos::generarDiamondSquare(int p, double h2, DiamondSquareDistrib distrib) {
    std::vector<float> resultado;
    // Ancho del cuadrado
    int xl = static_cast<int>(glm::pow(2, p)) + 1;
    resultado.reserve(xl * xl);

    for (auto z = 0; z < xl; ++z) {
        for (auto x = 0; x < xl; ++x) {
            resultado.push_back(0.0f);
        }
    }
   
    // Las 4 esquinas tienen un valor aleatorio
    resultado[0] = diamondNormal_distrib(diamondGen) * 4;
    resultado[xl - 1] = diamondNormal_distrib(diamondGen) * 4;
    resultado[xl * (xl - 1)] = diamondNormal_distrib(diamondGen) * 4;
    resultado[xl * xl - 1] = diamondNormal_distrib(diamondGen) * 4;

    diamanteCuadradoBien(resultado, p, 0, xl, h2, distrib);
    return resultado;
}
