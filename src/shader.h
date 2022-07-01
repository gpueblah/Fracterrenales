#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
    bool movido = false;
    bool tiene_geom = false;
    unsigned int vertex, fragment, geometry;

  public:
    unsigned int ID;

    Shader(const char* ruta_vertex, const char* ruta_fragment, const char* ruta_geometry = nullptr);

    Shader(const Shader& mesh) = delete;
    Shader& operator=(const Shader& shader) = delete;

    Shader(Shader&& shader) noexcept;

    Shader& operator=(Shader&& shader) noexcept {
        if (this != &shader) {
            tiene_geom = shader.tiene_geom;
            vertex = shader.vertex;
            fragment = shader.fragment;
            geometry = shader.geometry;

            ID = shader.ID;

            shader.movido = true;
        }
        return *this;
    }

    ~Shader();

    void use();

    void setBool(const std::string& name, bool value) const;

    void setInt(const std::string& name, int value) const;

    void setFloat(const std::string& name, float value) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;

    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec3(const std::string& name, const float value[3]) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

  private:
    void checkCompileErrors(GLuint shader, std::string type, const char* path);
};
