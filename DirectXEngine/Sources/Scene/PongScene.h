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

    class PongScene : public Scene {
    public:
        PongScene() = default;
        ~PongScene() = default;
        virtual bool initialize(GraphicsState& graphicsState) override;
        virtual void render() override;
        virtual void reset() override;
        virtual void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);

    private:
        bool initializeConstantBuffers();
        void pushBall();
        void updateAI(float dt);
        void checkCollision();

    private:
        ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
        ConstantBuffer<CB_PS_Phonglight> cb_ps_phonglight;
        ConstantBuffer<CB_PS_Camera> cb_ps_camera;

        std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
        std::unique_ptr<DirectX::SpriteFont> spriteFont;

        RenderableGameObject border;
        RenderableGameObject ball;
        Camera3D camera;
        float cameraSpeed = 0.1f;
        Light light;

        GraphicsState* graphicsState = nullptr;
        ImGUIW* imgui;

        const float tableWidth = 100;
        const float tableLength = 200;
        const float tableHeight = 1;
        const XMFLOAT3 tablePos = { 0, 0, 0 };

        const float borderWidth = 1;
        const float borderLength = tableLength;
        const float borderHeight = 5;
        XMFLOAT3 leftBorderPos;
        XMFLOAT3 rightBorderPos;

        const float padWidth = 20;
        const float padHeight = 10;
        const float padLength = 2;

        const XMFLOAT3 DefaultPlayerPos = { 0, 5,  -tableLength / 2 };
        const XMFLOAT3 DefaultAIPos = { 0, 5, tableLength / 2 };
        const XMFLOAT3 AIToPlayerDirection = DefaultPlayerPos - DefaultAIPos;
        const XMFLOAT3 PlayerToAIDirection = DefaultAIPos - DefaultPlayerPos;

        XMFLOAT3 PlayerPos;
        XMFLOAT3 AIPos;

        const float ballRadius = 1;
        XMFLOAT3 ballPosition;
        XMFLOAT3 ballDirection;

        float playerSpeed;
        float AISpeed;
        float ballSpeed;

        const float PI = std::numbers::pi_v<float>;

        int windowWidth = 0;
        int windowHeight = 0;

        struct GameState {
            enum { PLAYER = 0, AI = 1};
            int AIScore = 0;
            int playerScore = 0;
            bool ballside = PLAYER;
        };
        GameState gs;
    };
}
