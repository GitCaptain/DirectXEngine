#pragma once

#include "GameObject.h"

namespace NGraphics {

    class GameObject2D: public GameObject {
    public:
        GameObject2D() = default; 
        virtual ~GameObject2D() = default; 
        game_object_type getGameObjectType() override;
    };

}