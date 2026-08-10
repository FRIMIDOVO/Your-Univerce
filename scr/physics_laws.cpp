#include <cmath>
#include "physics_laws.h"

namespace Laws {
    void inertia(std::vector<Particle>& particles, const PhysicsConfig& physics) {
        for (Particle& p : particles) {
            p.move(physics.dt);
        }
    }

    void gravity(std::vector<Particle>& particles, const PhysicsConfig& physics) {
        if (physics.G) {
            for (size_t i = 0; i < particles.size(); ++i) {
                for (size_t j = i + 1; j < particles.size(); ++j) {
                    auto& p1 = particles[i];
                    auto& p2 = particles[j];
                    
                    Vec2 diff = p2.get_position() - p1.get_position();
                    float dist = diff.norm() + physics.softening;
                    float force = physics.G * p1.get_mass() * p2.get_mass() / (dist * dist);
                    
                    Vec2 accel = diff * (force / p1.get_mass() / dist);
                    p1.add_velocity(accel * physics.dt);
                    
                    accel = diff * (-force / p2.get_mass() / dist);
                    p2.add_velocity(accel * physics.dt);
                }
            }
        }
    }

    void bounce(std::vector<Particle>& particles, const SpaceConfig& space) {
        for (Particle& p : particles) {
            Vec2 pos = p.get_position();
            Vec2 vel = p.get_velocity();
            float r = p.radius();
            
            if (pos.x < space.min_bound + r) {
                p.set_position({space.min_bound + r, pos.y});
                p.set_velocity({std::abs(vel.x), vel.y});
            } else if (pos.x > space.max_bound - r) {
                p.set_position({space.max_bound - r, pos.y});
                p.set_velocity({-std::abs(vel.x), vel.y});
            }
            
            if (pos.y < space.min_bound + r) {
                p.set_position({pos.x, space.min_bound + r});
                p.set_velocity({vel.x, std::abs(vel.y)});
            } else if (pos.y > space.max_bound - r) {
                p.set_position({pos.x, space.max_bound - r});
                p.set_velocity({vel.x, -std::abs(vel.y)});
            }
        }
    }

    // Внедряем пространственную сетку + правильную физику столкновений
    void elastic(std::vector<Particle>& particles, const PhysicsConfig& physics, const HexGrid& hex_grid) {
        
        // Проходим по всем частицам, используя ИНДЕКС i
        for (size_t i = 0; i < particles.size(); ++i) {
            Particle& p1 = particles[i];
            
            // 1. BROAD-PHASE: Получаем все гексы, которые задевает эта частица
            std::vector<Hex> intersecting_hexes = hex_grid.get_intersecting_hexes(p1.get_position(), p1.radius());

            for (const Hex& hex : intersecting_hexes) {
                // 2. Получаем КОНСТАНТНУЮ ссылку на вектор индексов (0 копий памяти!)
                const std::vector<size_t>& indices = hex_grid.get_indices_hex(hex);

                for (size_t idx : indices) {
                    // 3. 🔑 КЛЮЧЕВАЯ ОПТИМИЗАЦИЯ:
                    // Это условие полностью заменяет цикл "for (size_t j = i + 1; ...)"
                    // Оно гарантирует, что мы не сравниваем частицу саму с собой (idx == i)
                    // и что каждая пара (p1, p2) обрабатывается РОВНО ОДИН РАЗ.
                    if (idx <= i) {
                        continue;
                    }

                    // 4. МГНОВЕННЫЙ доступ ко второй частице по индексу (O(1))
                    Particle& p2 = particles[idx];

                    // --- 5. NARROW-PHASE: Твоя правильная физика столкновений ---
                    Vec2 diff = p2.get_position() - p1.get_position();
                    float dist_sq = diff.norm_sq();
                    float min_dist = p1.radius() + p2.radius();
                    
                    if (dist_sq < min_dist * min_dist && dist_sq > 0.00001f) {
                        float dist = std::sqrt(dist_sq);
                        Vec2 normal = diff / dist;
                        
                        float m1 = p1.get_mass();
                        float m2 = p2.get_mass();
                        float total_mass = m1 + m2;
                        
                        // 5.1. РАЗДЕЛЕНИЕ ПЕРЕКРЫТИЯ ПРОПОРЦИОНАЛЬНО МАССЕ
                        float overlap = min_dist - dist;
                        float overlap1 = overlap * (m2 / total_mass); 
                        float overlap2 = overlap * (m1 / total_mass); 
                        
                        p1.set_position(p1.get_position() - normal * overlap1);
                        p2.set_position(p2.get_position() + normal * overlap2);
                        
                        // 5.2. ИМПУЛЬС С УЧЕТОМ УПРУГОСТИ
                        Vec2 rel_vel = p1.get_velocity() - p2.get_velocity();
                        float rel_vel_norm = rel_vel.x * normal.x + rel_vel.y * normal.y;
                        
                        if (rel_vel_norm > 0) { // Проверяем, сближаются ли они
                            float impulse = (1.0f + physics.K_elasticity) * rel_vel_norm / (1.0f / m1 + 1.0f / m2);
                            p1.add_velocity(-normal * (impulse / m1));
                            p2.add_velocity(normal * (impulse / m2));
                        }
                    }
                    // --- КОНЕЦ ФИЗИКИ ---
                }
            }
        }
    }

    // Внедряем пространственную сетку для трения О(N*k)
    void friction(std::vector<Particle>& particles, const PhysicsConfig& physics, const HexGrid& hex_grid) {
        for (size_t i = 0; i < particles.size(); ++i) {
            Particle& p1 = particles[i];
            
            // 1. BROAD-PHASE: Получаем все гексы в радиусе действия трения.
            // Используем p1.radius() * contact_distance как безопасную оценку радиуса поиска.
            float search_radius = p1.radius() * physics.contact_distance;
            std::vector<Hex> intersecting_hexes = hex_grid.get_intersecting_hexes(p1.get_position(), search_radius);

            for (const Hex& hex : intersecting_hexes) {
                // 2. Получаем КОНСТАНТНУЮ ссылку на вектор индексов (0 копий памяти!)
                const std::vector<size_t>& indices = hex_grid.get_indices_hex(hex);

                for (size_t idx : indices) {
                    // 3. 🔑 КЛЮЧЕВАЯ ОПТИМИЗАЦИЯ:
                    // Гарантируем, что каждая пара (p1, p2) обрабатывается РОВНО ОДИН РАЗ.
                    // Это также автоматически исключает сравнение частицы самой с собой (idx == i).
                    if (idx <= i) {
                        continue;
                    }

                    // 4. МГНОВЕННЫЙ доступ ко второй частице по индексу (O(1))
                    Particle& p2 = particles[idx];
                    
                    // 5. NARROW-PHASE: Точная проверка трения
                    Vec2 diff = p2.get_position() - p1.get_position();
                    float dist_sq = diff.norm_sq();
                    float min_dist = (p1.radius() + p2.radius()) * physics.contact_distance;
                    
                    // Проверяем dist_sq > 0.000001f вместо dist > 0.001f, чтобы избежать std::sqrt до последней необходимости
                    if (dist_sq < min_dist * min_dist && dist_sq > 0.000001f) {
                        float dist = std::sqrt(dist_sq);
                        Vec2 normal = diff / dist;
                        
                        Vec2 rel_vel = p1.get_velocity() - p2.get_velocity();
                        float rel_vel_norm = rel_vel.x * normal.x + rel_vel.y * normal.y; // скалярное произведение
                        
                        Vec2 rel_vel_tang = rel_vel - normal * rel_vel_norm;
                        
                        float damping = 1.0f - physics.K_friction * physics.dt * 30.0f;
                        if (damping < 0) damping = 0;
                        
                        p1.add_velocity(-rel_vel_tang * (1.0f - damping) / 2.0f);
                        p2.add_velocity(rel_vel_tang * (1.0f - damping) / 2.0f);
                    }
                }
            }
        }
    }

}