#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    (void)argc; // Suppress unused parameter warning
    (void)argv;
    
    Game game;
    
    if (!game.Initialize()) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }
    
    game.Run();
    game.Shutdown();
    
    return 0;
} 