#include "Light.h"

bool Light::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
    
    if (!model.initialize("Data/Objects/light.fbx", device, deviceContext, cb_vs_vertexshader)) {
        return false;
    }

    setPosition(0.0f, 0.0f, 0.0f);
    setRotation(0.0f, 0.0f, 0.0f);
    updateMatrix();
    return true;
}
