#include "Game.h"
#include "Player.h"
#include "Platform.h"
#include "Enemy.h"
#include "Coin.h"
#include "MovingPlatform.h"
#include "PowerUp.h"
#include "ParticleSystem.h"
#include <iostream>
#include <sstream>
#include <cmath>

const float Game::GRAVITY = 800.0f; // pixels per second squared

Game::Game() : m_window(nullptr), m_renderer(nullptr), m_font(nullptr), m_smallFont(nullptr), 
               m_isRunning(false), m_gameState(GameState::MENU), m_selectedMenuOption(MenuOption::PLAY), 
               m_score(0), m_lives(3), m_level(1), 
               m_levelTime(0.0f), m_lastFrameTime(0), m_deltaTime(0.0f) {
    // Initialize input arrays
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        m_keys[i] = false;
        m_keysPressed[i] = false;
    }
}

Game::~Game() {
    Shutdown();
}

bool Game::Initialize() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Create window
    m_window = SDL_CreateWindow("Super Mario",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               WINDOW_WIDTH,
                               WINDOW_HEIGHT,
                               SDL_WINDOW_SHOWN);
    
    if (!m_window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Initialize particle system
    m_particleSystem = std::make_unique<ParticleSystem>();
    
    // Load fonts - try bundled font first, then system fonts as fallback
    const char* fontPaths[] = {
        "assets/fonts/PressStart2P-Regular.ttf",           // Bundled retro font
        "/System/Library/Fonts/Helvetica.ttc",            // macOS
        "/System/Library/Fonts/Arial.ttf",                // macOS
        "/Library/Fonts/Arial.ttf",                       // macOS
        "/System/Library/Fonts/Times.ttc",                // macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", // Linux fallback
        "/usr/share/fonts/TTF/DejaVuSans.ttf",            // Linux fallback
        "C:/Windows/Fonts/arial.ttf",                     // Windows fallback
        nullptr
    };
    
    for (int i = 0; fontPaths[i] && !m_font; i++) {
        // Use smaller size for pixel font, larger for system fonts
        int fontSize = (i == 0) ? 16 : 24;
        m_font = TTF_OpenFont(fontPaths[i], fontSize);
        if (m_font) {
            std::cout << "Loaded font: " << fontPaths[i] << " (size: " << fontSize << ")" << std::endl;
        }
    }
    
    if (!m_font) {
        std::cerr << "Warning: Could not load any system font! TTF_Error: " << TTF_GetError() << std::endl;
        std::cerr << "Text rendering will be disabled." << std::endl;
    }
    
    // Try to load smaller font
    for (int i = 0; fontPaths[i] && !m_smallFont; i++) {
        // Use even smaller size for pixel font, normal small for system fonts
        int smallFontSize = (i == 0) ? 12 : 16;
        m_smallFont = TTF_OpenFont(fontPaths[i], smallFontSize);
    }
    
    // Start with menu
    m_gameState = GameState::MENU;
    
    m_isRunning = true;
    m_lastFrameTime = SDL_GetTicks();
    
    return true;
}

void Game::ResetGame() {
    // Clear all game objects
    m_platforms.clear();
    m_movingPlatforms.clear();
    m_enemies.clear();
    m_coins.clear();
    m_powerUps.clear();
    
    // Reset game state
    m_score = 0;
    m_lives = 3;
    m_level = 1;
    m_levelTime = 0.0f;
    
    // Initialize player
    m_player = std::make_unique<Player>(50, WINDOW_HEIGHT - 100);
    
    // Create level 1 - Balanced and achievable design
    // Ground platform
    m_platforms.push_back(std::make_unique<Platform>(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50));
    
    // Static platforms - arranged as stepping stones with reasonable gaps
    m_platforms.push_back(std::make_unique<Platform>(150, WINDOW_HEIGHT - 120, 120, 20));    // Low platform
    m_platforms.push_back(std::make_unique<Platform>(320, WINDOW_HEIGHT - 180, 120, 20));    // Medium platform
    m_platforms.push_back(std::make_unique<Platform>(500, WINDOW_HEIGHT - 240, 120, 20));    // Higher platform
    m_platforms.push_back(std::make_unique<Platform>(680, WINDOW_HEIGHT - 180, 120, 20));    // Back down
    m_platforms.push_back(std::make_unique<Platform>(850, WINDOW_HEIGHT - 140, 120, 20));    // Final platform
    
    // Additional platforms for easier navigation
    m_platforms.push_back(std::make_unique<Platform>(50, WINDOW_HEIGHT - 200, 80, 15));      // Helper platform
    m_platforms.push_back(std::make_unique<Platform>(400, WINDOW_HEIGHT - 120, 80, 15));     // Helper platform
    m_platforms.push_back(std::make_unique<Platform>(750, WINDOW_HEIGHT - 120, 80, 15));     // Helper platform
    
    // Moving platforms - positioned to help reach higher areas
    m_movingPlatforms.push_back(std::make_unique<MovingPlatform>(250, WINDOW_HEIGHT - 140, 60, 12, MovementType::HORIZONTAL, 40, 60));
    m_movingPlatforms.push_back(std::make_unique<MovingPlatform>(550, WINDOW_HEIGHT - 200, 60, 12, MovementType::VERTICAL, 30, 40));
    m_movingPlatforms.push_back(std::make_unique<MovingPlatform>(780, WINDOW_HEIGHT - 200, 50, 12, MovementType::HORIZONTAL, 35, 50));
    
    // Enemies - placed strategically but not blocking essential paths
    m_enemies.push_back(std::make_unique<Enemy>(200, WINDOW_HEIGHT - 100));
    m_enemies.push_back(std::make_unique<Enemy>(450, WINDOW_HEIGHT - 100));
    m_enemies.push_back(std::make_unique<Enemy>(700, WINDOW_HEIGHT - 100));
    m_enemies.push_back(std::make_unique<Enemy>(350, WINDOW_HEIGHT - 210));
    
    // Coins - placed near platforms and reachable with normal jumps
    m_coins.push_back(std::make_unique<Coin>(200, WINDOW_HEIGHT - 150));    // On first platform
    m_coins.push_back(std::make_unique<Coin>(370, WINDOW_HEIGHT - 210));    // On second platform  
    m_coins.push_back(std::make_unique<Coin>(550, WINDOW_HEIGHT - 270));    // On third platform
    m_coins.push_back(std::make_unique<Coin>(730, WINDOW_HEIGHT - 210));    // On fourth platform
    m_coins.push_back(std::make_unique<Coin>(900, WINDOW_HEIGHT - 170));    // On final platform
    m_coins.push_back(std::make_unique<Coin>(100, WINDOW_HEIGHT - 230));    // On helper platform
    m_coins.push_back(std::make_unique<Coin>(450, WINDOW_HEIGHT - 150));    // On helper platform
    m_coins.push_back(std::make_unique<Coin>(800, WINDOW_HEIGHT - 150));    // On helper platform
    m_coins.push_back(std::make_unique<Coin>(300, WINDOW_HEIGHT - 100));    // Ground level coin
    m_coins.push_back(std::make_unique<Coin>(600, WINDOW_HEIGHT - 100));    // Ground level coin
    
    // Power-ups - placed on accessible platforms
    m_powerUps.push_back(std::make_unique<PowerUp>(370, WINDOW_HEIGHT - 220, PowerUpType::SUPER_MARIO));    // On second platform
    m_powerUps.push_back(std::make_unique<PowerUp>(100, WINDOW_HEIGHT - 240, PowerUpType::SPEED_BOOST));    // On helper platform
    m_powerUps.push_back(std::make_unique<PowerUp>(900, WINDOW_HEIGHT - 180, PowerUpType::EXTRA_LIFE));     // On final platform
}

void Game::NextLevel() {
    m_level++;
    
    // Clear current level objects
    m_platforms.clear();
    m_movingPlatforms.clear();
    m_enemies.clear();
    m_coins.clear();
    m_powerUps.clear();
    
    // Reset player position
    m_player->SetPosition(Vector2(50, WINDOW_HEIGHT - 100));
    m_levelTime = 0.0f;
    
    // Create more challenging level
    m_platforms.push_back(std::make_unique<Platform>(0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50));
    
    // More complex platform layout
    for (int i = 0; i < 8; i++) {
        float x = 100 + i * 120;
        float y = WINDOW_HEIGHT - 150 - (i % 3) * 100;
        m_platforms.push_back(std::make_unique<Platform>(x, y, 80, 15));
    }
    
    // More moving platforms
    for (int i = 0; i < 4; i++) {
        MovementType type = static_cast<MovementType>(i % 3);
        float x = 200 + i * 200;
        float y = WINDOW_HEIGHT - 200 - i * 50;
        m_movingPlatforms.push_back(std::make_unique<MovingPlatform>(x, y, 60, 12, type, 50 + i * 20, 80));
    }
    
    // More enemies
    for (int i = 0; i < m_level + 3; i++) {
        float x = 150 + i * 120;
        float y = WINDOW_HEIGHT - 100;
        m_enemies.push_back(std::make_unique<Enemy>(x, y));
    }
    
    // More coins
    for (int i = 0; i < 12; i++) {
        float x = 80 + i * 80;
        float y = WINDOW_HEIGHT - 120 - (i % 4) * 80;
        m_coins.push_back(std::make_unique<Coin>(x, y));
    }
    
    // More power-ups
    m_powerUps.push_back(std::make_unique<PowerUp>(300, WINDOW_HEIGHT - 280, PowerUpType::SUPER_MARIO));
    m_powerUps.push_back(std::make_unique<PowerUp>(600, WINDOW_HEIGHT - 350, PowerUpType::SPEED_BOOST));
    m_powerUps.push_back(std::make_unique<PowerUp>(800, WINDOW_HEIGHT - 200, PowerUpType::EXTRA_LIFE));
}

void Game::Run() {
    while (m_isRunning) {
        // Calculate delta time
        Uint32 currentTime = SDL_GetTicks();
        m_deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTime;
        
        // Cap delta time to prevent large jumps
        if (m_deltaTime > 0.016f) {
            m_deltaTime = 0.016f;
        }
        
        HandleEvents();
        Update();
        Render();
    }
}

void Game::HandleEvents() {
    SDL_Event e;
    
    // Reset pressed keys
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        m_keysPressed[i] = false;
    }
    
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            m_isRunning = false;
        } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
            m_keysPressed[e.key.keysym.scancode] = true;
            
            // Menu controls
            if (m_gameState == GameState::MENU) {
                if (e.key.keysym.scancode == SDL_SCANCODE_UP || 
                    e.key.keysym.scancode == SDL_SCANCODE_W) {
                    m_selectedMenuOption = MenuOption::PLAY;
                } else if (e.key.keysym.scancode == SDL_SCANCODE_DOWN || 
                           e.key.keysym.scancode == SDL_SCANCODE_S) {
                    m_selectedMenuOption = MenuOption::INSTRUCTIONS;
                } else if (e.key.keysym.scancode == SDL_SCANCODE_SPACE || 
                           e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    if (m_selectedMenuOption == MenuOption::PLAY) {
                        ResetGame();
                        m_gameState = GameState::PLAYING;
                    } else if (m_selectedMenuOption == MenuOption::INSTRUCTIONS) {
                        m_gameState = GameState::INSTRUCTIONS;
                    }
                }
            } else if (m_gameState == GameState::INSTRUCTIONS) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE || 
                    e.key.keysym.scancode == SDL_SCANCODE_BACKSPACE ||
                    e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    m_gameState = GameState::MENU;
                }
            } else if (m_gameState == GameState::GAME_OVER) {
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE || 
                    e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    m_gameState = GameState::MENU;
                }
            } else if (m_gameState == GameState::PLAYING) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    m_gameState = GameState::PAUSED;
                } else if (e.key.keysym.scancode == SDL_SCANCODE_R) {
                    ResetGame();
                }
            } else if (m_gameState == GameState::PAUSED) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    m_gameState = GameState::PLAYING;
                }
            }
        }
    }
    
    // Update continuous key states
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        m_keys[i] = keystate[i];
    }
    
    // Handle player input only when playing - use direct SDL keystate for maximum reliability
    if (m_gameState == GameState::PLAYING && m_player) {
        const Uint8* directKeystate = SDL_GetKeyboardState(NULL);
        m_player->HandleInputSimple(directKeystate);
    }
}

void Game::Update() {
    if (m_gameState != GameState::PLAYING || !m_player) return;
    
    m_levelTime += m_deltaTime;
    
    // Update player
    m_player->Update(m_deltaTime);
    
    // Update moving platforms
    for (auto& platform : m_movingPlatforms) {
        platform->Update(m_deltaTime);
    }
    
    // Update enemies
    for (auto& enemy : m_enemies) {
        if (enemy->IsAlive()) {
            enemy->Update(m_deltaTime, m_platforms);
            enemy->SetOnGround(false);
        }
    }
    
    // Update coins
    for (auto& coin : m_coins) {
        coin->Update(m_deltaTime);
    }
    
    // Update power-ups
    for (auto& powerUp : m_powerUps) {
        powerUp->Update(m_deltaTime);
    }
    
    // Update particle system
    m_particleSystem->Update(m_deltaTime);
    
    // Platform collisions for player
    Rectangle playerBounds = m_player->GetBounds();
    m_player->SetOnGround(false);
    
    // Static platforms
    for (const auto& platform : m_platforms) {
        Rectangle platformBounds = platform->GetBounds();
        if (playerBounds.Intersects(platformBounds)) {
            m_player->ResolveCollision(platformBounds);
        }
    }
    
    // Moving platforms (with velocity transfer)
    for (const auto& platform : m_movingPlatforms) {
        Rectangle platformBounds = platform->GetBounds();
        if (playerBounds.Intersects(platformBounds)) {
            m_player->ResolveCollision(platformBounds);
            
            // Transfer platform velocity to player when on top
            if (m_player->IsOnGround()) {
                Vector2 platformVel = platform->GetVelocity();
                Vector2 newPos = m_player->GetPosition() + Vector2(platformVel.x * m_deltaTime, 0);
                m_player->SetPosition(newPos);
            }
        }
    }
    
    // Enemy platform collisions
    for (auto& enemy : m_enemies) {
        if (!enemy->IsAlive()) continue;
        
        Rectangle enemyBounds = enemy->GetBounds();
        
        // Static platforms
        for (const auto& platform : m_platforms) {
            Rectangle platformBounds = platform->GetBounds();
            if (enemyBounds.Intersects(platformBounds)) {
                enemy->ResolveCollision(platformBounds);
            }
        }
        
        // Moving platforms
        for (const auto& platform : m_movingPlatforms) {
            Rectangle platformBounds = platform->GetBounds();
            if (enemyBounds.Intersects(platformBounds)) {
                enemy->ResolveCollision(platformBounds);
            }
        }
    }
    
    // Coin collection
    for (auto& coin : m_coins) {
        if (!coin->IsCollected() && playerBounds.Intersects(coin->GetBounds())) {
            coin->Collect();
            m_score += coin->GetValue();
            m_particleSystem->CreateCoinEffect(coin->GetBounds().Center());
        }
    }
    
    // Power-up collection
    for (auto& powerUp : m_powerUps) {
        if (!powerUp->IsCollected() && playerBounds.Intersects(powerUp->GetBounds())) {
            powerUp->Collect();
            m_particleSystem->CreatePowerUpEffect(powerUp->GetBounds().Center());
            
            switch (powerUp->GetType()) {
                case PowerUpType::SUPER_MARIO:
                    m_player->SetPoweredUp(true);
                    m_score += 200;
                    break;
                case PowerUpType::SPEED_BOOST:
                    m_score += 150;
                    break;
                case PowerUpType::EXTRA_LIFE:
                    m_lives++;
                    m_score += 500;
                    break;
            }
        }
    }
    
    // Enemy-player collisions
    for (auto& enemy : m_enemies) {
        if (enemy->IsAlive() && playerBounds.Intersects(enemy->GetBounds())) {
            Vector2 playerPos = m_player->GetPosition();
            Vector2 enemyPos = Vector2(enemy->GetBounds().x, enemy->GetBounds().y);
            
            if (playerPos.y < enemyPos.y - 10 && !m_player->IsInvulnerable()) {
                // Player stomped enemy
                enemy->Kill();
                m_score += 150;
                m_particleSystem->CreateExplosion(enemy->GetBounds().Center(), Color(139, 0, 0));
                
                // Small bounce
                Vector2 bouncePos = Vector2(playerPos.x, playerPos.y - 5);
                m_player->SetPosition(bouncePos);
            } else if (!m_player->IsInvulnerable()) {
                // Player hit by enemy
                if (m_player->IsPoweredUp()) {
                    m_player->SetPoweredUp(false);
                    m_player->SetInvulnerable(2.0f);
                } else {
                    m_lives--;
                    m_player->SetInvulnerable(2.0f);
                    if (m_lives <= 0) {
                        m_gameState = GameState::GAME_OVER;
                    } else {
                        // Reset position
                        m_player->SetPosition(Vector2(50, WINDOW_HEIGHT - 100));
                    }
                }
            }
        }
    }
    
    // Check for level completion (all coins collected)
    bool allCoinsCollected = true;
    for (const auto& coin : m_coins) {
        if (!coin->IsCollected()) {
            allCoinsCollected = false;
            break;
        }
    }
    
    if (allCoinsCollected) {
        NextLevel();
        m_score += 1000 * m_level; // Bonus for completing level
    }
    
    // Keep player in bounds
    Vector2 playerPos = m_player->GetPosition();
    if (playerPos.x < 0) {
        m_player->SetPosition(Vector2(0, playerPos.y));
    } else if (playerPos.x > WINDOW_WIDTH - 30) {
        m_player->SetPosition(Vector2(WINDOW_WIDTH - 30, playerPos.y));
    }
    
    // Check if player fell off screen
    if (playerPos.y > WINDOW_HEIGHT) {
        m_lives--;
        if (m_lives <= 0) {
            m_gameState = GameState::GAME_OVER;
        } else {
            m_player->SetPosition(Vector2(50, WINDOW_HEIGHT - 100));
        }
    }
    
    // Add jump effect particles when jump keys are pressed and player is on ground
    if ((m_keysPressed[SDL_SCANCODE_SPACE] || m_keysPressed[SDL_SCANCODE_UP] || m_keysPressed[SDL_SCANCODE_W]) && 
        m_player->IsOnGround()) {
        m_particleSystem->CreateJumpEffect(Vector2(playerPos.x + 15, playerPos.y + 30));
    }
}

void Game::Render() {
    // Clear screen with gradient sky
    SDL_SetRenderDrawColor(m_renderer, 135, 206, 235, 255);
    SDL_RenderClear(m_renderer);
    
    // Render gradient background
    for (int y = 0; y < WINDOW_HEIGHT/2; y++) {
        Uint8 r = 135 + (120 * y) / (WINDOW_HEIGHT/2);
        Uint8 g = 206 + (49 * y) / (WINDOW_HEIGHT/2);
        Uint8 b = 235 + (20 * y) / (WINDOW_HEIGHT/2);
        SDL_SetRenderDrawColor(m_renderer, r, g, b, 255);
        SDL_RenderDrawLine(m_renderer, 0, y, WINDOW_WIDTH, y);
    }
    
    if (m_gameState == GameState::MENU) {
        RenderMenu();
    } else if (m_gameState == GameState::INSTRUCTIONS) {
        RenderInstructions();
    } else if (m_gameState == GameState::GAME_OVER) {
        RenderGameOver();
    } else {
        // Render game objects
        for (const auto& platform : m_platforms) {
            platform->Render(m_renderer);
        }
        
        for (const auto& platform : m_movingPlatforms) {
            platform->Render(m_renderer);
        }
        
        for (const auto& coin : m_coins) {
            coin->Render(m_renderer);
        }
        
        for (const auto& powerUp : m_powerUps) {
            powerUp->Render(m_renderer);
        }
        
        for (const auto& enemy : m_enemies) {
            enemy->Render(m_renderer);
        }
        
        if (m_player) {
            m_player->Render(m_renderer);
        }
        
        // Render particles
        m_particleSystem->Render(m_renderer);
        
        // Render finish flag when close to completion
        int coinsCollected = 0;
        for (const auto& coin : m_coins) {
            if (coin->IsCollected()) {
                coinsCollected++;
            }
        }
        if (coinsCollected >= static_cast<int>(m_coins.size()) - 3) {
            // Draw finish flag pole
            SDL_SetRenderDrawColor(m_renderer, 139, 69, 19, 255); // Brown pole
            SDL_Rect flagPole = {920, WINDOW_HEIGHT - 250, 8, 200};
            SDL_RenderFillRect(m_renderer, &flagPole);
            
            // Draw flag
            SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255); // Red flag
            SDL_Rect flag = {928, WINDOW_HEIGHT - 240, 40, 30};
            SDL_RenderFillRect(m_renderer, &flag);
            
            // Draw flag details
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255); // White stripes
            SDL_Rect stripe1 = {930, WINDOW_HEIGHT - 235, 36, 5};
            SDL_Rect stripe2 = {930, WINDOW_HEIGHT - 225, 36, 5};
            SDL_Rect stripe3 = {930, WINDOW_HEIGHT - 215, 36, 5};
            SDL_RenderFillRect(m_renderer, &stripe1);
            SDL_RenderFillRect(m_renderer, &stripe2);
            SDL_RenderFillRect(m_renderer, &stripe3);
            
            // Draw "GOAL" text above flag
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
            SDL_Rect goalBg = {905, WINDOW_HEIGHT - 280, 60, 20};
            SDL_RenderFillRect(m_renderer, &goalBg);
            
            SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
            SDL_Rect goalText = {910, WINDOW_HEIGHT - 275, 50, 10};
            SDL_RenderFillRect(m_renderer, &goalText);
        }
        
        // Render UI
        RenderUI();
        
        if (m_gameState == GameState::PAUSED) {
            // Render pause overlay
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 128);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(m_renderer, &overlay);
            
            // Pause text
            RenderTextCentered("PAUSED", WINDOW_HEIGHT/2 - 50, {255, 255, 255, 255});
            RenderTextCentered("Press ESC to resume", WINDOW_HEIGHT/2 - 10, {255, 255, 255, 255});
            RenderTextCentered("Press R to restart level", WINDOW_HEIGHT/2 + 30, {255, 255, 255, 255});
        }
    }
    
    SDL_RenderPresent(m_renderer);
}

void Game::RenderText(const std::string& text, int x, int y, SDL_Color color) {
    if (!m_font) {
        // Fallback: render a colored rectangle as placeholder
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_Rect fallbackRect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderFillRect(m_renderer, &fallbackRect);
        return;
    }
    
    // First render black shadow for better contrast
    SDL_Surface* shadowSurface = TTF_RenderText_Solid(m_font, text.c_str(), {0, 0, 0, 255});
    if (shadowSurface) {
        SDL_Texture* shadowTexture = SDL_CreateTextureFromSurface(m_renderer, shadowSurface);
        if (shadowTexture) {
            SDL_Rect shadowRect = {x + 2, y + 2, shadowSurface->w, shadowSurface->h};
            SDL_RenderCopy(m_renderer, shadowTexture, nullptr, &shadowRect);
            SDL_DestroyTexture(shadowTexture);
        }
        SDL_FreeSurface(shadowSurface);
    }
    
    // Then render the main text
    SDL_Surface* textSurface = TTF_RenderText_Solid(m_font, text.c_str(), color);
    if (!textSurface) {
        // Fallback to rectangle if text rendering fails
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_Rect fallbackRect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderFillRect(m_renderer, &fallbackRect);
        return;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        // Fallback to rectangle
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_Rect fallbackRect = {x, y, static_cast<int>(text.length() * 8), 20};
        SDL_RenderFillRect(m_renderer, &fallbackRect);
        return;
    }
    
    SDL_Rect destRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(m_renderer, textTexture, nullptr, &destRect);
    
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void Game::RenderTextCentered(const std::string& text, int y, SDL_Color color) {
    if (!m_font) {
        // Fallback: estimate center position
        int estimatedWidth = text.length() * 8;
        int x = (WINDOW_WIDTH - estimatedWidth) / 2;
        RenderText(text, x, y, color);
        return;
    }
    
    int textWidth, textHeight;
    if (TTF_SizeText(m_font, text.c_str(), &textWidth, &textHeight) == 0) {
        int x = (WINDOW_WIDTH - textWidth) / 2;
        RenderText(text, x, y, color);
    } else {
        // Fallback if size calculation fails
        int estimatedWidth = text.length() * 8;
        int x = (WINDOW_WIDTH - estimatedWidth) / 2;
        RenderText(text, x, y, color);
    }
}

void Game::RenderUI() {
    // Score
    std::string scoreText = "Score: " + std::to_string(m_score);
    RenderText(scoreText, 10, 10, {255, 255, 255, 255});
    
    // Level
    std::string levelText = "Level: " + std::to_string(m_level);
    RenderText(levelText, 10, 40, {255, 255, 255, 255});
    
    // Lives
    std::string livesText = "Lives: " + std::to_string(m_lives);
    RenderText(livesText, 10, 70, {255, 255, 255, 255});
    
    // Draw hearts for lives
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
    for (int i = 0; i < m_lives && i < 5; i++) {
        int x = 120 + i * 25;
        int y = 73;
        // Simple heart shape approximation
        SDL_Rect heart = {x, y, 15, 15};
        SDL_RenderFillRect(m_renderer, &heart);
    }
    
    // Coin progress indicator
    int coinsCollected = 0;
    int totalCoins = m_coins.size();
    for (const auto& coin : m_coins) {
        if (coin->IsCollected()) {
            coinsCollected++;
        }
    }
    
    // Coins text
    std::string coinsText = "Coins: " + std::to_string(coinsCollected) + "/" + std::to_string(totalCoins);
    RenderText(coinsText, 10, 100, {255, 255, 255, 255});
    
    // Draw coin progress bar
    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255); // Gray background
    SDL_Rect coinBg = {150, 105, 130, 15};
    SDL_RenderFillRect(m_renderer, &coinBg);
    
    SDL_SetRenderDrawColor(m_renderer, 255, 215, 0, 255); // Gold progress
    int progressWidth = totalCoins > 0 ? (coinsCollected * 130) / totalCoins : 0;
    SDL_Rect coinProgress = {150, 105, progressWidth, 15};
    SDL_RenderFillRect(m_renderer, &coinProgress);
    

    
    // Goal indicator when close to completion
    if (coinsCollected >= totalCoins - 2) {
        RenderText("ALMOST", WINDOW_WIDTH - 110, WINDOW_HEIGHT/2 - 25, {0, 255, 0, 255});
        RenderText("DONE!", WINDOW_WIDTH - 100, WINDOW_HEIGHT/2 - 5, {0, 255, 0, 255});
    }
    
    // Power-up indicator
    if (m_player && m_player->IsPoweredUp()) {
        RenderText("POWERED UP!", WINDOW_WIDTH - 130, 15, {255, 255, 0, 255});
    }
}

void Game::RenderMenu() {
    // Title
    RenderTextCentered("SUPER MARIO GAME", 120, {255, 255, 0, 255});
    
    // Subtitle
    RenderTextCentered("Collect all coins to complete the level!", 180, {255, 255, 255, 255});
    
    // Menu Options
    int menuY = 280;
    int optionSpacing = 60;
    
    // Play Option
    if (m_selectedMenuOption == MenuOption::PLAY) {
        // Highlighted - draw background
        SDL_SetRenderDrawColor(m_renderer, 0, 128, 0, 128); // Semi-transparent green
        SDL_Rect playBg = {WINDOW_WIDTH/2 - 100, menuY - 5, 200, 40};
        SDL_RenderFillRect(m_renderer, &playBg);
        RenderTextCentered("PLAY", menuY + 5, {255, 255, 0, 255}); // Yellow text
    } else {
        RenderTextCentered("PLAY", menuY + 5, {180, 180, 180, 255}); // Gray text
    }
    
    // Instructions Option
    menuY += optionSpacing;
    if (m_selectedMenuOption == MenuOption::INSTRUCTIONS) {
        // Highlighted - draw background
        SDL_SetRenderDrawColor(m_renderer, 0, 128, 0, 128); // Semi-transparent green
        SDL_Rect instrBg = {WINDOW_WIDTH/2 - 100, menuY - 5, 200, 40};
        SDL_RenderFillRect(m_renderer, &instrBg);
        RenderTextCentered("INSTRUCTIONS", menuY + 5, {255, 255, 0, 255}); // Yellow text
    } else {
        RenderTextCentered("INSTRUCTIONS", menuY + 5, {180, 180, 180, 255}); // Gray text
    }
    
    // Navigation hints
    RenderTextCentered("Use UP/DOWN to navigate", 460, {255, 255, 255, 255});
    RenderTextCentered("Press SPACE or ENTER to select", 490, {255, 255, 255, 255});
    
    // Create some demo particles
    m_particleSystem->CreateCoinEffect(Vector2(200, 400));
    m_particleSystem->CreatePowerUpEffect(Vector2(800, 400));
    m_particleSystem->Render(m_renderer);
}

void Game::RenderInstructions() {
    // Title
    RenderTextCentered("GAME INSTRUCTIONS", 30, {255, 255, 255, 255});
    
    // Two-column layout
    int leftColumnX = 30;
    int rightColumnX = 520;
    int yPos = 80;
    int lineHeight = 26;
    int sectionSpacing = 34;
    
    // Left Column - Controls and Objectives
    
    // Movement controls section
    RenderText("MOVEMENT CONTROLS:", leftColumnX, yPos, {255, 255, 0, 255});
    yPos += sectionSpacing;
    
    RenderText("  A / LEFT  - Move left", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  D / RIGHT - Move right", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  SPACE/W/UP - Jump", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += sectionSpacing;
    
    // Game objectives section
    RenderText("GAME OBJECTIVES:", leftColumnX, yPos, {255, 255, 0, 255});
    yPos += sectionSpacing;
    
    RenderText("  * Collect all COINS", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  * Avoid or stomp ENEMIES", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  * Use platforms to navigate", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += sectionSpacing;
    
    // Power-ups section
    RenderText("POWER-UPS:", leftColumnX, yPos, {255, 255, 0, 255});
    yPos += sectionSpacing;
    
    // Power-up examples with colored squares and text
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255); // Red for Super Mario
    SDL_Rect superMario = {leftColumnX + 20, yPos + 3, 12, 12};
    SDL_RenderFillRect(m_renderer, &superMario);
    RenderText("Super Mario - More power", leftColumnX + 40, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    
    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255); // Green for Speed
    SDL_Rect speedBoost = {leftColumnX + 20, yPos + 3, 12, 12};
    SDL_RenderFillRect(m_renderer, &speedBoost);
    RenderText("Speed Boost - Move faster", leftColumnX + 40, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255); // Magenta for Extra Life
    SDL_Rect extraLife = {leftColumnX + 20, yPos + 3, 12, 12};
    SDL_RenderFillRect(m_renderer, &extraLife);
    RenderText("Extra Life - Additional life", leftColumnX + 40, yPos, {255, 255, 255, 255});
    yPos += sectionSpacing;
    
    // Tips section
    RenderText("TIPS:", leftColumnX, yPos, {255, 255, 0, 255});
    yPos += sectionSpacing;
    RenderText("  - Jump on enemies to defeat", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  - Use moving platforms", leftColumnX + 20, yPos, {255, 255, 255, 255});
    yPos += lineHeight;
    RenderText("  - Watch for the goal flag!", leftColumnX + 20, yPos, {255, 255, 255, 255});
    
    // Right Column - Visual examples
    int rightY = 80;
    
    RenderText("GAME ELEMENTS:", rightColumnX, rightY, {255, 255, 0, 255});
    rightY += sectionSpacing;
    
    // Draw example coin
    SDL_SetRenderDrawColor(m_renderer, 255, 215, 0, 255);
    SDL_Rect exampleCoin = {rightColumnX, rightY, 18, 18};
    SDL_RenderFillRect(m_renderer, &exampleCoin);
    RenderText("Coin (collect)", rightColumnX + 25, rightY + 3, {255, 255, 255, 255});
    rightY += 35;
    
    // Draw example enemy
    SDL_SetRenderDrawColor(m_renderer, 139, 0, 0, 255);
    SDL_Rect exampleEnemy = {rightColumnX, rightY, 22, 18};
    SDL_RenderFillRect(m_renderer, &exampleEnemy);
    RenderText("Enemy (avoid/stomp)", rightColumnX + 30, rightY + 3, {255, 255, 255, 255});
    rightY += 35;
    
    // Draw example platform
    SDL_SetRenderDrawColor(m_renderer, 139, 69, 19, 255);
    SDL_Rect examplePlatform = {rightColumnX, rightY, 50, 12};
    SDL_RenderFillRect(m_renderer, &examplePlatform);
    RenderText("Platform", rightColumnX + 60, rightY + 1, {255, 255, 255, 255});
    rightY += 35;
    
    // Draw example moving platform with arrow
    SDL_SetRenderDrawColor(m_renderer, 0, 100, 255, 255);
    SDL_Rect exampleMovingPlatform = {rightColumnX, rightY, 50, 12};
    SDL_RenderFillRect(m_renderer, &exampleMovingPlatform);
    
    // Arrow indicating movement
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
    SDL_Rect arrow = {rightColumnX + 15, rightY - 8, 20, 6};
    SDL_RenderFillRect(m_renderer, &arrow);
    
    RenderText("Moving Platform", rightColumnX + 60, rightY + 1, {255, 255, 255, 255});
    rightY += 35;
    
    // Additional visual examples
    RenderText("SCORING:", rightColumnX, rightY + 10, {255, 255, 0, 255});
    rightY += 40;
    RenderText("Coin: 100 points", rightColumnX, rightY, {255, 255, 0, 255});
    rightY += lineHeight;
    RenderText("Enemy defeat: 150 points", rightColumnX, rightY, {255, 50, 50, 255});
    rightY += lineHeight;
    RenderText("Level complete: 1000+ points", rightColumnX, rightY, {0, 255, 0, 255});
    
    // Back instruction at bottom
    RenderTextCentered("Press ESC, BACKSPACE, or SPACE to go back", 600, {255, 255, 255, 255});
    
    // Add some particle effects
    m_particleSystem->CreateCoinEffect(Vector2(rightColumnX + 10, 200));
    m_particleSystem->Render(m_renderer);
}

void Game::RenderGameOver() {
    // Game Over title
    RenderTextCentered("GAME OVER", 200, {255, 0, 0, 255});
    
    // Final score
    std::string scoreText = "Final Score: " + std::to_string(m_score);
    RenderTextCentered(scoreText, 280, {255, 255, 255, 255});
    
    // Level reached
    std::string levelText = "Level Reached: " + std::to_string(m_level);
    RenderTextCentered(levelText, 320, {255, 255, 255, 255});
    
    // Instructions
    RenderTextCentered("Press SPACE or ENTER to return to menu", 380, {255, 255, 255, 255});
}

void Game::Shutdown() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    
    if (m_smallFont) {
        TTF_CloseFont(m_smallFont);
        m_smallFont = nullptr;
    }
    
    TTF_Quit();
    
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    SDL_Quit();
} 