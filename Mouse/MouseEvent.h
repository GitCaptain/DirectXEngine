#pragma once

namespace Mouse {
    struct MousePoint {
        MousePoint() = default;
        MousePoint(int x, int y) : x(x), y(y) {}
        int x;
        int y;
    };
    
    class MouseEvent {
    public:
        enum class EventType {
            LPress,
            LRelease,
            RPress,
            RRelease,
            MPress,
            MRelease,
            WheelUp,
            WheelDown,
            Move,
            RAW_MOVE,
            Invalid
        };
    
        MouseEvent();
        MouseEvent(const EventType eType, const int x, const int y);
        bool isValid() const;
        EventType getType() const;
        MousePoint getPoint() const;
        int getPosX() const;
        int getPosY() const;

    private:
        EventType eType;
        int x;
        int y;

    };

}