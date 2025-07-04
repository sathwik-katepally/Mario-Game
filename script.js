// Game canvas setup
const canvas = document.getElementById("gameCanvas");
const ctx = canvas.getContext("2d");

// Set canvas size
canvas.width = 800;
canvas.height = 400;

// Game state
const gameState = {
    player: {
        x: 50,
        y: canvas.height - 50,
        width: 30,
        height: 30,
        velocityX: 0,
        velocityY: 0,
        isJumping: false
    }
};

// Game constants
const GRAVITY = 0.5;
const JUMP_FORCE = -12;
const MOVEMENT_SPEED = 5;

// Event listeners for controls
document.addEventListener("keydown", handleKeyDown);
document.addEventListener("keyup", handleKeyUp);

function handleKeyDown(e) {
    switch(e.key) {
        case "ArrowLeft":
            gameState.player.velocityX = -MOVEMENT_SPEED;
            break;
        case "ArrowRight":
            gameState.player.velocityX = MOVEMENT_SPEED;
            break;
        case "ArrowUp":
        case " ":
            if (!gameState.player.isJumping) {
                gameState.player.velocityY = JUMP_FORCE;
                gameState.player.isJumping = true;
            }
            break;
    }
}

function handleKeyUp(e) {
    switch(e.key) {
        case "ArrowLeft":
        case "ArrowRight":
            gameState.player.velocityX = 0;
            break;
    }
}

// Game loop
function gameLoop() {
    // Update
    updatePlayer();
    
    // Draw
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawPlayer();
    
    // Next frame
    requestAnimationFrame(gameLoop);
}

function updatePlayer() {
    // Apply gravity
    gameState.player.velocityY += GRAVITY;
    
    // Update position
    gameState.player.x += gameState.player.velocityX;
    gameState.player.y += gameState.player.velocityY;
    
    // Ground collision
    if (gameState.player.y > canvas.height - gameState.player.height) {
        gameState.player.y = canvas.height - gameState.player.height;
        gameState.player.velocityY = 0;
        gameState.player.isJumping = false;
    }
    
    // Wall collision
    if (gameState.player.x < 0) gameState.player.x = 0;
    if (gameState.player.x > canvas.width - gameState.player.width) {
        gameState.player.x = canvas.width - gameState.player.width;
    }
}

function drawPlayer() {
    ctx.fillStyle = "red";
    ctx.fillRect(
        gameState.player.x,
        gameState.player.y,
        gameState.player.width,
        gameState.player.height
    );
}

// Start the game
gameLoop();
