#include "utils.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>


void Utils::writePpm(std::string path, int width, int height, const std::vector<pixel_t>& pixels) {
    if (pixels.size() != width * height) {
        std::cout << "Dimensiones incorrectas" << std::endl;
        return;
    }
    std::ofstream out;
    out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        out.open(path, std::ios_base::out);
    } catch (std::ofstream::failure& ex) {
        std::cout << ex.what() << " " << ex.code() << std::endl;
    }
    if (!out || !out.is_open() || out.bad() || !out.good()) {
        std::cout << "Error de sistema de archivos" << std::endl;
    } else {
        out << "P3\n" << width << " " << height << '\n' << 255 << '\n';
        for (auto j = height - 1; j >= 0; --j) {
            for (auto i = 0; i < width; ++i) {
                auto& pixel = pixels[j * width + i];
                out << ' ' << static_cast<int>(pixel[0]) << ' ' << static_cast<int>(pixel[1]) << ' ' << static_cast<int>(pixel[2]);
            }
        }
        std::cout << "Archivo escrito satisfactoriamente\n";
    }
}

void Utils::writePpm(std::string path, int width, int height, const std::vector<bwpixel_t>& pixels) {
    if (pixels.size() != width * height) {
        std::cout << "Dimensiones incorrectas" << std::endl;
        return;
    }
    std::ofstream out;
    out.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    try {
        out.open(path, std::ios_base::out);
    } catch (std::ofstream::failure& ex) {
        std::cout << ex.what() << " " << ex.code() << std::endl;
    }
    if (!out || !out.is_open() || out.bad() || !out.good()) {
        std::cout << "Error de sistema de archivos" << std::endl;
    } else {
        out << "P2\n" << width << " " << height << '\n' << 255 << '\n';
        for (auto j = 0; j < height; ++j) {
            for (auto i = 0; i < width; ++i) {
                auto& pixel = pixels[j * width + i];
                out << ' ' << static_cast<int>(pixel);
            }
        }
        std::cout << "Archivo escrito satisfactoriamente\n";
    }
}

std::string Utils::generatePictureFilename() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream sstream;
    sstream << "outputs/" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d") << "/";

    std::filesystem::create_directories(sstream.str());

    sstream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d-%H-%M-%S");
    return sstream.str();
}

void Utils::proyectarPlanoEnEsfera(std::vector<Vertice>& plano, glm::vec3 c, double r) {
    for (auto& vertice : plano) {
        glm::vec3 v = glm::normalize(vertice.Position - c);
        auto newPos = c + v * glm::vec3(r);
        vertice.Position = newPos;
    }
}

void Utils::flipTriangulos(std::vector<vert_index_t>& indices) {
    for (std::size_t i = 0; i <= indices.size() - 3; i += 3) {
        auto temp = indices[i + 1];
        indices[i + 1] = indices[i + 2];
        indices[i + 2] = temp;
    }
}

float Utils::calcularCuartiles(std::vector<float> valores) {
    std::sort(valores.begin(), valores.end());
    return valores[valores.size() / 6];
}
