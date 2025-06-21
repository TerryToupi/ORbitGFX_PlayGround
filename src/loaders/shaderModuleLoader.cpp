#include <loaders/shaderModuleLoader.hpp> 

#include <filesystem>
#include <fstream>
#include <cstdint>
#include <stdexcept>

std::vector<uint8_t> loadModule(const char* filename)
{
    std::string pathString = std::string(ASSETS) + filename;
    std::filesystem::path filepath(pathString);

    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("Shader file not found: " + filepath.string());
    }

    auto filesize = std::filesystem::file_size(filepath);
    std::vector<uint8_t> buffer(filesize);

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open shader file: " + filepath.string());
    }

    file.read(reinterpret_cast<char*>(buffer.data()), filesize);
    if (!file) {
        throw std::runtime_error("Failed to read entire shader file: " + filepath.string());
    }
    
    // null terminate the string buffer 
    buffer.push_back('\0');
    return buffer;
}
