#pragma once

#include <vector>
#include <d3d11.h>
#include "interfaces/IRenderable.h"

class ForwardRenderer {

public:

    ForwardRenderer() = default;
    bool initialize(ID3D11DeviceContext* deviceContext);
    void renderFrame(const DirectX::XMMATRIX& viewProjectionMatrix);
    void setObjects(std::vector<IRenderable*> &renderables);

private:
    std::vector<IRenderable*> *renderableObjects;
    ID3D11DeviceContext* deviceContext = nullptr;
};