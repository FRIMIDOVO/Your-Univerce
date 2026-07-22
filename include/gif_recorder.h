#pragma once
#include <SFML/Graphics.hpp>
#include "config.h"
#include <string>
#include <vector>
#include <cstdint>

class GifRecorder {
public:
    GifRecorder(const std::string& filename, VisualizerConfig& viz, int delay_cs = 2);
    ~GifRecorder();

    void save_frame(const sf::RenderTexture& texture);

private:
    // Храним внутреннее состояние GifWriter как массив байт, 
    // чтобы не включать gif.h в этот заголовок и избежать multiple definition
    alignas(8) uint8_t writer_memory[32]; 
    
    std::vector<uint8_t> pixels_;
    int width_;
    int height_;
    int delay_cs_;
    bool is_open_;
};