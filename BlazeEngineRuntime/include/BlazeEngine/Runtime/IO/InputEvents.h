#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Type/Variant.h"
#include "BlazeEngine/Runtime/IO/Types.h"

namespace Blaze
{
    class Window;
}

namespace Blaze::Input
{
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

    using GenericInputEvent = Variant<KeyDownEvent, KeyUpEvent, TextInputEvent, MouseButtonDownEvent, MouseButtonUpEvent, MouseMotionEvent, MouseScrollEvent>;

    
}