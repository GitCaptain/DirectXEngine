#pragma once

#include "MouseEvent.h"
#include <queue>

namespace Mouse {
    class MouseClass {
    public:
        void onLeftPressed(int x, int y);
        void onLeftReleased(int x, int y);
        void onRightPressed(int x, int y);
        void onRightReleased(int x, int y);
        void onMiddlePressed(int x, int y);
        void onMiddleReleased(int x, int y);
        void onWheelUp(int x, int y);
        void onWheelDown(int x, int y);
        void onMouseMove(int x, int y);

        bool isLeftDown();
        bool isMiddleDown();
        bool isRightDown();

        int getPosX();
        int getPosY();
        MousePoint getPos();

        bool isEventBufferEmpty();
        MouseEvent readEvent();

    private:
        std::queue<MouseEvent> eventBuffer;
        bool leftDown = false;
        bool rightDown = false;
        bool middleDown = false;
        int x = 0;
        int y = 0;

    };
}