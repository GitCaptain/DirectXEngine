#include "ForwardRenderer.h"

bool ForwardRenderer::initialize(ID3D11DeviceContext* deviceContext) {
    this->deviceContext = deviceContext;
    return true;
}

void ForwardRenderer::renderFrame(const DirectX::XMMATRIX& viewProjectionMatrix) {

    for (auto& o : *renderableObjects) {
        o->prepare(deviceContext);
        o->draw(viewProjectionMatrix);
    }

}

void ForwardRenderer::setObjects(std::vector<IRenderable*> &renderables) {
    renderableObjects = &renderables;
}