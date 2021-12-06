#pragma once

#include "GameObject3D.h"

namespace NGameObject {

    class RenderableGameObject: public GameObject3D {

    public:
        bool initialize(
            const std::string& filePath,
            ID3D11Device* device
        );
        const Model& getModel() const;
        const XMMATRIX& getWorldMatrix() const;

    protected:
        void updateMatrix() override;

        Model model;
    };

}
