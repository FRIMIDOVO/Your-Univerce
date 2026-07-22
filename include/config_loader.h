#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "config.h"

class ConfigLoader {
public:
    // === УНИВЕРСАЛЬНЫЙ ЗАГРУЗЧИК ===
    template<typename T>
    static void load(const std::string& filename, T& config) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "[CONFIG] No config found for " << type_name<T>() << ". Using defaults.\n";
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream iss(line);
            std::string key;
            float value;
            if (!(iss >> key >> value)) continue;

            set_field(config, key, value);
        }
        file.close();
        std::cout << "[CONFIG] Loaded " << type_name<T>() << " from " << filename << "\n";
    }

    // === УНИВЕРСАЛЬНЫЙ СОХРАНЯЛКА ===
    template<typename T>
    static void save(const std::string& filename, const T& config) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[CONFIG] Failed to save " << type_name<T>() << " to " << filename << "\n";
            return;
        }

        file << "# " << type_name<T>() << " Configuration\n";
        save_fields(file, config);
        file.close();
        std::cout << "[CONFIG] Saved " << type_name<T>() << " to " << filename << "\n";
    }

private:
    // === ВСПОМОГАТЕЛЬНЫЙ МЕТОД ДЛЯ ПОЛУЧЕНИЯ ИМЕНИ ТИПА ===
    template<typename T>
    static std::string type_name() {
        if constexpr (std::is_same_v<T, PhysicsConfig>) return "Physics";
        if constexpr (std::is_same_v<T, SpaceConfig>) return "Space";
        if constexpr (std::is_same_v<T, VisualizerConfig>) return "Visualizer";
        return "Unknown";
    }

    // === УСТАНОВКА ПОЛЯ (специализация для PhysicsConfig) ===
    static void set_field(PhysicsConfig& config, const std::string& key, float value);

    // === УСТАНОВКА ПОЛЯ (специализация для SpaceConfig) ===
    static void set_field(SpaceConfig& config, const std::string& key, float value);

    // === УСТАНОВКА ПОЛЯ (специализация для VisualizerConfig) ===
    static void set_field(VisualizerConfig& config, const std::string& key, float value);

    // === СОХРАНЕНИЕ ПОЛЕЙ (специализация для PhysicsConfig) ===
    static void save_fields(std::ofstream& file, const PhysicsConfig& config);

    // === СОХРАНЕНИЕ ПОЛЕЙ (специализация для SpaceConfig) ===
    static void save_fields(std::ofstream& file, const SpaceConfig& config);

    // === СОХРАНЕНИЕ ПОЛЕЙ (специализация для VisualizerConfig) ===
    static void save_fields(std::ofstream& file, const VisualizerConfig& config);
};