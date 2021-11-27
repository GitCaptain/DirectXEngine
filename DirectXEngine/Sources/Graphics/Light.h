#pragma once
#include "RenderableGameObject.h"

struct PointLight: public NGameObject::RenderableGameObject {
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    DirectX::XMFLOAT3 attenuations = DirectX::XMFLOAT3(1.0f, 0.1f, 0.1f);
    float lightStrength = 1.0f;
};

struct AmbientLight final {
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;
};

struct LightInfo {
    AmbientLight ambient;
    std::vector<PointLight> pointLights;
};
