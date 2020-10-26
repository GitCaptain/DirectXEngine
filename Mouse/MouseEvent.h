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
        EventType GetType() const;
        MousePoint GetPoint() const;
        int GetPosX() const;
        int GetPosY() const;

    private:
        EventType eType;
        int x;
        int y;

    };

}