#pragma once

#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/GraphicsState.h"
#include "../HID/Mouse.h"
#include "../HID/Keyboard.h"

namespace App {

	class Scene {

	public:
		virtual bool initialize(GraphicsState& graphicsState) = 0;
		virtual void reset() = 0;
		virtual void render() = 0;
		virtual void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) = 0;
		virtual ~Scene() = default;
	protected:
		Scene() = default;
	};
}