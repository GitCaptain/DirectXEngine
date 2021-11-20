#pragma once
#include "GameObject3D.h"

struct Light: public NGameObject::GameObject3D {
    Light();
    void updateMatrix() override;
    DirectX::XMFLOAT3 lightColor = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    float lightStrength = 1.0f;
};

struct DynamicLight final : public Light {
    void updateMatrix() override;
    // light strength equation: strength = 1/(A + Bx +Cx*x)
    float attenuation_a = 1.0f;
    float attenuation_b = 0.1f;
    float attenuation_c = 0.1f;
};

struct AmbientLight final: public Light { };

struct LightInfo {
    AmbientLight ambient;
    std::vector<DynamicLight> dLights;
};
