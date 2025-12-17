#pragma once
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Runtime/IO/Types.h"

namespace Blaze::Input
{
    struct InputPreUpdateEvent {};
    struct InputPostUpdateEvent {};

    BLAZE_API void Update();

    BLAZE_API Vec2f GetFocusedWindowLastUpdateMousePos();
    BLAZE_API Vec2f GetFocusedWindowLastUpdateMouseMovementSum();

    BLAZE_API bool HasClipboardText();
    BLAZE_API u8String GetClipboardText();
    BLAZE_API void SetClipboardText(u8StringView text);

    BLAZE_API KeyCode GetKeyCode(Key key, KeyModifier modifier);
    BLAZE_API StringView GetKeyName(KeyCode keyCode);

    /*
        Returns key state between two Input::Update() calls. By using this function an application wont be
        able to register multiple key presses or releases between two Input::Update() calls, they can
        register only if they happened at least once. For full control over key presses and releases use
        event handling to handle key states.

        \param key - the key whose state to get
        \return A KeyFrameState structure with information about the state of the key between two Input::Update()
        calls
    */
    BLAZE_API KeyFrameState GetKeyFrameState(Key key);
    /*
        Returns mouse button state between two Input::Update() calls. By using this function an application
        wont be able to register multiple key presses or releases between two Input::Update() calls, they
        can register only if they happened at least once. For full control over mouse button presses and
        releases use event handling.

        \param button - the mouse button whose state to retrieve
        \return A MouseButtonFrameState structure with information about the state of the key between two
        Input::Update() calls
    */
    BLAZE_API MouseButtonFrameState GetMouseButtonFrameState(MouseButton button);

    BLAZE_API StringView GetMouseName(MouseID mouseID);
    BLAZE_API void SetCursorType(CursorType type);
    BLAZE_API void ShowCursor(bool show);
}