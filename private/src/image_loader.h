#pragma once
#include "interface/file_loader/image_loader.h"
#include <fstream>
namespace Arieo
{
    class ImageLoader
        : public Interface::FileLoader::IImageLoader
    {
    public:
        Interface::FileLoader::ImageBuffer loadDDS(void* buffer, size_t size) override;
    };
}