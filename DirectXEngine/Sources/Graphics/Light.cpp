#include "Light.h"

bool Light::initialize(ID3D11Device* device) {
    
    if (!model.initialize("Resources/Objects/light.fbx", device)) {
        return false;
    }

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();
    return true;
}
