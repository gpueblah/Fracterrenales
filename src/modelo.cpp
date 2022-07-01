#include "modelo.h"

void Modelo::draw(Shader& shader) {
    for (auto i = 0; i < meshes.size(); i++) meshes[i].draw(shader);
}