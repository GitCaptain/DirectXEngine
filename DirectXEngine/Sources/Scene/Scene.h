#pragma once

#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/GraphicsState.h"

namespace App {

	class Scene {

	public:
		virtual void render() = 0;
		virtual bool initialize(GraphicsState& graphicsState) = 0;
		virtual ~Scene() = default;
	protected:
		Scene() = default;
	};
}