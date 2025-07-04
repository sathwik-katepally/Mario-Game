#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <memory>
#include <string>
#include <cmath>

// Forward declarations
class Player;
class Platform;
class Enemy;
class Coin;
class MovingPlatform;
class PowerUp;
class ParticleSystem;

enum class GameState {
    MENU,
    INSTRUCTIONS,
    PLAYING,
    GAME_OVER,
    PAUSED
};

enum class MenuOption {
    PLAY,
    INSTRUCTIONS
};

class Game {
public:
    Game();
    ~Game();
    
    bool Initialize();
    void Run();
    void Shutdown();
    
private:
    void HandleEvents();
    void Update();
    void Render();
    void RenderUI();
    void RenderMenu();
    void RenderInstructions();
    void RenderGameOver();
    
    // Text rendering helpers
    void RenderText(const std::string& text, int x, int y, SDL_Color color = {255, 255, 255, 255});
    void RenderTextCentered(const std::string& text, int y, SDL_Color color = {255, 255, 255, 255});
    
    // Game state management
    void ResetGame();
    void NextLevel();
    
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font* m_font;
    TTF_Font* m_smallFont;
    bool m_isRunning;
    
    // Game objects
    std::unique_ptr<Player> m_player;
    std::vector<std::unique_ptr<Platform>> m_platforms;
    std::vector<std::unique_ptr<MovingPlatform>> m_movingPlatforms;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Coin>> m_coins;
    std::vector<std::unique_ptr<PowerUp>> m_powerUps;
    std::unique_ptr<ParticleSystem> m_particleSystem;
    
    // Game state
    GameState m_gameState;
    MenuOption m_selectedMenuOption;
    int m_score;
    int m_lives;
    int m_level;
    float m_levelTime;
    
    // Game constants
    static const int WINDOW_WIDTH = 1000;
    static const int WINDOW_HEIGHT = 700;
    static const float GRAVITY;
    
    Uint32 m_lastFrameTime;
    float m_deltaTime;
    
    // Input handling
    bool m_keys[SDL_NUM_SCANCODES];
    bool m_keysPressed[SDL_NUM_SCANCODES];
};

// Enhanced utility structs
struct Vector2 {
    float x, y;
    Vector2(float x = 0, float y = 0) : x(x), y(y) {}
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    float Length() const { return sqrt(x*x + y*y); }
    Vector2 Normalized() const { float len = Length(); return len > 0 ? Vector2(x/len, y/len) : Vector2(0,0); }
};

struct Rectangle {
    float x, y, width, height;
    Rectangle(float x = 0, float y = 0, float w = 0, float h = 0) : x(x), y(y), width(w), height(h) {}
    
    bool Intersects(const Rectangle& other) const {
        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }
    
    Vector2 Center() const { return Vector2(x + width/2, y + height/2); }
};

struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255) : r(r), g(g), b(b), a(a) {}
}; 