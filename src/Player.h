#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

class Player {
public:
    Player(float x, float y);
    
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    void HandleInput(const bool* keystate, const bool* keysPressed);
    void HandleInputSimple(const Uint8* keystate);
    
    Rectangle GetBounds() const;
    Vector2 GetPosition() const { return m_position; }
    void SetPosition(const Vector2& pos) { m_position = pos; }
    
    bool IsOnGround() const { return m_isOnGround; }
    void SetOnGround(bool onGround) { m_isOnGround = onGround; }
    
    void ResolveCollision(const Rectangle& platform);
    
    // Power-up system
    void SetPoweredUp(bool powered) { m_isPoweredUp = powered; m_powerUpTimer = powered ? 10.0f : 0.0f; }
    bool IsPoweredUp() const { return m_isPoweredUp; }
    
    // Animation and visual effects
    void PlayJumpAnimation() { m_jumpAnimTimer = 0.3f; }
    void SetInvulnerable(float time) { m_invulnerabilityTimer = time; }
    bool IsInvulnerable() const { return m_invulnerabilityTimer > 0; }
    
private:
    void RenderNormal(SDL_Renderer* renderer);
    void RenderPoweredUp(SDL_Renderer* renderer);
    void UpdateAnimation(float deltaTime);
    
    Vector2 m_position;
    Vector2 m_velocity;
    float m_width;
    float m_height;
    bool m_isOnGround;
    bool m_facingRight;
    
    // Power-up state
    bool m_isPoweredUp;
    float m_powerUpTimer;
    
    // Animation and effects
    float m_walkAnimTimer;
    float m_jumpAnimTimer;
    float m_invulnerabilityTimer;
    bool m_isMoving;
    
    // Input state tracking
    bool m_wasJumpKeyHeld;
    float m_jumpBuffer;
    float m_coyoteTime;
    
    // Movement constants
    static const float MOVE_SPEED;
    static const float JUMP_FORCE;
    static const float MAX_FALL_SPEED;
    static const float POWERED_UP_SCALE;
}; 