#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glad/glad.h>
#include <iostream>

Texture Texture::fromFile(const char* path, std::string name) {
    Texture result;
    result.name = name;
    int nrChannels;
    unsigned char* data = stbi_load(path, &result.width, &result.height, &nrChannels, 0);

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);

    auto type = nrChannels == 1 ? GL_RED : (nrChannels == 3 ? GL_RGB : GL_RGBA);

    glTexImage2D(GL_TEXTURE_2D, 0, type, result.width, result.height, 0, type, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float border_color[] = {0.0, 0.0, 0.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    stbi_image_free(data);

    return result;
}

Texture Texture::fromData(std::vector<float> data, int xl, int zl, std::string name) {
    Texture result;
    result.name = name;
    result.width = xl;
    result.height = zl;

    glGenTextures(1, &result.id);
    glBindTexture(GL_TEXTURE_2D, result.id);

    auto type = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, type, result.width, result.height, 0, type, GL_FLOAT, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    float border_color[] = {0.0, 0.0, 0.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    return result;
}
