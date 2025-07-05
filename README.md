# Mario Game

A classic Mario-style platformer game built with C++

## 🚀 Quick Start Guide

### 1. Download the Source Code

```bash
# Clone the repository
git clone https://github.com/your-username/Mario-Game.git
cd Mario-Game
```

### 2. Install Dependencies

#### 🍎 **macOS**
```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install sdl2 sdl2_image sdl2_ttf
```

#### 🪟 **Windows**

1. Download and install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 terminal and run:
```bash
# Update package database
pacman -Syu

# Install dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf
```

### 3. Build the Game

```bash
# Clean any previous builds
make clean

# Build the game
make all
```

### 4. Run the Game and Have a Good Time Fam

```bash
# Run the Mario game
./mario_game
```
