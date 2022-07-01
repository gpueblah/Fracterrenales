#pragma once
#include <string>
#include <vector>

class Texture {
    // Texture() = default;
  public:
    std::string name;
    unsigned int id;
    int width;
    int height;

    static Texture fromFile(const char* path, std::string name);
    static Texture fromData(std::vector<float> data, int xl, int zl, std::string name);
};