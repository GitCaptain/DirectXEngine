#pragma once
#include "Scene.h"
#include "../Graphics/Model.h"
#include "../Graphics/RenderableGameObject.h"
#include "../Graphics/Camera3D.h"
#include "../Graphics/Shaders.h"
#include "../Graphics/Light.h"
#include "../HID/Mouse.h"
#include "../HID/Keyboard.h"
#include "../Graphics/ImGUIWrapper.h"

namespace App {

	using namespace NGameObject;

	class PongScene : Scene {
	public:
		PongScene() = default;
		~PongScene() = default;
		virtual bool initialize(GraphicsState& graphicsState) override;
		virtual void render() override;
		void update(HID::Keyboard &kbd, HID::Mouse &mouse, float dt);
	private:
		bool initializeShaders();

	private:
		ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
		ConstantBuffer<CB_PS_light> cb_ps_light;

		VertexShader vertexShader;
		PixelShader pixelShader;

		RenderableGameObject border;
		RenderableGameObject ball;
		Camera3D camera;
		float cameraSpeed = 0.1;
		Light light;

		GraphicsState *graphicsState = nullptr;
		ImGUIW *imgui;

		float tableWidth = 100;
		float tableLength = 200;
		float tableHeight = 1;
		XMFLOAT3 tablePos = { 0, 0, 0 };

		float borderWidth = 1;
		float borderLength = tableLength;
		float borderHeight = 5;
		XMFLOAT3 leftBorderPos = { -tableWidth / 2, 2 * tableHeight, 0 };
		XMFLOAT3 rightBorderPos = { tableWidth / 2, 2 * tableHeight, 0 };

		float padWidth = 20;
		float padHeight = 10;
		float padLength = 2;

		const XMFLOAT3 DefaultPlayerPos = { 0, 5,  -tableLength / 2 };
		const XMFLOAT3 DefaultAIPos = { 0, 5, tableLength / 2 };

		XMFLOAT3 PlayerPos = DefaultPlayerPos;
		XMFLOAT3 AIPos = DefaultAIPos;

		float ballRadius = 3;
		XMFLOAT3 ballPosition = DefaultPlayerPos;

		float playerSpeed = 1;
		float AISpeed = 1;
		float ballSpeed = 1;
	};
}
