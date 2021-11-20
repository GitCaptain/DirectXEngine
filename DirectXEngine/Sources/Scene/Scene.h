#pragma once

#include <DirectXMath.h>
#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/GraphicsState.h"
#include "../HID/Mouse.h"
#include "../HID/Keyboard.h"
#include "../Graphics/RenderableGameObject.h"
#include "../Graphics/Light.h"
#include "../Graphics/Camera3D.h"

namespace App {

	class Scene {

	public:
		virtual bool initialize(GraphicsState& graphicsState) = 0;
		virtual void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
		virtual void postRender();
		virtual const Light& getLightInfo() const = 0;
		virtual const NGameObject::Camera3D& getCameraInfo() const = 0;
		virtual const DirectX::XMMATRIX& getViewMatrix() const = 0;
		virtual const DirectX::XMMATRIX& getProjectionMatrix() const = 0;
		virtual const std::vector<NGameObject::RenderableGameObject*>& getRenderables() const;
		virtual ~Scene() = default;
	protected:
		virtual void reset();
		virtual void updateInput(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
		virtual void updateLights();
		virtual void updateAI(float dt);
		virtual void checkCollision();
		virtual void updateGUI();
		virtual void updateGameObjects();
		Scene() = default;
		std::vector<NGameObject::RenderableGameObject*> p_renderables;
	};
}
