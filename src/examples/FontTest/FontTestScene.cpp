#include "FontTestScene.h"
#include "core/Engine.h"
#include "graphics/Color.h"
#include "graphics/Renderer.h"
#include "graphics/FontManager.h"
#include "graphics/Font5x7.h"
#include <cstdio>

namespace pr32 = pixelroot32;
using Color = pr32::graphics::Color;

extern pr32::core::Engine engine;

namespace fonttest {

class FontTestScene::TestBackground : public pr32::core::Entity {
public:
    TestBackground()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(0);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        // Fill background
        renderer.drawFilledRectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, Color::Black);
    }
};

class FontTestScene::TestText : public pr32::core::Entity {
public:
    TestText()
        : pr32::core::Entity(0.0f, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, pr32::core::EntityType::GENERIC) {
        setRenderLayer(1);
    }

    void update(unsigned long) override {
    }

    void draw(pr32::graphics::Renderer& renderer) override {
        int y = 10;
        
        // Title
        renderer.drawTextCentered("FONT TEST", y, Color::White, 2);
        y += 25;
        
        // Test 1: Different sizes
        renderer.drawText("Size 1:", 10, y, Color::Cyan, 1);
        renderer.drawText("Hello World!", 70, y, Color::White, 1);
        y += 15;
        
        renderer.drawText("Size 2:", 10, y, Color::Cyan, 1);
        renderer.drawText("Hello World!", 70, y, Color::White, 2);
        y += 25;
        
        renderer.drawText("Size 3:", 10, y, Color::Cyan, 1);
        renderer.drawText("Hello World!", 70, y, Color::White, 3);
        y += 35;
        
        // Test 2: Centered text
        renderer.drawTextCentered("CENTERED TEXT", y, Color::Yellow, 1);
        y += 20;
        renderer.drawTextCentered("SIZE 2 CENTERED", y, Color::Yellow, 2);
        y += 25;
        
        // Test 3: All ASCII characters (in rows)
        renderer.drawText("ASCII 32-63:", 10, y, Color::Green, 1);
        y += 12;
        char asciiRow1[33] = {0};
        for (int i = 32; i <= 63; i++) {
            asciiRow1[i - 32] = static_cast<char>(i);
        }
        asciiRow1[32] = '\0';
        renderer.drawText(asciiRow1, 10, y, Color::White, 1);
        y += 12;
        
        renderer.drawText("ASCII 64-95:", 10, y, Color::Green, 1);
        y += 12;
        char asciiRow2[33] = {0};
        for (int i = 64; i <= 95; i++) {
            asciiRow2[i - 64] = static_cast<char>(i);
        }
        asciiRow2[32] = '\0';
        renderer.drawText(asciiRow2, 10, y, Color::White, 1);
        y += 12;
        
        renderer.drawText("ASCII 96-126:", 10, y, Color::Green, 1);
        y += 12;
        char asciiRow3[32] = {0};
        for (int i = 96; i <= 126; i++) {
            asciiRow3[i - 96] = static_cast<char>(i);
        }
        asciiRow3[31] = '\0';
        renderer.drawText(asciiRow3, 10, y, Color::White, 1);
        y += 20;
        
        // Test 4: Edge cases
        renderer.drawText("Edge Cases:", 10, y, Color::Red, 1);
        y += 12;
        
        // Empty string (should not crash)
        renderer.drawText("", 10, y, Color::White, 1);
        renderer.drawText("(empty)", 10, y, Color::Gray, 1);
        y += 12;
        
        // Long text
        renderer.drawText("Long: This is a very long text string that should wrap or be clipped", 10, y, Color::White, 1);
        y += 12;
        
        // Special characters (some may be unsupported)
        renderer.drawText("Special: !@#$%^&*()", 10, y, Color::White, 1);
        y += 12;
        
        // Numbers
        renderer.drawText("Numbers: 0123456789", 10, y, Color::White, 1);
        y += 12;
        
        // Mixed case
        renderer.drawText("Mixed: Hello WORLD 123!", 10, y, Color::White, 1);
        y += 20;
        
        // Test 5: Font info
        const pr32::graphics::Font* font = pr32::graphics::FontManager::getDefaultFont();
        if (font) {
            char info[64];
            snprintf(info, sizeof(info), "Font: %dx%d, Chars: %d-%d", 
                     font->glyphWidth, font->glyphHeight, 
                     font->firstChar, font->lastChar);
            renderer.drawText(info, 10, y, Color::Cyan, 1);
        }
    }
};

FontTestScene::FontTestScene()
    : background(nullptr), testText(nullptr) {
    background = new TestBackground();
    testText = new TestText();
    
    addEntity(background);
    addEntity(testText);
}

FontTestScene::~FontTestScene() {
    if (background) {
        removeEntity(background);
        delete background;
        background = nullptr;
    }
    if (testText) {
        removeEntity(testText);
        delete testText;
        testText = nullptr;
    }
}

void FontTestScene::init() {
    // Font is already set as default in Engine::init()
    // Verify it's set correctly
    const pr32::graphics::Font* defaultFont = pr32::graphics::FontManager::getDefaultFont();
    if (!defaultFont) {
        // Fallback: set it explicitly
        pr32::graphics::FontManager::setDefaultFont(&pr32::graphics::FONT_5X7);
    }
}

void FontTestScene::update(unsigned long deltaTime) {
    Scene::update(deltaTime);
}

void FontTestScene::draw(pr32::graphics::Renderer& renderer) {
    Scene::draw(renderer);
}

}
