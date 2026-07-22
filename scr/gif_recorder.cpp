#include "gif_recorder.h"
#include "gif.h"       // Включаем ТОЛЬКО здесь!
#include <iostream>

GifRecorder::GifRecorder(const std::string& filename, VisualizerConfig& viz, int delay_cs)
    : width_(viz.width_texture), height_(viz.height_texture), delay_cs_(delay_cs), is_open_(false) {
    
    pixels_.resize(width_ * height_ * 4); // 4 байта на пиксель (RGBA)
    
    // Преобразуем наш массив байт в указатель на GifWriter
    GifWriter* writer = reinterpret_cast<GifWriter*>(writer_memory);
    
    is_open_ = GifBegin(writer, filename.c_str(), width_, height_, delay_cs_);
    
    if (!is_open_) {
        std::cerr << "Ошибка: не удалось создать GIF-файл " << filename << std::endl;
    }
}

GifRecorder::~GifRecorder() {
    if (is_open_) {
        GifWriter* writer = reinterpret_cast<GifWriter*>(writer_memory);
        GifEnd(writer); // Обязательно закрываем файл
    }
}

void GifRecorder::save_frame(const sf::RenderTexture& texture) {
    if (!is_open_) return;

    // 1. Берём текстуру и копируем пиксели в sf::Image (нативный способ SFML)
    sf::Image image = texture.getTexture().copyToImage();
    const uint8_t* src = image.getPixelsPtr();
    
    // 2. Копируем в наш буфер
    for (size_t i = 0; i < pixels_.size(); ++i) {
        pixels_[i] = src[i];
    }

    // 3. Пишем кадр в GIF
    GifWriter* writer = reinterpret_cast<GifWriter*>(writer_memory);
    GifWriteFrame(writer, pixels_.data(), width_, height_, delay_cs_);
}