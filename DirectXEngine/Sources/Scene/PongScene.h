#pragma once

#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <memory>
#include <numbers>

#include "Scene.h"
#include "../Graphics/Model.h"
#include "../Graphics/RenderableGameObject.h"
#include "../Graphics/Camera3D.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/Light.h"
#include "../Graphics/ImGUIWrapper.h"
#include "../Utils/DXUtils.h"

namespace App {

    using namespace NGameObject;

    class PongScene final: public Scene {
    public:
        PongScene() = default;
        ~PongScene() = default;
        bool initialize(GraphicsState& graphicsState) override;
        void reset() override;
        const XMMATRIX& getViewMatrix() const override;
        const XMMATRIX& getProjectionMatrix() const override;
        const Light& getLightInfo() const override;
        const Camera3D& getCameraInfo() const override;
    private:
        void pushBall();
        void updateAI(float dt);
        void checkCollision();
        void updateInput(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) override;
        void updateGUI() override;
        void updateGameObjects() override;

    private:

        std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
        std::unique_ptr<DirectX::SpriteFont> spriteFont;

        RenderableGameObject leftBorder;
        RenderableGameObject rightBorder;
        RenderableGameObject table;
        RenderableGameObject AIPad;
        RenderableGameObject playerPad;
        RenderableGameObject ball;
        Light light;
        Camera3D camera;
        float cameraSpeed = 0.1f;

        GraphicsState* graphicsState = nullptr;
        ImGUIW* imgui;

        const float tableWidth = 100;
        const float tableLength = 200;
        const float tableHeight = 1;
        const XMFLOAT3 tablePos = { 0, 0, 0 };

        const float borderWidth = 1;
        const float borderLength = tableLength;
        const float borderHeight = 5;
        XMFLOAT3 leftBorderPos{};
        XMFLOAT3 rightBorderPos{};

        const float padWidth = 20;
        const float padHeight = 10;
        const float padLength = 2;

        const XMFLOAT3 DefaultPlayerPos = { 0, 5,  -tableLength / 2 };
        const XMFLOAT3 DefaultAIPos = { 0, 5, tableLength / 2 };
        const XMFLOAT3 AIToPlayerDirection = DefaultPlayerPos - DefaultAIPos;
        const XMFLOAT3 PlayerToAIDirection = DefaultAIPos - DefaultPlayerPos;

        XMFLOAT3 PlayerPos{};
        XMFLOAT3 AIPos{};

        const float ballRadius = 1;
        XMFLOAT3 ballPosition{};
        XMFLOAT3 ballDirection{};

        float playerSpeed{};
        float AISpeed{};
        float ballSpeed{};

        const float PI = std::numbers::pi_v<float>;

        int windowWidth = 0;
        int windowHeight = 0;

        struct GameState {
            enum { PLAYER = 0, AI = 1};
            int AIScore = 0;
            int playerScore = 0;
            bool ballside = PLAYER;
        } gs;

    };
}
