#pragma once

#include "../GraphicsState.h"


class Renderer {
public:
    virtual bool initRenderer(HWND renderWindowHandle, int windowWidth, int windowHeight) = 0;
    virtual GraphicsState& getGraphicsState() = 0;
    virtual void preparePipeline() = 0;
    virtual void present(size_t syncInterval, size_t flags) = 0;
    virtual ~Renderer() = default;
};
