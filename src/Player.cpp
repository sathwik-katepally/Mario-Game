#include "Player.h"
#include <algorithm>
#include <cmath>

const float Player::MOVE_SPEED = 200.0f;
const float Player::JUMP_FORCE = -400.0f;
const float Player::MAX_FALL_SPEED = 500.0f;
const float Player::POWERED_UP_SCALE = 1.5f;

Player::Player(float x, float y) 
    : m_position(x, y), m_velocity(0, 0), m_width(30), m_height(30), 
      m_isOnGround(false), m_facingRight(true), m_isPoweredUp(false), m_powerUpTimer(0.0f),
      m_walkAnimTimer(0.0f), m_jumpAnimTimer(0.0f), m_invulnerabilityTimer(0.0f), m_isMoving(false),
      m_wasJumpKeyHeld(false), m_jumpBuffer(0.0f), m_coyoteTime(0.0f) {
}

void Player::Update(float deltaTime) {
    UpdateAnimation(deltaTime);
    
    // Update power-up timer
    if (m_powerUpTimer > 0) {
        m_powerUpTimer -= deltaTime;
        if (m_powerUpTimer <= 0) {
            m_isPoweredUp = false;
        }
    }
    
    // Update invulnerability timer
    if (m_invulnerabilityTimer > 0) {
        m_invulnerabilityTimer -= deltaTime;
    }
    
    // Update jump buffer (allows jump input slightly before landing)
    if (m_jumpBuffer > 0) {
        m_jumpBuffer -= deltaTime;
    }
    
    // Update coyote time (allows jump slightly after leaving ground)
    if (m_isOnGround) {
        m_coyoteTime = 0.1f; // 100ms coyote time
    } else if (m_coyoteTime > 0) {
        m_coyoteTime -= deltaTime;
    }
    
    // Apply gravity
    if (!m_isOnGround) {
        m_velocity.y += 800.0f * deltaTime; // Gravity constant from Game class
        m_velocity.y = std::min(m_velocity.y, MAX_FALL_SPEED);
    }
    
    // Update position
    m_position = m_position + m_velocity * deltaTime;
}

void Player::Render(SDL_Renderer* renderer) {
    if (m_isPoweredUp) {
        RenderPoweredUp(renderer);
    } else {
        RenderNormal(renderer);
    }
}

void Player::RenderNormal(SDL_Renderer* renderer) {
    // Flashing effect when invulnerable
    if (m_invulnerabilityTimer > 0) {
        int flashRate = static_cast<int>(m_invulnerabilityTimer * 10) % 2;
        if (flashRate == 0) return; // Skip rendering to create flashing effect
    }
    
    // Draw player as a red rectangle with animation
    float bounceOffset = 0;
    if (m_jumpAnimTimer > 0) {
        bounceOffset = sin(m_jumpAnimTimer * 10) * 3;
    }
    
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect playerRect = {
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y + bounceOffset),
        static_cast<int>(m_width),
        static_cast<int>(m_height)
    };
    SDL_RenderFillRect(renderer, &playerRect);
    
    // Draw walking animation (slight width variation)
    if (m_isMoving && m_isOnGround) {
        float walkOffset = sin(m_walkAnimTimer * 8) * 2;
        SDL_Rect walkRect = {
            static_cast<int>(m_position.x - walkOffset/2),
            static_cast<int>(m_position.y + m_height - 5),
            static_cast<int>(m_width + walkOffset),
            5
        };
        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        SDL_RenderFillRect(renderer, &walkRect);
    }
    
    // Draw eyes to show direction
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int eyeSize = 4;
    int eyeY = static_cast<int>(m_position.y + 8 + bounceOffset);
    
    if (m_facingRight) {
        SDL_Rect rightEye = {static_cast<int>(m_position.x + 18), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &rightEye);
    } else {
        SDL_Rect leftEye = {static_cast<int>(m_position.x + 8), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &leftEye);
    }
    
    // Draw pupils
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int pupilSize = 2;
    if (m_facingRight) {
        SDL_Rect pupil = {static_cast<int>(m_position.x + 19), eyeY + 1, pupilSize, pupilSize};
        SDL_RenderFillRect(renderer, &pupil);
    } else {
        SDL_Rect pupil = {static_cast<int>(m_position.x + 9), eyeY + 1, pupilSize, pupilSize};
        SDL_RenderFillRect(renderer, &pupil);
    }
}

void Player::RenderPoweredUp(SDL_Renderer* renderer) {
    // Flashing effect when invulnerable
    if (m_invulnerabilityTimer > 0) {
        int flashRate = static_cast<int>(m_invulnerabilityTimer * 10) % 2;
        if (flashRate == 0) return;
    }
    
    float scaledWidth = m_width * POWERED_UP_SCALE;
    float scaledHeight = m_height * POWERED_UP_SCALE;
    float offsetX = (scaledWidth - m_width) * 0.5f;
    float offsetY = scaledHeight - m_height;
    
    // Power-up glow effect
    float pulse = (sin(m_powerUpTimer * 5) + 1.0f) * 0.5f;
    
    // Draw larger Mario with gradient effect
    SDL_SetRenderDrawColor(renderer, 255, static_cast<Uint8>(100 + pulse * 100), 0, 255);
    SDL_Rect powerRect = {
        static_cast<int>(m_position.x - offsetX),
        static_cast<int>(m_position.y - offsetY),
        static_cast<int>(scaledWidth),
        static_cast<int>(scaledHeight)
    };
    SDL_RenderFillRect(renderer, &powerRect);
    
    // Draw original size on top for layered effect
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 200);
    SDL_Rect normalRect = {
        static_cast<int>(m_position.x),
        static_cast<int>(m_position.y),
        static_cast<int>(m_width),
        static_cast<int>(m_height)
    };
    SDL_RenderFillRect(renderer, &normalRect);
    
    // Enhanced eyes
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int eyeSize = 6;
    int eyeY = static_cast<int>(m_position.y + 8);
    
    if (m_facingRight) {
        SDL_Rect rightEye = {static_cast<int>(m_position.x + 20), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &rightEye);
    } else {
        SDL_Rect leftEye = {static_cast<int>(m_position.x + 4), eyeY, eyeSize, eyeSize};
        SDL_RenderFillRect(renderer, &leftEye);
    }
    
    // Power indicator "S" on chest
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    int centerX = static_cast<int>(m_position.x + m_width/2);
    int centerY = static_cast<int>(m_position.y + m_height/2);
    SDL_Rect sTop = {centerX - 3, centerY - 4, 6, 2};
    SDL_Rect sMiddle = {centerX - 3, centerY - 1, 6, 2};
    SDL_Rect sBottom = {centerX - 3, centerY + 2, 6, 2};
    SDL_RenderFillRect(renderer, &sTop);
    SDL_RenderFillRect(renderer, &sMiddle);
    SDL_RenderFillRect(renderer, &sBottom);
}

void Player::UpdateAnimation(float deltaTime) {
    // Update walk animation
    if (m_isMoving) {
        m_walkAnimTimer += deltaTime;
    } else {
        m_walkAnimTimer = 0;
    }
    
    // Update jump animation
    if (m_jumpAnimTimer > 0) {
        m_jumpAnimTimer -= deltaTime;
    }
}

void Player::HandleInput(const bool* keystate, const bool* keysPressed) {
    m_isMoving = false;
    
    // Horizontal movement
    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) {
        float speed = m_isPoweredUp ? MOVE_SPEED * 1.3f : MOVE_SPEED;
        m_velocity.x = -speed;
        m_facingRight = false;
        m_isMoving = true;
    } else if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) {
        float speed = m_isPoweredUp ? MOVE_SPEED * 1.3f : MOVE_SPEED;
        m_velocity.x = speed;
        m_facingRight = true;
        m_isMoving = true;
    } else {
        m_velocity.x = 0;
    }
    
    // Jumping - use only continuous keystate for more reliable detection
    bool jumpKeyCurrentlyHeld = (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_SPACE]);
    
    // Jump when key is newly pressed (wasn't held last frame, but is held now)
    bool shouldJump = jumpKeyCurrentlyHeld && !m_wasJumpKeyHeld && m_isOnGround;
    
    if (shouldJump) {
        float jumpForce = m_isPoweredUp ? JUMP_FORCE * 1.2f : JUMP_FORCE;
        m_velocity.y = jumpForce;
        m_isOnGround = false;
        PlayJumpAnimation();
    }
    
    // Update jump key state for next frame
    m_wasJumpKeyHeld = jumpKeyCurrentlyHeld;
}

void Player::HandleInputSimple(const Uint8* keystate) {
    m_isMoving = false;
    
    // Horizontal movement
    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A]) {
        float speed = m_isPoweredUp ? MOVE_SPEED * 1.3f : MOVE_SPEED;
        m_velocity.x = -speed;
        m_facingRight = false;
        m_isMoving = true;
    } else if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]) {
        float speed = m_isPoweredUp ? MOVE_SPEED * 1.3f : MOVE_SPEED;
        m_velocity.x = speed;
        m_facingRight = true;
        m_isMoving = true;
    } else {
        m_velocity.x = 0;
    }
    
    // Jumping with buffer system - much more forgiving
    bool jumpKeyHeld = (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_SPACE]);
    
    // Set jump buffer when any jump key is pressed
    if (jumpKeyHeld && !m_wasJumpKeyHeld) {
        m_jumpBuffer = 0.15f; // 150ms jump buffer
    }
    
    // Attempt jump if we have jump buffer and can jump (on ground or coyote time)
    if (m_jumpBuffer > 0 && (m_isOnGround || m_coyoteTime > 0)) {
        float jumpForce = m_isPoweredUp ? JUMP_FORCE * 1.2f : JUMP_FORCE;
        m_velocity.y = jumpForce;
        m_isOnGround = false;
        m_jumpBuffer = 0; // Consume the jump buffer
        m_coyoteTime = 0; // Consume coyote time
        PlayJumpAnimation();
    }
    
    // Update jump key state
    m_wasJumpKeyHeld = jumpKeyHeld;
}

Rectangle Player::GetBounds() const {
    if (m_isPoweredUp) {
        float scaledWidth = m_width * POWERED_UP_SCALE;
        float scaledHeight = m_height * POWERED_UP_SCALE;
        float offsetX = (scaledWidth - m_width) * 0.5f;
        float offsetY = scaledHeight - m_height;
        return Rectangle(m_position.x - offsetX, m_position.y - offsetY, scaledWidth, scaledHeight);
    }
    return Rectangle(m_position.x, m_position.y, m_width, m_height);
}

void Player::ResolveCollision(const Rectangle& platform) {
    Rectangle playerBounds = GetBounds();
    
    // Calculate overlap
    float overlapLeft = (playerBounds.x + playerBounds.width) - platform.x;
    float overlapRight = (platform.x + platform.width) - playerBounds.x;
    float overlapTop = (playerBounds.y + playerBounds.height) - platform.y;
    float overlapBottom = (platform.y + platform.height) - playerBounds.y;
    
    // Find minimum overlap
    float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});
    
    if (minOverlap == overlapTop && m_velocity.y >= 0) {
        // Landing on top of platform
        if (m_isPoweredUp) {
            m_position.y = platform.y - (m_height * POWERED_UP_SCALE);
        } else {
            m_position.y = platform.y - m_height;
        }
        m_velocity.y = 0;
        m_isOnGround = true;
    } else if (minOverlap == overlapBottom && m_velocity.y < 0) {
        // Hitting platform from below
        m_position.y = platform.y + platform.height;
        m_velocity.y = 0;
    } else if (minOverlap == overlapLeft && m_velocity.x > 0) {
        // Hitting platform from left
        if (m_isPoweredUp) {
            float offsetX = (m_width * POWERED_UP_SCALE - m_width) * 0.5f;
            m_position.x = platform.x - m_width - offsetX;
        } else {
            m_position.x = platform.x - m_width;
        }
        m_velocity.x = 0;
    } else if (minOverlap == overlapRight && m_velocity.x < 0) {
        // Hitting platform from right
        if (m_isPoweredUp) {
            float offsetX = (m_width * POWERED_UP_SCALE - m_width) * 0.5f;
            m_position.x = platform.x + platform.width - offsetX;
        } else {
            m_position.x = platform.x + platform.width;
        }
        m_velocity.x = 0;
    }
} 