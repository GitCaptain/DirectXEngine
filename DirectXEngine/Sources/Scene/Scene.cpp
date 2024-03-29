#include "Scene.h"

using namespace App;

void Scene::update(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) {
    updateInput(kbd, mouse, dt);
    updateLights();
    updateGameObjects();
    updateAI(dt);
    checkCollision();
}

void App::Scene::postRender() {
    updateGUI();
}

const std::vector<const NGameObject::RenderableGameObject*>& Scene::getRenderables() const {
    return p_renderables;
}

void Scene::reset() { }

void Scene::updateInput(HID::Keyboard& kbd, HID::Mouse& mouse, float dt) { }

void Scene::updateLights() { }

void Scene::updateAI(float dt) { }

void Scene::checkCollision() { }

void Scene::updateGUI() { }

void Scene::updateGameObjects() { }

void Scene::updateProjectionSetting() {
    if (
        projSettings.farZ != graphicsSettings->farZ
        || projSettings.nearZ != graphicsSettings->nearZ
        || projSettings.fov != graphicsSettings->fov
    ) {
        projSettings.farZ = graphicsSettings->farZ;
        projSettings.nearZ != graphicsSettings->nearZ;
        projSettings.fov != graphicsSettings->fov;
        camera.setProjectionValues(
            graphicsSettings->fov,
            static_cast<float>(graphicsState->windowWidth) / static_cast<float>(graphicsState->windowHeight),
            graphicsSettings->nearZ,
            graphicsSettings->farZ
        );
    }
}
