#include "Platform.h"

Platform::Platform(float x, float y, float width, float height) 
    : m_bounds(x, y, width, height) {
}

void Platform::Render(SDL_Renderer* renderer) {
    // Draw platform as a brown rectangle
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
    SDL_Rect platformRect = {
        static_cast<int>(m_bounds.x),
        static_cast<int>(m_bounds.y),
        static_cast<int>(m_bounds.width),
        static_cast<int>(m_bounds.height)
    };
    SDL_RenderFillRect(renderer, &platformRect);
    
    // Draw platform border
    SDL_SetRenderDrawColor(renderer, 101, 67, 33, 255);
    SDL_RenderDrawRect(renderer, &platformRect);
}

Rectangle Platform::GetBounds() const {
    return m_bounds;
} 