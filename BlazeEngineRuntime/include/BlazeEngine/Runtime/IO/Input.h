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


    BLAZE_API bool GetKeyState(Key key);
    BLAZE_API bool GetMouseButtonState(MouseButton button);

    BLAZE_API StringView GetMouseName(MouseID mouseID);
    BLAZE_API void SetCursorType(CursorType type);
    BLAZE_API void ShowCursor(bool show);
}