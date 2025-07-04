#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

enum class MovementType {
    HORIZONTAL,
    VERTICAL,
    CIRCULAR
};

class MovingPlatform {
public:
    MovingPlatform(float x, float y, float width, float height, MovementType type, float speed = 50.0f, float range = 100.0f);
    
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    Rectangle GetBounds() const;
    Vector2 GetVelocity() const { return m_velocity; }
    
private:
    Vector2 m_position;
    Vector2 m_startPosition;
    Vector2 m_velocity;
    Rectangle m_bounds;
    MovementType m_movementType;
    float m_speed;
    float m_range;
    float m_timer;
    bool m_movingForward;
}; 