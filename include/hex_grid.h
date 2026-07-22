#pragma once

#include <vector>
#include "particle.h"
#include "config.h"
#include "vec2.h"


struct Hex {
    Hex(int q = 0, int r = 0) : q(q), r(r) {}
    int q, r;

    bool operator==(const Hex& other) const {
        return q == other.q && r == other.r;
    }
};

// для unordered_map
struct HexHash {
    size_t operator()(const Hex& h) const {
        return std::hash<int>()(h.q) ^ (std::hash<int>()(h.r) << 1);
    }
};

class HexGrid {
public:
    HexGrid(float hex_R, const SpaceConfig& space) 
        : hex_R(hex_R), 
          hex_D(std::sqrt(3.0f) * hex_R),
          // Предвычисляем обратные величины для замены деления на умножение
          inv_hex_D(1.0f / hex_D),
          inv_hex_D_sqrt3(1.0f / (hex_D * std::sqrt(3.0f))),
          space(space) {}

    void update_cells(std::vector<Particle>& particles);

    // Переводы координаты
    Hex pixel_to_hex(const Vec2& pos) const;
    Vec2 hex_to_pixel(const Hex& hex) const;

    // Работа с ячейками
    std::vector<Hex> get_intersecting_hexes(const Vec2& pos, const float radius) const;
    const std::vector<size_t>& get_indices_hex(const Hex& hex) const;

    // Геттеры
    float get_hex_radius() const { return hex_R; }
    std::vector<Hex> get_active_cells() const;

private:
    const float hex_R;
    const float hex_D;
    const float inv_hex_D;
    const float inv_hex_D_sqrt3;
    const SpaceConfig& space;
    std::unordered_map<Hex, std::vector<size_t>, HexHash> cells;
};