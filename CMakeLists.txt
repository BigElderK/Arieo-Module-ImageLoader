cmake_minimum_required (VERSION 3.31)
    arieo_engine_project(
        arieo_image_loader_module
        ALIAS arieo::modules::image_loader
        PROJECT_TYPE module
        SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/private/src/*/*.cpp
        INTERFACES
            arieo::interface::file_loader
        PACKAGES
            stb
        LIBS
            arieo::base
            arieo::core
            stb::stb
    )