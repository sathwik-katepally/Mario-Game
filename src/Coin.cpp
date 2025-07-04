#include "Coin.h"
#include <cmath>

const float Coin::ANIMATION_SPEED = 3.0f;
const float Coin::FLOAT_AMPLITUDE = 5.0f;

Coin::Coin(float x, float y) 
    : m_position(x, y), m_width(20), m_height(20), m_isCollected(false), 
      m_value(100), m_animationTimer(0.0f), m_floatOffset(0.0f) {
}

void Coin::Update(float deltaTime) {
    if (m_isCollected) return;
    
    // Floating animation
    m_animationTimer += deltaTime * ANIMATION_SPEED;
    m_floatOffset = sin(m_animationTimer) * FLOAT_AMPLITUDE;
}

void Coin::Render(SDL_Renderer* renderer) {
    if (m_isCollected) return;
    
    float renderY = m_position.y + m_floatOffset;
    
    // Draw coin as a yellow circle with shine effect
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold color
    
    // Draw multiple circles to create coin effect
    int centerX = static_cast<int>(m_position.x + m_width/2);
    int centerY = static_cast<int>(renderY + m_height/2);
    int radius = static_cast<int>(m_width/2);
    
    // Draw filled circle (approximate)
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
    
    // Add shine effect
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int dy = -radius/2; dy <= radius/2; dy++) {
        for (int dx = -radius/2; dx <= radius/2; dx++) {
            if (dx*dx + dy*dy <= (radius/3)*(radius/3)) {
                SDL_RenderDrawPoint(renderer, centerX + dx - radius/3, centerY + dy - radius/3);
            }
        }
    }
    
    // Draw dollar sign in the center
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect dollarLine1 = {centerX - 1, centerY - 6, 2, 4};
    SDL_Rect dollarLine2 = {centerX - 1, centerY + 2, 2, 4};
    SDL_Rect dollarVert = {centerX - 1, centerY - 8, 2, 16};
    SDL_RenderFillRect(renderer, &dollarLine1);
    SDL_RenderFillRect(renderer, &dollarLine2);
    SDL_RenderFillRect(renderer, &dollarVert);
}

Rectangle Coin::GetBounds() const {
    if (m_isCollected) return Rectangle(0, 0, 0, 0);
    return Rectangle(m_position.x, m_position.y + m_floatOffset, m_width, m_height);
} 