#ifndef __IMAGE_LOADER_HPP__
#define __IMAGE_LOADER_HPP__

#include <vector> 
#include <resources/resourceManger.hpp>

utils::Handle<gfx::Texture> loadImage(const char* name);

#endif