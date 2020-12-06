#pragma once
namespace NKeyboard {

    using key_type = unsigned char;

    class KeyboardEvent {
    public:
        enum class EventType {
            Press, 
            Release,
            Invalid
        };

        KeyboardEvent();
        KeyboardEvent(const EventType eType, const key_type key);
        bool isPressEvent() const;
        bool isReleaseEvent() const;
        bool isValidEvent() const;

        unsigned char getKeyCode() const;

    private:
        EventType eType;
        unsigned char key;

    };
}