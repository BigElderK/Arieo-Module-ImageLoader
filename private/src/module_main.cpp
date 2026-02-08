#include "base/prerequisites.h"
#include "core/core.h"
#include "image_loader.h"
namespace Arieo
{
    GENERATOR_MODULE_ENTRY_FUN()
    ARIEO_DLLEXPORT void ModuleMain()
    {
        Core::Logger::setDefaultLogger("image_loader");

        static struct DllLoader
        {
            ImageLoader image_loader;
            DllLoader()
            {
                Core::ModuleManager::registerInterface<Interface::FileLoader::IImageLoader>(
                    "image_loader", 
                    &image_loader
                );
            }

            ~DllLoader()
            {
                Core::ModuleManager::unregisterInterface<Interface::FileLoader::IImageLoader>(
                    &image_loader
                );
            }
        } dll_loader;
    }
}