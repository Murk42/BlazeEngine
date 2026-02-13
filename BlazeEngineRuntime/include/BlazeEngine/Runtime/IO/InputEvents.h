#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Type/Variant.h"
#include "BlazeEngine/Runtime/IO/InputTypes.h"

namespace Blaze
{
    class Window;
}

namespace Blaze::Input
{    
    struct EventProcessedState
    {
        enum State
        {
            NotProcessed,
            Processed,
            Consumed
        } value;

        constexpr EventProcessedState() : value(NotProcessed) { }
        constexpr EventProcessedState(const EventProcessedState&) = default;
        constexpr EventProcessedState(const State& value) : value(value) {}
        
        constexpr bool operator==(const EventProcessedState&) const = default;
        constexpr bool operator!=(const EventProcessedState&) const = default;
        constexpr bool operator==(const State& value) const { return this->value == value; }
        constexpr bool operator!=(const State& value) const { return this->value != value; }

        constexpr explicit operator bool() const { return value != EventProcessedState::NotProcessed; }
        constexpr operator State() const { return value; }

        constexpr EventProcessedState& operator=(const EventProcessedState&) = default;
        constexpr EventProcessedState& operator=(const State& value) { this->value = value; return *this; }
    };

    struct KeyDownEvent
    {
        Window& window;
        uint64 timeNS;
        KeyboardID keyboardID;

        Key key;
        KeyModifier modifier;
        KeyCode keyCode;

        bool repeat;
    };
    struct KeyUpEvent
    {
        Window& window;
        uint64 timeNS;
        KeyboardID keyboardID;

        Key key;
        KeyModifier modifier;
        KeyCode keyCode;
    };
    struct TextInputEvent
    {
        Window& window;
        uint64 timeNS;

        u8String string;
    };
    struct MouseButtonDownEvent
    {
        Window& window;
        uint64 timeNS;
        MouseID mouseID;

        MouseButton button;
        uint combo; //number of consecutive clicks
        Vec2f pos; //position in window coordinates
    };
    struct MouseButtonUpEvent
    {
        Window& window;
        uint64 timeNS;
        MouseID mouseID;

        MouseButton button;
        uint combo; //number of consecutive clicks
        Vec2f pos; //position in window coordinates
    };
    struct MouseMotionEvent
    {
        Window& window;
        uint64 timeNS;
        MouseID mouseID;

        Vec2f delta;
        Vec2f pos; //position in window coordinates
    };
    struct MouseScrollEvent
    {
        Window& window;
        uint64 timeNS;
        MouseID mouseID;

        Vec2f amount;
        ScrollDirection direction;
        Vec2f pos; //position in window coordinates
    };
    struct MouseEntersWindowEvent 
    {
        Window& window; 
        uint64 timeNS; 
        Input::MouseID mouseID; 

        Vec2f pos; 
    };
    struct MouseLeavesWindowEvent 
    { 
        Window& window; 
        uint64 timeNS; 
        Input::MouseID mouseID; 
        Vec2f pos; 
    };

    using GenericInputEvent = Variant<KeyDownEvent, KeyUpEvent, TextInputEvent, MouseButtonDownEvent, MouseButtonUpEvent, MouseMotionEvent, MouseScrollEvent, MouseEntersWindowEvent, MouseLeavesWindowEvent>;
}