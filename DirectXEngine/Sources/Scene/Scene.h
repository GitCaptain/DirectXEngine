#pragma once

#include <DirectXMath.h>
#include "../Graphics/GraphicsState.h"
#include "../HID/Mouse.h"
#include "../HID/Keyboard.h"
#include "../Graphics/RenderableGameObject.h"
#include "../Graphics/Light.h"
#include "../Graphics/Camera3D.h"
#include "../Timer.h"
namespace App {

    using namespace NGameObject;

    class Scene {

    public:
        virtual bool initialize(GraphicsState& graphicsState, GraphicsSettings& graphicsSettings) = 0;
        virtual void update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
        virtual void postRender();
        virtual const LightInfo& getLightInfo() const = 0;
        virtual const NGameObject::Camera3D& getCameraInfo() const = 0;
        virtual const DirectX::XMMATRIX& getViewMatrix() const = 0;
        virtual const DirectX::XMMATRIX& getProjectionMatrix() const = 0;
        virtual const std::vector<const NGameObject::RenderableGameObject*>& getRenderables() const;
        virtual ~Scene() = default;
    protected:
        virtual void reset();
        virtual void updateInput(HID::Keyboard& kbd, HID::Mouse& mouse, float dt);
        virtual void updateLights();
        virtual void updateAI(float dt);
        virtual void checkCollision();
        virtual void updateGUI();
        virtual void updateGameObjects();
        virtual void updateProjectionSetting();
        Scene() = default;

        std::vector<const NGameObject::RenderableGameObject*> p_renderables;
        GraphicsState* graphicsState = nullptr;
        GraphicsSettings* graphicsSettings = nullptr;
        struct ProjectionSettings {
            float fov;
            float nearZ;
            float farZ;
        } projSettings;

        Camera3D camera;
    };
}
