#pragma once

#include <unordered_map>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <vector>

#include "vec2.h"
#include "config.h"


class Particle {
private:
    int id;

    Vec2 position;
    Vec2 velocity;
    float mass;
    
public:
    Particle(
        int id,
        Vec2 position,
        Vec2 velocity,
        float mass
    ) : id(id),
        position(position),
        velocity(velocity),
        mass(mass) {}

    static Particle random_particle(const SpaceConfig& space);

    int get_id() const { return id; }
    float get_mass() const { return mass; }
    const Vec2& get_position() const { return position; }
    const Vec2& get_velocity() const { return velocity; }

    // Свойства
    float speed() const { return velocity.norm(); }
    float kinetic_energy() const { return 0.5f * mass * speed() * speed(); }
    float radius() const { return mass * 0.05f; }

    // Движение
    void set_position(const Vec2& new_pos) { position = new_pos; }
    void set_velocity(const Vec2& new_vel) { velocity = new_vel; }
    void add_velocity(const Vec2& delta) { velocity = velocity + delta; }
    void move(float dt) { position = position + velocity * dt; }
};