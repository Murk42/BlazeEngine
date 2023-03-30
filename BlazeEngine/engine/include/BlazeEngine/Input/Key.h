#pragma once

namespace Blaze
{
	enum class Key
	{
		Unknown,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Zero,

		Return, Escape, Backspace, Tab, Space, Minus, Equals, LeftBracket, RightBracket, Backslash,
		Semicolon, Aapostrophe, Grave, Comma, Period, Slash, CapsLock,

		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

		PrintScreen, ScrollLock, Pause, Insert,

		Home, PageUp, Delete, End, PageDown, Right, Left, Down, Up,

		KeypadDivide, KeypadMultiply, KeypadMinus, KeypadPlus, KeypadEnter,
		Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9, Keypad0,
		KeypadPeriod,

		BlazeEngine, Power,
		F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
		Execute, Help, Menu, Select, Stop, Again, Undo, Cut, Copy, Paste, Mute, VolumeUp, VolumeDown,

		LCtrl, LShift, LAlt, RCtrl, RShift, RAlt,

		KeyCount
	};

	enum class MouseKey
	{
		MouseLeft, 
		MouseMiddle, 
		MouseRight,
		KeyCount
	};

	String BLAZE_API GetKeyName(Key key);
	String BLAZE_API GetKeyName(MouseKey key);
}