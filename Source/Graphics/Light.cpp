#include "Light.h"
#include <wrl/client.h>

bool Light::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ConstantBuffer<CB_VS_m_world_viewprojeciton>& cb_vs_vertexshader) {
    
    if (!RenderableGameObject3D::initialize("Data/Objects/light.fbx", 
                                            device, 
                                            deviceContext, 
                                            cb_vs_vertexshader)) {
        return false;
    }
    return true;
}

void Light::draw(const DirectX::XMMATRIX& viewProjectionMatrix) {
    RenderableGameObject3D::draw(viewProjectionMatrix);
}


void Light::prepare(ID3D11DeviceContext* deviceContext) {}
