#pragma once

#include <memory>
#include <numbers>
#include <SpriteBatch.h>
#include <SpriteFont.h>

#include "Scene.h"
#include "../Graphics/Camera3D.h"
#include "../Graphics/ImGUIWrapper.h"
#include "../Graphics/Light.h"
#include "../Graphics/RenderableGameObject.h"
#include "../Utils/DXUtils.h"

namespace App {

    class PongScene final: public Scene {
    public:
        PongScene() = default;
        ~PongScene() = default;
        bool initialize(GraphicsState& graphicsState, GraphicsSettings& graphicsSettings) override;
        void reset() override;
        const XMMATRIX& getViewMatrix() const override;
        const XMMATRIX& getProjectionMatrix() const override;
        const LightInfo& getLightInfo() const override;
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
        LightInfo light;

        float cameraSpeed = 0.1f;

        ImGUIW* imgui = nullptr;


        const XMFLOAT3 leftBorderPos{ -75, 2, 0 };
        const XMFLOAT3 rightBorderPos{ 75, 2, 0 };

        const float tableWidth = rightBorderPos.x - leftBorderPos.x;
        const float tableLength = 280;
        const XMFLOAT3 tablePos = { 0, 0, 0 };

        float borderWidth = 1;
        float borderLength = tableLength;
        float borderHeight = 5;

        const float padWidth = 16;
        const float padHeight = 18;
        const float padLength = 2;
        const XMFLOAT3 DefaultPlayerPos = { 0, -15,  -tableLength / 2 };
        const XMFLOAT3 DefaultAIPos = { 0, -15, tableLength / 2 };
        const XMFLOAT3 AIToPlayerDirection = DefaultPlayerPos - DefaultAIPos;
        const XMFLOAT3 PlayerToAIDirection = DefaultAIPos - DefaultPlayerPos;

        XMFLOAT3 PlayerPos{};
        XMFLOAT3 AIPos{};
        float AITargetPosX = DefaultAIPos.x;

        const float ballRadius = 2;
        const float ballRadWidth = 2.125f * ballRadius;
        const float defaultBallSpeed = 0.0005;
        const float maximumBallSpeed = defaultBallSpeed * 4;
        const float deltaBallSpeed = defaultBallSpeed / 10;
        XMFLOAT3 ballPosition{};
        XMFLOAT3 ballDirection{};


        const float maxXDiff = padWidth / 2 + ballRadWidth;

        float playerSpeed{};
        float AISpeed{};
        float ballSpeed{};
        const float defaultPadSpeed = 0.15;

        const float PI = std::numbers::pi_v<float>;

        int windowWidth = 0;
        int windowHeight = 0;

        struct GameState {
            enum STATES { PLAYER = 0, AI = 1, NONE = 2};
            int AIScore = 0;
            int playerScore = 0;
            STATES ballside = PLAYER;
        } gs;

        const float AIWaitTime = 1000.0f; // time (millis) to wait after goal before pushing the ball
        Timer AIDelayTimer;
        bool free_camera = false;
    };
}
