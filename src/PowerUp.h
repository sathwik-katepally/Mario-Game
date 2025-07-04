#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

enum class PowerUpType {
    SUPER_MARIO,
    SPEED_BOOST,
    EXTRA_LIFE
};

class PowerUp {
public:
    PowerUp(float x, float y, PowerUpType type);
    
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    Rectangle GetBounds() const;
    
    bool IsCollected() const { return m_isCollected; }
    void Collect() { m_isCollected = true; }
    PowerUpType GetType() const { return m_type; }
    
private:
    Vector2 m_position;
    float m_width;
    float m_height;
    bool m_isCollected;
    PowerUpType m_type;
    float m_animationTimer;
    Color m_color;
    
    static const float ANIMATION_SPEED;
}; 