#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h> 
#include "AdapterReader.h"
#include "Sprite.h"
#include "GraphicsState.h"
#include "../Timer.h"
#include "../Scene/PongScene.h"
#include "ImGUIWrapper.h"
#include "Render/ForwardRenderer.h"
#include "Render/DeferredRenderer.h"

class Graphics {
public:
    bool initialize(HWND hwnd, int width, int height);
    void renderFrame();
    void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
    void postRender();
    void present();
    GraphicsState& getGraphicsState() const;

private:
    bool initializeRenderer(HWND hwnd, int width, int height);
    bool initializeScene();
    void initSprites();

    std::unique_ptr<Renderer> renderer{ new ForwardRenderer{} };
    //std::unique_ptr<Renderer> renderer{ new DeferredRenderer{} };
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    Timer fpsTimer;
    App::PongScene renderScene;
    ImGUIW *imgui;
};
