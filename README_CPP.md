# Mario Game - C++ Implementation

A classic Mario-style platformer game implemented in C++ using SDL2.

## Features

- **Player Character**: A red Mario-like character with smooth movement and jumping
- **Physics System**: Gravity, collision detection, and realistic movement
- **Platforms**: Multiple platforms to jump on and navigate
- **Enemies**: Moving enemies that can be defeated by jumping on them
- **Game Mechanics**: 
  - Left/Right movement (Arrow keys or A/D)
  - Jumping (Up arrow, W, or Spacebar)
  - Enemy stomping
  - Respawn system when falling or touching enemies

## Prerequisites

You need SDL2 and its extensions installed on your system:

### macOS (using Homebrew)
```bash
brew install sdl2 sdl2_image sdl2_ttf
```

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```

### Windows
Download SDL2 development libraries from [libsdl.org](https://www.libsdl.org/download-2.0.php)

## Building and Running

1. **Install dependencies** (if not already installed):
   ```bash
   make install-deps
   ```

2. **Build the game**:
   ```bash
   make
   ```

3. **Run the game**:
   ```bash
   ./mario_game
   ```

4. **Clean build files**:
   ```bash
   make clean
   ```

## Controls

- **Arrow Keys** or **A/D**: Move left and right
- **Up Arrow**, **W**, or **Spacebar**: Jump
- **ESC** or close window: Quit game

## Game Mechanics

### Player
- Gravity affects the player when not on ground
- Can jump only when touching a platform
- Has momentum-based movement
- Can defeat enemies by jumping on them
- Respawns at starting position when touching enemies from the side or falling off screen

### Enemies
- Move back and forth automatically
- Can be defeated by jumping on them from above
- Will reset player position if touched from the side

### Platforms
- Static brown platforms that provide collision surfaces
- Include ground platform and floating platforms

## Code Structure

- `src/main.cpp`: Entry point
- `src/Game.cpp/h`: Main game class handling initialization, game loop, and cleanup
- `src/Player.cpp/h`: Player character with movement, physics, and input handling
- `src/Platform.cpp/h`: Static platform objects
- `src/Enemy.cpp/h`: Enemy objects with simple AI
- `Makefile`: Build configuration with cross-platform SDL2 support

## Future Enhancements

Potential improvements you could add:
- Sprite graphics instead of colored rectangles
- Sound effects and music
- Multiple levels
- Power-ups (bigger Mario, fire power, etc.)
- Coins and scoring system
- More enemy types
- Animated sprites
- Parallax scrolling backgrounds

## Comparison with JavaScript Version

This C++ version includes several improvements over the original JavaScript version:
- Better object-oriented design with separate classes
- More robust physics and collision detection
- Multiple platforms and enemies
- Proper game architecture for easy expansion
- Cross-platform compatibility
- Better performance due to native compilation

Enjoy your C++ Mario game! 