#include "Game.h"
#include <iostream>
#include <exception>

int main() {
    try {
        // Create game instance
        Game game;
        
        // Initialize game
        if (!game.initialize(1920, 1080, "3D Racing Game")) {
            std::cerr << "Failed to initialize game" << std::endl;
            return -1;
        }
        
        std::cout << "3D Racing Game initialized successfully!" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "  WASD - Car movement" << std::endl;
        std::cout << "  Mouse - Camera look" << std::endl;
        std::cout << "  Space - Boost" << std::endl;
        std::cout << "  Shift - Handbrake" << std::endl;
        std::cout << "  ESC - Pause" << std::endl;
        std::cout << "  F1 - Reset" << std::endl;
        std::cout << "PvP Controls:" << std::endl;
        std::cout << "  Q - Laser" << std::endl;
        std::cout << "  F - Punch" << std::endl;
        std::cout << "  E - Hold Shield" << std::endl;
        std::cout << "  R - Teleport forward" << std::endl;
        std::cout << "Stats (spend points):" << std::endl;
        std::cout << "  1 - Strength, 2 - Defense, 3 - Stamina, 4 - Agility" << std::endl;
        
        // Run game
        game.run();
        
        std::cout << "Game ended successfully" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return -1;
    }
}