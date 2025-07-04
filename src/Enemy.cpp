#include "Enemy.h"
#include "Platform.h"
#include <cmath>

const float Enemy::MOVE_SPEED = 50.0f;
const float Enemy::EDGE_DETECTION_DISTANCE = 30.0f;

Enemy::Enemy(float x, float y) 
    : m_position(x, y), m_velocity(-MOVE_SPEED, 0), m_width(25), m_height(25), 
      m_isAlive(true), m_movingRight(false), m_isOnGround(false), m_animationTimer(0.0f) {
}

void Enemy::Update(float deltaTime, const std::vector<std::unique_ptr<Platform>>& platforms) {
    if (!m_isAlive) return;
    
    m_animationTimer += deltaTime * 4.0f;
    
    // Apply gravity
    if (!m_isOnGround) {
        m_velocity.y += 800.0f * deltaTime; // Same gravity as player
        m_velocity.y = std::min(m_velocity.y, 500.0f); // Max fall speed
    }
    
    // Edge detection - check if enemy would fall off platform
    bool foundGround = false;
    Vector2 futurePosition = m_position + Vector2(m_velocity.x * deltaTime * 2, 0);
    Vector2 groundCheckPos = futurePosition + Vector2(m_width/2, m_height + 5);
    
    for (const auto& platform : platforms) {
        Rectangle platformBounds = platform->GetBounds();
        if (groundCheckPos.x >= platformBounds.x && 
            groundCheckPos.x <= platformBounds.x + platformBounds.width &&
            groundCheckPos.y >= platformBounds.y && 
            groundCheckPos.y <= platformBounds.y + platformBounds.height) {
            foundGround = true;
            break;
        }
    }
    
    // Change direction if no ground ahead or hit boundary
    if (!foundGround || m_position.x <= 0 || m_position.x >= 1000 - m_width) {
        m_velocity.x = -m_velocity.x;
        m_movingRight = !m_movingRight;
    }
    
    // Update position
    m_position = m_position + m_velocity * deltaTime;
    
    // Keep enemy in bounds
    if (m_position.x < 0) m_position.x = 0;
    if (m_position.x > 1000 - m_width) m_position.x = 1000 - m_width;
}

void Enemy::Render(SDL_Renderer* renderer) {
    if (!m_isAlive) return;
    
    // Walking animation offset
    float walkOffset = sin(m_animationTimer) * 2;
    
    // Draw enemy as a dark red rectangle with animation
    SDL_SetRenderDrawColor(renderer, 139, 0, 0, 255);
    SDL_Rect enemyRect = {
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y + walkOffset),
        static_cast<int>(m_width),
        static_cast<int>(m_height)
    };
    SDL_RenderFillRect(renderer, &enemyRect);
    
    // Draw legs for walking animation
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    float legOffset = sin(m_animationTimer + 1.5f) * 3;
    SDL_Rect leftLeg = {
        static_cast<int>(m_position.x + 3),
        static_cast<int>(m_position.y + m_height - 8 + legOffset),
        4, 8
    };
    SDL_Rect rightLeg = {
        static_cast<int>(m_position.x + m_width - 7),
        static_cast<int>(m_position.y + m_height - 8 - legOffset),
        4, 8
    };
    SDL_RenderFillRect(renderer, &leftLeg);
    SDL_RenderFillRect(renderer, &rightLeg);
    
    // Draw angry eyes
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red eyes for angry look
    int eyeSize = 3;
    int eyeY = static_cast<int>(m_position.y + 6 + walkOffset);
    
    SDL_Rect leftEye = {static_cast<int>(m_position.x + 5), eyeY, eyeSize, eyeSize};
    SDL_Rect rightEye = {static_cast<int>(m_position.x + m_width - 8), eyeY, eyeSize, eyeSize};
    SDL_RenderFillRect(renderer, &leftEye);
    SDL_RenderFillRect(renderer, &rightEye);
    
    // Draw angry eyebrows
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    if (m_movingRight) {
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + 4), static_cast<int>(m_position.y + 4 + walkOffset),
            static_cast<int>(m_position.x + 9), static_cast<int>(m_position.y + 6 + walkOffset));
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + m_width - 9), static_cast<int>(m_position.y + 4 + walkOffset),
            static_cast<int>(m_position.x + m_width - 4), static_cast<int>(m_position.y + 6 + walkOffset));
    } else {
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + 9), static_cast<int>(m_position.y + 4 + walkOffset),
            static_cast<int>(m_position.x + 4), static_cast<int>(m_position.y + 6 + walkOffset));
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + m_width - 4), static_cast<int>(m_position.y + 4 + walkOffset),
            static_cast<int>(m_position.x + m_width - 9), static_cast<int>(m_position.y + 6 + walkOffset));
    }
    
    // Draw direction indicator (spikes)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int centerY = static_cast<int>(m_position.y + m_height/2 + walkOffset);
    if (m_movingRight) {
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + m_width - 2), centerY - 3,
            static_cast<int>(m_position.x + m_width + 2), centerY);
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + m_width - 2), centerY + 3,
            static_cast<int>(m_position.x + m_width + 2), centerY);
    } else {
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + 2), centerY - 3,
            static_cast<int>(m_position.x - 2), centerY);
        SDL_RenderDrawLine(renderer, 
            static_cast<int>(m_position.x + 2), centerY + 3,
            static_cast<int>(m_position.x - 2), centerY);
    }
}

void Enemy::ResolveCollision(const Rectangle& platform) {
    Rectangle enemyBounds = GetBounds();
    
    // Calculate overlap
    float overlapLeft = (enemyBounds.x + enemyBounds.width) - platform.x;
    float overlapRight = (platform.x + platform.width) - enemyBounds.x;
    float overlapTop = (enemyBounds.y + enemyBounds.height) - platform.y;
    float overlapBottom = (platform.y + platform.height) - enemyBounds.y;
    
    // Find minimum overlap
    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
    
    if (minOverlap == overlapTop && m_velocity.y >= 0) {
        // Landing on top of platform
        m_position.y = platform.y - m_height;
        m_velocity.y = 0;
        m_isOnGround = true;
    } else if (minOverlap == overlapBottom && m_velocity.y < 0) {
        // Hitting platform from below
        m_position.y = platform.y + platform.height;
        m_velocity.y = 0;
    } else if (minOverlap == overlapLeft && m_velocity.x > 0) {
        // Hitting platform from left - change direction
        m_position.x = platform.x - m_width;
        m_velocity.x = -MOVE_SPEED;
        m_movingRight = false;
    } else if (minOverlap == overlapRight && m_velocity.x < 0) {
        // Hitting platform from right - change direction
        m_position.x = platform.x + platform.width;
        m_velocity.x = MOVE_SPEED;
        m_movingRight = true;
    }
}

Rectangle Enemy::GetBounds() const {
    return Rectangle(m_position.x, m_position.y, m_width, m_height);
} 