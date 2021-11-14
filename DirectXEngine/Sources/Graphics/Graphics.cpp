#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height) {

    if (!initializeRenderer(hwnd, width, height)) {
        return false;
    }

    imgui = ImGUIWInstance::getPInstance(hwnd, getGraphicsState());

    if (!initializeScene()) {
        return false;
    }
    initSprites();

    fpsTimer.startTimer();
    return true;
}

void Graphics::renderFrame() {

    renderer->preparePipeline();
    renderScene.render();

    //Draw text
    static int fpsCounter = 0;
    fpsCounter += 1;
    static std::wstring fpsString = L"FPS: 0";
    if (fpsTimer.getMillisecondsElapsed() > 1000.0f) {
        fpsString = L"FPS: " + std::to_wstring(fpsCounter);
        fpsCounter = 0;
        fpsTimer.restartTimer(); 
    }
    spriteBatch->Begin(); 
    spriteFont->DrawString(spriteBatch.get(), fpsString.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
    spriteBatch->End();

    const UINT vsync = 0;
    renderer->present(vsync, 0);
}

void Graphics::update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) {
    renderScene.update(kbd, mouse, dt);
}

bool Graphics::initializeRenderer(HWND hwnd, int width, int height) {
    return renderer->initRenderer(hwnd, width, height);
}

bool Graphics::initializeScene() {
    return renderScene.initialize(getGraphicsState());
}

void Graphics::initSprites() {
    GraphicsState& gs = getGraphicsState();
    spriteBatch = std::make_unique<DirectX::SpriteBatch>(gs.deviceContext);
    spriteFont = std::make_unique<DirectX::SpriteFont>(gs.device, L"Resources\\Fonts\\comic_sans_ms_16.spritefont");
}

GraphicsState& Graphics::getGraphicsState() const {
    return renderer->getGraphicsState();
}
