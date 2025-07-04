#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>
#include <random>

ParticleSystem::ParticleSystem() {
    m_particles.reserve(1000); // Pre-allocate for performance
}

void ParticleSystem::Update(float deltaTime) {
    // Update all particles
    for (auto& particle : m_particles) {
        particle.life -= deltaTime;
        particle.position = particle.position + particle.velocity * deltaTime;
        
        // Apply gravity to some particles
        if (particle.velocity.y > -200) { // Not for upward-flying particles
            particle.velocity.y += 300.0f * deltaTime; // Gravity
        }
        
        // Fade out particles as they age
        float lifeRatio = particle.life / particle.maxLife;
        particle.color.a = static_cast<Uint8>(255 * lifeRatio);
    }
    
    RemoveDeadParticles();
}

void ParticleSystem::Render(SDL_Renderer* renderer) {
    for (const auto& particle : m_particles) {
        SDL_SetRenderDrawColor(renderer, particle.color.r, particle.color.g, particle.color.b, particle.color.a);
        
        // Draw particle as a small rectangle
        SDL_Rect particleRect = {
            static_cast<int>(particle.position.x - particle.size/2),
            static_cast<int>(particle.position.y - particle.size/2),
            static_cast<int>(particle.size),
            static_cast<int>(particle.size)
        };
        SDL_RenderFillRect(renderer, &particleRect);
    }
}

void ParticleSystem::CreateExplosion(Vector2 position, Color color) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    static std::uniform_real_distribution<float> speedDist(50, 150);
    static std::uniform_real_distribution<float> lifeDist(0.5f, 1.2f);
    
    // Create 15-25 particles for explosion
    int particleCount = 15 + (gen() % 11);
    for (int i = 0; i < particleCount; i++) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        Vector2 velocity(cos(angle) * speed, sin(angle) * speed);
        float life = lifeDist(gen);
        
        AddParticle(Particle(position, velocity, color, life, 4.0f));
    }
}

void ParticleSystem::CreateCoinEffect(Vector2 position) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angleDist(-3.14159f/4, -3*3.14159f/4);
    static std::uniform_real_distribution<float> speedDist(80, 120);
    
    // Create sparkling coin effect
    for (int i = 0; i < 8; i++) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        Vector2 velocity(cos(angle) * speed, sin(angle) * speed);
        Color sparkleColor(255, 215, 0); // Gold
        
        AddParticle(Particle(position, velocity, sparkleColor, 1.0f, 3.0f));
    }
    
    // Add some white sparkles
    for (int i = 0; i < 5; i++) {
        float angle = angleDist(gen);
        float speed = speedDist(gen) * 0.7f;
        Vector2 velocity(cos(angle) * speed, sin(angle) * speed);
        Color sparkleColor(255, 255, 255); // White
        
        AddParticle(Particle(position, velocity, sparkleColor, 0.8f, 2.0f));
    }
}

void ParticleSystem::CreateJumpEffect(Vector2 position) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angleDist(3.14159f/6, 5*3.14159f/6);
    static std::uniform_real_distribution<float> speedDist(30, 60);
    
    // Create dust particles when jumping
    for (int i = 0; i < 6; i++) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        Vector2 velocity(cos(angle) * speed, sin(angle) * speed);
        Color dustColor(139, 69, 19, 180); // Brown dust
        
        AddParticle(Particle(position, velocity, dustColor, 0.6f, 3.0f));
    }
}

void ParticleSystem::CreatePowerUpEffect(Vector2 position) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    static std::uniform_real_distribution<float> speedDist(20, 80);
    
    // Create colorful power-up effect
    Color colors[] = {
        Color(255, 0, 255),   // Magenta
        Color(0, 255, 255),   // Cyan
        Color(255, 255, 0),   // Yellow
        Color(0, 255, 0),     // Green
        Color(255, 0, 0)      // Red
    };
    
    for (int i = 0; i < 12; i++) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        Vector2 velocity(cos(angle) * speed, sin(angle) * speed);
        Color color = colors[i % 5];
        
        AddParticle(Particle(position, velocity, color, 1.5f, 4.0f));
    }
}

void ParticleSystem::AddParticle(const Particle& particle) {
    m_particles.push_back(particle);
}

void ParticleSystem::RemoveDeadParticles() {
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const Particle& p) { return p.life <= 0; }),
        m_particles.end()
    );
} 