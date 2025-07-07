#include <loaders/imageLoader.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <loaders/stb_image.h>

utils::Handle<gfx::Texture> loadImage(const char* name)
{
    stbi_set_flip_vertically_on_load(1);

    std::string path = std::string(ASSETS) + name;

    int width, height, channels; 
    unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (image == NULL) {
        std::cout << "Image path: " + path + " Not found!\n";
        return utils::Handle<gfx::Texture>();
    }

    utils::Handle<gfx::Texture> texture = gfx::ResourceManager::instance->Create(gfx::TextureDescriptor{
        .width = (uint32_t)width,
        .height = (uint32_t)height,
        .initialData = utils::Span(reinterpret_cast<uint8_t*>(image), width * height * 4)
        });

    stbi_image_free(image);

    return texture;
}
