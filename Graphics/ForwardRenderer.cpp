#include "ForwardRenderer.h"

bool ForwardRenderer::initialize(ID3D11Device* device, 
                                 ID3D11DeviceContext* deviceContext, 
                                 IDXGISwapChain* swapChain, 
                                 ID3D11RenderTargetView* renderTargetView) {
    
    this->device = device;
    this->deviceContext = deviceContext;
    this->swapChain = swapChain;
    this->renderTargetView = renderTargetView;
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