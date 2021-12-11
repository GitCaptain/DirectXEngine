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
    const float bgcolor[4] = { 0.f, 0.f, 0.f, 1.f };
    renderer->renderScene(renderScene.get(), &gSettings, bgcolor);
}

void Graphics::update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) {
    renderScene->update(kbd, mouse, dt);
}

void Graphics::postRender() {
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
    imgui->startFrame();

    imgui->newWindow("graphics settings")
        .attach<IMGUIFN::DRAGFLOAT>("fov", &gSettings.fov, 1.f, 1.f, 180.f)
        .attach<IMGUIFN::DRAGFLOAT>("nearZ", &gSettings.nearZ, 0.1f, 0.1f, 10.0f)
        .attach<IMGUIFN::DRAGFLOAT>("farZ", &gSettings.farZ, 100.f, 100.f, 5000.f)
        .attach<IMGUIFN::DRAGFLOAT>("gamma", &gSettings.gammaCoef, 0.1f, 0.1f, 3.0f)
        .attach<IMGUIFN::DRAGFLOAT>("exposure", &gSettings.exposure, 0.1f, 0.0f, 5.0f)
        .attach<IMGUIFN::CHECKBOX>("hdr", &gSettings.hdr)
        .end();

    renderScene->postRender();
    imgui->endFrame();
}

void Graphics::present() {
    const UINT vsync = 0;
    renderer->present(vsync, 0);
}

bool Graphics::initializeRenderer(HWND hwnd, int width, int height) {
    return renderer->initRenderer(hwnd, width, height);
}

bool Graphics::initializeScene() {
    return renderScene->initialize(getGraphicsState(), gSettings);
}

void Graphics::initSprites() {
    GraphicsState& gs = getGraphicsState();
    spriteBatch = std::make_unique<DirectX::SpriteBatch>(gs.deviceContext);
    spriteFont = std::make_unique<DirectX::SpriteFont>(gs.device, L"Resources\\Fonts\\comic_sans_ms_16.spritefont");
}

GraphicsState& Graphics::getGraphicsState() const {
    return renderer->getGraphicsState();
}
