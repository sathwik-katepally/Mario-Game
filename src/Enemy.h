#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

class Enemy {
public:
    Enemy(float x, float y);
    
    void Update(float deltaTime, const std::vector<std::unique_ptr<class Platform>>& platforms);
    void Render(SDL_Renderer* renderer);
    Rectangle GetBounds() const;
    
    bool IsAlive() const { return m_isAlive; }
    void Kill() { m_isAlive = false; }
    
    // Platform collision
    void ResolveCollision(const Rectangle& platform);
    void SetOnGround(bool onGround) { m_isOnGround = onGround; }
    
private:
    Vector2 m_position;
    Vector2 m_velocity;
    float m_width;
    float m_height;
    bool m_isAlive;
    bool m_movingRight;
    bool m_isOnGround;
    float m_animationTimer;
    
    static const float MOVE_SPEED;
    static const float EDGE_DETECTION_DISTANCE;
}; 