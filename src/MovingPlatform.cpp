#include "MovingPlatform.h"
#include <cmath>

MovingPlatform::MovingPlatform(float x, float y, float width, float height, MovementType type, float speed, float range)
    : m_position(x, y), m_startPosition(x, y), m_velocity(0, 0), m_bounds(x, y, width, height),
      m_movementType(type), m_speed(speed), m_range(range), m_timer(0.0f), m_movingForward(true) {
}

void MovingPlatform::Update(float deltaTime) {
    m_timer += deltaTime;
    
    switch (m_movementType) {
        case MovementType::HORIZONTAL:
            if (m_movingForward) {
                m_velocity.x = m_speed;
                if (m_position.x >= m_startPosition.x + m_range) {
                    m_movingForward = false;
                }
            } else {
                m_velocity.x = -m_speed;
                if (m_position.x <= m_startPosition.x) {
                    m_movingForward = true;
                }
            }
            m_velocity.y = 0;
            break;
            
        case MovementType::VERTICAL:
            if (m_movingForward) {
                m_velocity.y = -m_speed;
                if (m_position.y <= m_startPosition.y - m_range) {
                    m_movingForward = false;
                }
            } else {
                m_velocity.y = m_speed;
                if (m_position.y >= m_startPosition.y) {
                    m_movingForward = true;
                }
            }
            m_velocity.x = 0;
            break;
            
        case MovementType::CIRCULAR:
            {
                float angle = m_timer * m_speed * 0.02f; // Slower circular motion
                m_position.x = m_startPosition.x + cos(angle) * m_range;
                m_position.y = m_startPosition.y + sin(angle) * m_range * 0.5f;
                
                // Calculate velocity for smooth player movement
                float nextAngle = angle + deltaTime * m_speed * 0.02f;
                Vector2 nextPos(m_startPosition.x + cos(nextAngle) * m_range,
                               m_startPosition.y + sin(nextAngle) * m_range * 0.5f);
                m_velocity = (nextPos - m_position) * (1.0f / deltaTime);
            }
            break;
    }
    
    if (m_movementType != MovementType::CIRCULAR) {
        m_position = m_position + m_velocity * deltaTime;
    }
    
    // Update bounds
    m_bounds.x = m_position.x;
    m_bounds.y = m_position.y;
}

void MovingPlatform::Render(SDL_Renderer* renderer) {
    // Draw platform with a different color to distinguish from static platforms
    SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255); // Cornflower blue
    SDL_Rect platformRect = {
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.y),
        static_cast<int>(m_bounds.width),
        static_cast<int>(m_bounds.height)
    };
    SDL_RenderFillRect(renderer, &platformRect);
    
    // Draw platform border
    SDL_SetRenderDrawColor(renderer, 65, 105, 225, 255); // Royal blue
    SDL_RenderDrawRect(renderer, &platformRect);
    
    // Add movement indicator arrows
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int centerX = static_cast<int>(m_bounds.x + m_bounds.width/2);
    int centerY = static_cast<int>(m_bounds.y + m_bounds.height/2);
    
    if (m_movementType == MovementType::HORIZONTAL) {
        // Draw left-right arrows
        SDL_RenderDrawLine(renderer, centerX - 10, centerY, centerX + 10, centerY);
        SDL_RenderDrawLine(renderer, centerX - 8, centerY - 2, centerX - 10, centerY);
        SDL_RenderDrawLine(renderer, centerX - 8, centerY + 2, centerX - 10, centerY);
        SDL_RenderDrawLine(renderer, centerX + 8, centerY - 2, centerX + 10, centerY);
        SDL_RenderDrawLine(renderer, centerX + 8, centerY + 2, centerX + 10, centerY);
    } else if (m_movementType == MovementType::VERTICAL) {
        // Draw up-down arrows
        SDL_RenderDrawLine(renderer, centerX, centerY - 10, centerX, centerY + 10);
        SDL_RenderDrawLine(renderer, centerX - 2, centerY - 8, centerX, centerY - 10);
        SDL_RenderDrawLine(renderer, centerX + 2, centerY - 8, centerX, centerY - 10);
        SDL_RenderDrawLine(renderer, centerX - 2, centerY + 8, centerX, centerY + 10);
        SDL_RenderDrawLine(renderer, centerX + 2, centerY + 8, centerX, centerY + 10);
    } else {
        // Draw circular arrow for circular movement
        for (int i = 0; i < 8; i++) {
            float angle1 = i * 3.14159f / 4;
            float angle2 = (i + 1) * 3.14159f / 4;
            int x1 = centerX + static_cast<int>(cos(angle1) * 8);
            int y1 = centerY + static_cast<int>(sin(angle1) * 8);
            int x2 = centerX + static_cast<int>(cos(angle2) * 8);
            int y2 = centerY + static_cast<int>(sin(angle2) * 8);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }
    }
}

Rectangle MovingPlatform::GetBounds() const {
    return m_bounds;
} 