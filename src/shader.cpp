#include "shader.h"

Shader::Shader(const char* ruta_vertex, const char* ruta_fragment, const char* ruta_geometry) {
    std::string vertex_code;
    std::string fragment_code;
    std::string geometry_code;
    try {
        std::ifstream v_shader_file;
        std::ifstream f_shader_file;
        std::ifstream g_shader_file;
        // ensure ifstream objects can throw exceptions:
        v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        // open files
        v_shader_file.open(ruta_vertex);
        f_shader_file.open(ruta_fragment);
        std::stringstream v_shader_stream, f_shader_stream;
        // read file's buffer contents into streams
        v_shader_stream << v_shader_file.rdbuf();
        f_shader_stream << f_shader_file.rdbuf();
        // close file handlers
        v_shader_file.close();
        f_shader_file.close();
        // convert stream into string
        vertex_code = v_shader_stream.str();
        fragment_code = f_shader_stream.str();
        // if geometry shader path is present, also load a geometry shader
        if (ruta_geometry != nullptr) {
            tiene_geom = true;
            g_shader_file.open(ruta_geometry);
            std::stringstream g_shader_stream;
            g_shader_stream << g_shader_file.rdbuf();
            g_shader_file.close();
            geometry_code = g_shader_stream.str();
        }
    } catch (std::ifstream::failure& e) {
        std::cout << "EN: " << std::filesystem::current_path() << std::endl;
        std::cout << "MIENTRAS SE BUSCABA POR: " << ruta_fragment << std::endl;
        std::cout << "ERROR DE LECTURA: " << e.what() << std::endl;
    }
    const char* v_shader_code = vertex_code.c_str();
    const char* f_shader_code = fragment_code.c_str();
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_shader_code, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX", ruta_vertex);

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_shader_code, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", ruta_fragment);

    if (ruta_geometry != nullptr) {
        const char* g_shader_code = geometry_code.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &g_shader_code, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY", ruta_geometry);
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (ruta_geometry != nullptr) glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM", "Shader Program");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (ruta_geometry != nullptr) glDeleteShader(geometry);
}

Shader::Shader(Shader&& shader) noexcept {
    tiene_geom = shader.tiene_geom;
    vertex = shader.vertex;
    fragment = shader.fragment;
    geometry = shader.geometry;

    ID = shader.ID;

    shader.movido = true;
}

Shader::~Shader() {
    if (!movido) {
        glDetachShader(ID, vertex);
        glDetachShader(ID, fragment);
        if (tiene_geom) {
            glDetachShader(ID, geometry);
        }
        glDeleteProgram(ID);
    }
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const float value[3]) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value[0], value[1], value[2]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, std::string type, const char* path) {
    GLint success;
    GLchar info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR: " << type << '\n'
                      << path << '\n'
                      << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, info_log);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR: " << type << '\n'
                      << path << '\n'
                      << info_log << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
