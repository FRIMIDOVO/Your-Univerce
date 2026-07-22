#include "hex_grid.h"


void HexGrid::update_cells(std::vector<Particle>& particles) {
    cells.clear(); // очищаем сетку

    for (size_t i = 0; i < particles.size(); ++i) {
        // получаем все пересекающиеся с партиклом ячейки
        Particle& p = particles[i];
        std::vector<Hex> intersecting_hexes = get_intersecting_hexes(p.get_position(), p.radius());

        // записываем во все эти ячейки индекс партикла в списке
        for (Hex& hex : intersecting_hexes) {
            cells[hex].push_back(i);
        }
    }
}

std::vector<Hex> HexGrid::get_intersecting_hexes(const Vec2& pos, const float radius) const {
    std::vector<Hex> result;

    Hex center = pixel_to_hex(pos);

    int d_max = std::ceil((radius + hex_R) / (1.5f * hex_R));
    float max_dist_sq = (radius + hex_R) * (radius + hex_R);

    for (int dq = -d_max; dq <= d_max; ++dq) {
        for (int dr = -d_max; dr <= d_max; ++dr) {
            
            int cdq = std::abs(dq);
            int cdr = std::abs(dr);
            int hex_dist = (cdq + cdr + std::abs(dq + dr)) / 2;
            
            if (hex_dist <= d_max) {
                int q = center.q + dq;
                int r = center.r + dr;
                
                Vec2 hex_center = hex_to_pixel(Hex(q, r));
                Vec2 diff = pos - hex_center;
                
                if (diff.norm_sq() <= max_dist_sq) {
                    result.push_back(Hex(q, r));
                }
            }
        }
    }

    return result;
}

const std::vector<size_t>& HexGrid::get_indices_hex(const Hex& hex) const {
    auto it = cells.find(hex);
    if (it != cells.end()) {
        return it->second; // Возвращаем ссылку на существующий вектор
    }
    // Если гекс пустой, возвращаем ссылку на статический пустой вектор
    static const std::vector<size_t> empty_vector;
    return empty_vector;
}

std::vector<Hex> HexGrid::get_active_cells() const {
    std::vector<Hex> result;
    result.reserve(cells.size());
    
    for (const auto& pair : cells) {
        result.push_back(pair.first);
    }
    
    return result;
}

Hex HexGrid::pixel_to_hex(const Vec2& pos) const {
    float q = pos.x * inv_hex_D + pos.y * inv_hex_D_sqrt3;
    float r = -2 * pos.y * inv_hex_D_sqrt3;

    float& x = q;
    float& z = r;
    float y = -x -z;

    int rx = round(x);
    int ry = round(y);
    int rz = round(z);

    float dx = abs(rx -x);
    float dy = abs(ry -y);
    float dz = abs(rz -z);

    if (dx > dy && dx > dz) {
        rx = -ry -rz;
    } else if (dy > dx && dy > dz) {
        ry = -rx -rz;
    } else if (dz > dy && dz > dx) {
        rz = -rx - ry;
    }

    return Hex(rx, rz);
}

Vec2 HexGrid::hex_to_pixel(const Hex& hex) const {
    float x = hex_D * (hex.q + hex.r * 0.5f);
    float y = -hex_R * 1.5f * hex.r;
    return Vec2(x, y);
}