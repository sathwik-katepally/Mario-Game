#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

class Coin {
public:
    Coin(float x, float y);
    
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    Rectangle GetBounds() const;
    
    bool IsCollected() const { return m_isCollected; }
    void Collect() { m_isCollected = true; }
    int GetValue() const { return m_value; }
    
private:
    Vector2 m_position;
    float m_width;
    float m_height;
    bool m_isCollected;
    int m_value;
    float m_animationTimer;
    float m_floatOffset;
    
    static const float ANIMATION_SPEED;
    static const float FLOAT_AMPLITUDE;
}; 