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
		Light light;

		GraphicsState *graphicsState = nullptr;
		ImGUIW *imgui;
	};
}
