#pragma once
#include "Game.h"
#include <SDL2/SDL.h>

class Platform {
public:
    Platform(float x, float y, float width, float height);
    
    void Render(SDL_Renderer* renderer);
    Rectangle GetBounds() const;
    
private:
    Rectangle m_bounds;
}; 