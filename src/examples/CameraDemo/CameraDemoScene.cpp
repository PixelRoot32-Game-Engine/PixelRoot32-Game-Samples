#include "CameraDemoScene.h"
#include "core/Engine.h"
#include "Config.h"
#include "input/InputManager.h"
#include "graphics/Renderer.h"
#include "GameConstants.h"
#include "PlayerCube.h"

namespace pr32 = pixelroot32;

extern pr32::core::Engine engine;

namespace camerademo {

using pixelroot32::graphics::TileMap;
using pixelroot32::graphics::Color;

static PlatformRect gPlatforms[PLATFORM_COUNT];

static const uint16_t TILE_EMPTY_BITS[] = {
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000,
    0x0000
};

static const uint16_t TILE_GROUND_BITS[] = {
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF
};

static const uint16_t TILE_PLATFORM_BITS[] = {
    0x0000,
    0x0000,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0xFFFF,
    0x0000,
    0x0000
};

static const pixelroot32::graphics::Sprite PLATFORMER_TILES[] = {
    { TILE_EMPTY_BITS,     TILE_SIZE, TILE_SIZE },
    { TILE_GROUND_BITS,    TILE_SIZE, TILE_SIZE },
    { TILE_PLATFORM_BITS,  TILE_SIZE, TILE_SIZE }
};

static uint8_t PLATFORMER_INDICES[TILEMAP_WIDTH * TILEMAP_HEIGHT];

static TileMap PLATFORMER_MAP = {
    PLATFORMER_INDICES,
    static_cast<uint8_t>(TILEMAP_WIDTH),
    static_cast<uint8_t>(TILEMAP_HEIGHT),
    PLATFORMER_TILES,
    TILE_SIZE,
    TILE_SIZE,
    static_cast<uint16_t>(sizeof(PLATFORMER_TILES) / sizeof(pixelroot32::graphics::Sprite))
};

// Build the scrolling platformer tilemap and precompute platform collision rectangles.
static void initPlatformerTilemap() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    initialized = true;

    int total = TILEMAP_WIDTH * TILEMAP_HEIGHT;
    for (int i = 0; i < total; ++i) {
        PLATFORMER_INDICES[i] = 0;
    }

    int groundRow1 = TILEMAP_HEIGHT - 2;
    int groundRow2 = TILEMAP_HEIGHT - 1;

    for (int x = 0; x < TILEMAP_WIDTH; ++x) {
        PLATFORMER_INDICES[groundRow1 * TILEMAP_WIDTH + x] = 1;
        PLATFORMER_INDICES[groundRow2 * TILEMAP_WIDTH + x] = 1;
    }

    int pIndex = 0;

    int p1Start = 10;
    int p1End = 18;
    int p1Row = groundRow1 - 3;
    for (int x = p1Start; x < p1End; ++x) {
        PLATFORMER_INDICES[p1Row * TILEMAP_WIDTH + x] = 2;
    }
    gPlatforms[pIndex].x = static_cast<float>(p1Start * TILE_SIZE);
    gPlatforms[pIndex].y = static_cast<float>(p1Row * TILE_SIZE + PLATFORM_VISUAL_OFFSET);
    gPlatforms[pIndex].w = static_cast<float>((p1End - p1Start) * TILE_SIZE);
    gPlatforms[pIndex].h = static_cast<float>(TILE_SIZE);
    pIndex++;

    int p2Start = 28;
    int p2End = 36;
    int p2Row = groundRow1 - 5;
    for (int x = p2Start; x < p2End; ++x) {
        PLATFORMER_INDICES[p2Row * TILEMAP_WIDTH + x] = 2;
    }
    gPlatforms[pIndex].x = static_cast<float>(p2Start * TILE_SIZE);
    gPlatforms[pIndex].y = static_cast<float>(p2Row * TILE_SIZE + PLATFORM_VISUAL_OFFSET);
    gPlatforms[pIndex].w = static_cast<float>((p2End - p2Start) * TILE_SIZE);
    gPlatforms[pIndex].h = static_cast<float>(TILE_SIZE);
    pIndex++;

    int p3Start = 50;
    int p3End = 58;
    int p3Row = groundRow1 - 4;
    for (int x = p3Start; x < p3End; ++x) {
        PLATFORMER_INDICES[p3Row * TILEMAP_WIDTH + x] = 2;
    }
    gPlatforms[pIndex].x = static_cast<float>(p3Start * TILE_SIZE);
    gPlatforms[pIndex].y = static_cast<float>(p3Row * TILE_SIZE + PLATFORM_VISUAL_OFFSET);
    gPlatforms[pIndex].w = static_cast<float>((p3End - p3Start) * TILE_SIZE);
    gPlatforms[pIndex].h = static_cast<float>(TILE_SIZE);
}

static PlayerCube* gPlayer = nullptr;

CameraDemoScene::CameraDemoScene()
    : camera(DISPLAY_WIDTH, DISPLAY_HEIGHT)
    , levelWidth(static_cast<float>(TILEMAP_WIDTH * TILE_SIZE)) {
}

CameraDemoScene::~CameraDemoScene() {
    if (gPlayer) {
        delete gPlayer;
        gPlayer = nullptr;
    }
}

// Initialize tilemap, create the player cube, and configure camera world bounds.
void CameraDemoScene::init() {
    pr32::graphics::setPalette(pr32::graphics::PaletteType::PR32);
    initPlatformerTilemap();
    jumpInputReady = false;

    if (gPlayer) {
        delete gPlayer;
        gPlayer = nullptr;
    }

    float startX = PLAYER_START_X;
    float playerWidth = PLAYER_WIDTH;
    float playerHeight = PLAYER_HEIGHT;
    float startY = PLAYER_START_Y;

    gPlayer = new PlayerCube(startX,
                             startY,
                             playerWidth,
                             playerHeight);
    int worldWidthPixels = TILEMAP_WIDTH * TILE_SIZE;
    int worldHeightPixels = (TILEMAP_HEIGHT - 2) * TILE_SIZE;
    gPlayer->setWorldSize(worldWidthPixels, worldHeightPixels);

    float maxCameraX = levelWidth - DISPLAY_WIDTH;
    if (maxCameraX < 0.0f) {
        maxCameraX = 0.0f;
    }
    camera.setBounds(0.0f, maxCameraX);
    camera.setVerticalBounds(0.0f, 0.0f);
    camera.setPosition(0.0f, 0.0f);
}

// Read input, update the player cube, and move the camera to follow it.
void CameraDemoScene::update(unsigned long deltaTime) {
    auto& input = engine.getInputManager();

    float moveDir = 0.0f;
    if (input.isButtonDown(3)) {
        moveDir += 1.0f;
    }
    if (input.isButtonDown(2)) {
        moveDir -= 1.0f;
    }

    bool rawJumpDown = input.isButtonDown(4);

    if (!jumpInputReady) {
        if (!rawJumpDown) {
            jumpInputReady = true;
        }
    }

    bool jumpPressed = false;
    if (jumpInputReady && input.isButtonPressed(4)) {
        jumpPressed = true;
    }

    if (gPlayer) {
        gPlayer->setInput(moveDir, jumpPressed);
        gPlayer->update(deltaTime, gPlatforms, PLATFORM_COUNT);

        float centerX = gPlayer->x + gPlayer->width * 0.5f;
        float centerY = gPlayer->y + gPlayer->height * 0.5f;
        camera.followTarget(centerX, centerY);
    }
}

// Render parallax background layers, tilemap world, and player based on camera X.
void CameraDemoScene::draw(pr32::graphics::Renderer& renderer) {
    float camX = camera.getX();

    float farFactor = 0.4f;
    int farOffset = static_cast<int>(-camX * farFactor);
    renderer.setDisplayOffset(farOffset, 0);

    int horizonY = DISPLAY_HEIGHT / 3;
    int hillHeight = DISPLAY_HEIGHT / 4;

    renderer.drawFilledRectangle(-40, horizonY, DISPLAY_WIDTH + 80, hillHeight, Color::DarkBlue);
    renderer.drawFilledRectangle(DISPLAY_WIDTH / 2, horizonY + 10, DISPLAY_WIDTH, hillHeight + 10, Color::DarkGray);

    int midOffset = static_cast<int>(-camX * 0.7f);
    renderer.setDisplayOffset(midOffset, 0);

    int midY = (DISPLAY_HEIGHT * 2) / 3;
    renderer.drawFilledRectangle(-20, midY, DISPLAY_WIDTH + 40, 10, Color::DarkGreen);

    int mainOffset = static_cast<int>(-camX);
    renderer.setDisplayOffset(mainOffset, 0);

    renderer.drawTileMap(PLATFORMER_MAP, 0, 0, Color::Brown);

    if (gPlayer) {
        gPlayer->draw(renderer);
    }
}

}

