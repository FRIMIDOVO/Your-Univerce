#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Image.hpp>
#include <filesystem>
#include <fstream>
#include <string>

namespace FrameSaver {
    void save_frame(const sf::RenderTexture& texture, int frame) {
        std::filesystem::create_directories("frames");
        
        if (auto buffer = texture.getTexture().copyToImage().saveToMemory("png")) {
            std::string path = "frames/frame_" + std::to_string(frame) + ".png";
            
            std::ofstream file(path, std::ios::binary);
            if (file) { // Проверяем успешность открытия файла
                file.write(reinterpret_cast<const char*>(buffer->data()), buffer->size());
            }
        }
    }
}
