#include "PowerUp.h"
#include <cmath>

const float PowerUp::ANIMATION_SPEED = 2.0f;

PowerUp::PowerUp(float x, float y, PowerUpType type) 
    : m_position(x, y), m_width(25), m_height(25), m_isCollected(false), 
      m_type(type), m_animationTimer(0.0f) {
    
    // Set color based on type
    switch (m_type) {
        case PowerUpType::SUPER_MARIO:
            m_color = Color(255, 0, 0); // Red
            break;
        case PowerUpType::SPEED_BOOST:
            m_color = Color(0, 255, 0); // Green
            break;
        case PowerUpType::EXTRA_LIFE:
            m_color = Color(255, 0, 255); // Magenta
            break;
    }
}

void PowerUp::Update(float deltaTime) {
    if (m_isCollected) return;
    
    m_animationTimer += deltaTime * ANIMATION_SPEED;
}

void PowerUp::Render(SDL_Renderer* renderer) {
    if (m_isCollected) return;
    
    // Pulsing effect
    float pulse = (sin(m_animationTimer * 3.0f) + 1.0f) * 0.5f;
    float scale = 0.8f + pulse * 0.4f;
    
    float scaledWidth = m_width * scale;
    float scaledHeight = m_height * scale;
    float offsetX = (m_width - scaledWidth) * 0.5f;
    float offsetY = (m_height - scaledHeight) * 0.5f;
    
    // Draw power-up based on type
    SDL_SetRenderDrawColor(renderer, m_color.r, m_color.g, m_color.b, m_color.a);
    
    SDL_Rect powerUpRect = {
        static_cast<int>(m_position.x + offsetX),
        static_cast<int>(m_position.y + offsetY),
        static_cast<int>(scaledWidth),
        static_cast<int>(scaledHeight)
    };
    SDL_RenderFillRect(renderer, &powerUpRect);
    
    // Add type-specific visual elements
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int centerX = static_cast<int>(m_position.x + m_width/2);
    int centerY = static_cast<int>(m_position.y + m_height/2);
    
    switch (m_type) {
        case PowerUpType::SUPER_MARIO:
        {
            // Draw "S" for Super
            SDL_Rect sTop = {centerX - 4, centerY - 6, 8, 2};
            SDL_Rect sMiddle = {centerX - 4, centerY - 1, 8, 2};
            SDL_Rect sBottom = {centerX - 4, centerY + 4, 8, 2};
            SDL_RenderFillRect(renderer, &sTop);
            SDL_RenderFillRect(renderer, &sMiddle);
            SDL_RenderFillRect(renderer, &sBottom);
            break;
        }
            
        case PowerUpType::SPEED_BOOST:
        {
            // Draw lightning bolt
            SDL_RenderDrawLine(renderer, centerX - 3, centerY - 6, centerX + 1, centerY);
            SDL_RenderDrawLine(renderer, centerX - 1, centerY, centerX + 3, centerY + 6);
            SDL_RenderDrawLine(renderer, centerX - 2, centerY - 3, centerX + 2, centerY - 3);
            SDL_RenderDrawLine(renderer, centerX - 2, centerY + 3, centerX + 2, centerY + 3);
            break;
        }
            
        case PowerUpType::EXTRA_LIFE:
        {
            // Draw heart shape
            for (int y = -4; y <= 2; y++) {
                for (int x = -5; x <= 5; x++) {
                    // Simple heart approximation
                    if ((x*x + (y+2)*(y+2) <= 9 && y <= 0) || 
                        ((x-2)*(x-2) + (y-1)*(y-1) <= 4) || 
                        ((x+2)*(x+2) + (y-1)*(y-1) <= 4)) {
                        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
                    }
                }
            }
            break;
        }
    }
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &powerUpRect);
}

Rectangle PowerUp::GetBounds() const {
    if (m_isCollected) return Rectangle(0, 0, 0, 0);
    return Rectangle(m_position.x, m_position.y, m_width, m_height);
} 