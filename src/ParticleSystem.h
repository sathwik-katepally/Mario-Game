#pragma once
#include "Game.h"
#include <SDL2/SDL.h>
#include <vector>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
    float size;
    
    Particle(Vector2 pos, Vector2 vel, Color col, float lifetime, float particleSize = 3.0f)
        : position(pos), velocity(vel), color(col), life(lifetime), maxLife(lifetime), size(particleSize) {}
};

class ParticleSystem {
public:
    ParticleSystem();
    
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);
    
    // Effect creation methods
    void CreateExplosion(Vector2 position, Color color = Color(255, 100, 0));
    void CreateCoinEffect(Vector2 position);
    void CreateJumpEffect(Vector2 position);
    void CreatePowerUpEffect(Vector2 position);
    
private:
    std::vector<Particle> m_particles;
    
    void AddParticle(const Particle& particle);
    void RemoveDeadParticles();
}; 