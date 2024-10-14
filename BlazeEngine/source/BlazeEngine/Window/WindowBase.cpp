#include "pch.h"

#include "BlazeEngine/Window/WindowBase.h"
#include "BlazeEngine/Internal/GlobalData.h"

namespace Blaze
{			
	WindowBase::WindowBase() :
		pressedKeyCount(0), releasedKeyCount(0)
	{		
		{
			//std::lock_guard<std::mutex> lk{ blazeEngineContext.windowsMutex };
			blazeEngineContext.windows.Insert(this);
		}
	}	
	WindowBase::WindowBase(WindowBase&& other) noexcept :
		WindowBase(std::move(other), std::lock_guard(other.mutex))
	{
	}
	WindowBase::~WindowBase()
	{
		{
			//std::lock_guard<std::mutex> lk{ blazeEngineContext.windowsMutex };
			blazeEngineContext.windows.Erase(this);			
		}
	}

	WindowBase::KeyState WindowBase::GetLastKeyState(Key key) const
	{
		if ((uintMem)key > _countof(keyStates))
			return { };
		
		return keyStates[(uintMem)key];		
	}
	void WindowBase::ResetPressedAndReleasedKeys()
	{
		//If the all the pressed and released keys can fit in the arrays then use the arrays,
		//otherwise go through all the keys
		if (releasedKeyCount < MAX_RELEASED_KEY_COUNT && pressedKeyCount < MAX_PRESSED_KEY_COUNT)
		{
			for (uint i = 0; i < pressedKeyCount; ++i)
				keyStates[(uintMem)pressedKeys[i]].pressed = false;
			for (uint i = 0; i < releasedKeyCount; ++i)
				keyStates[(uintMem)releasedKeys[i]].released = false;
		}
		else
		{
			for (auto& keyState : keyStates)
			{
				keyState.pressed = false;
				keyState.released = false;
			}
		}

		pressedKeyCount = 0;
		releasedKeyCount = 0;
	}

	WindowBase& WindowBase::operator=(WindowBase&& other) noexcept
	{	
		std::lock_guard lk1(mutex);
		std::lock_guard lk2(other.mutex);

		resizedEventDispatcher     = std::move(other.resizedEventDispatcher);
		movedEventDispatcher       = std::move(other.movedEventDispatcher);
		minimizedEventDispatcher   = std::move(other.minimizedEventDispatcher);
		maximizedEventDispatcher   = std::move(other.maximizedEventDispatcher);
		focusGainedEventDispatcher = std::move(other.focusGainedEventDispatcher);
		focusLostEventDispatcher   = std::move(other.focusLostEventDispatcher);
		closeEventDispatcher       = std::move(other.closeEventDispatcher);
		mouseEnterDispatcher       = std::move(other.mouseEnterDispatcher);
		mouseLeaveDispatcher       = std::move(other.mouseLeaveDispatcher);
		keyPressedDispatcher       = std::move(other.keyPressedDispatcher);
		keyReleasedDispatcher      = std::move(other.keyReleasedDispatcher);
		mouseMotionDispatcher      = std::move(other.mouseMotionDispatcher);
		mouseScrollDispatcher      = std::move(other.mouseScrollDispatcher);
		textInputDispatcher        = std::move(other.textInputDispatcher);		

		pressedKeyCount = other.pressedKeyCount;
		releasedKeyCount = other.releasedKeyCount;
		std::copy_n(other.pressedKeys, pressedKeyCount, pressedKeys);
		std::copy_n(other.releasedKeys, releasedKeyCount, releasedKeys);
		other.pressedKeyCount = 0;
		other.releasedKeyCount = 0;

		return *this;
	}	
	WindowBase::WindowBase(WindowBase&& other, const std::lock_guard<std::mutex>& lk) noexcept :
		resizedEventDispatcher(std::move(other.resizedEventDispatcher)),
		movedEventDispatcher(std::move(other.movedEventDispatcher)),
		minimizedEventDispatcher(std::move(other.minimizedEventDispatcher)),
		maximizedEventDispatcher(std::move(other.maximizedEventDispatcher)),
		focusGainedEventDispatcher(std::move(other.focusGainedEventDispatcher)),
		focusLostEventDispatcher(std::move(other.focusLostEventDispatcher)),
		closeEventDispatcher(std::move(other.closeEventDispatcher)),
		mouseEnterDispatcher(std::move(other.mouseEnterDispatcher)),
		mouseLeaveDispatcher(std::move(other.mouseLeaveDispatcher)),
		keyPressedDispatcher(std::move(other.keyPressedDispatcher)),
		keyReleasedDispatcher(std::move(other.keyReleasedDispatcher)),
		mouseMotionDispatcher(std::move(other.mouseMotionDispatcher)),
		mouseScrollDispatcher(std::move(other.mouseScrollDispatcher)),
		textInputDispatcher(std::move(textInputDispatcher)),		
		pressedKeyCount(other.pressedKeyCount),
		releasedKeyCount(other.releasedKeyCount)
	{
		std::copy_n(other.pressedKeys, pressedKeyCount, pressedKeys);
		std::copy_n(other.releasedKeys, releasedKeyCount, releasedKeys);
		other.pressedKeyCount = 0;
		other.releasedKeyCount = 0;		

		//std::lock_guard<std::mutex> lk2{ blazeEngineContext.windowsMutex };
		blazeEngineContext.windows.Insert(this);
	}
	void WindowBase::HandleKeyEvent(Input::Events::KeyPressed event)
	{
		WindowBase::KeyState& keyState = keyStates[(uint)event.key];

		if (keyState.pressed == false && keyState.down == false)
		{
			pressedKeys[pressedKeyCount] = event.key;
			++pressedKeyCount;
			keyState.pressed = true;
		}

		keyState.down = true;
		keyState.up = false;

		if ((event.time - keyState.lastPressedTime).ToSeconds() <= blazeEngineContext.keyComboTime)
			++keyState.combo;
		else
			keyState.combo = 1;

		keyState.lastPressedTime = event.time;

		event.combo = keyState.combo;	

		keyPressedDispatcher.Call(event);
	}
	void WindowBase::HandleKeyEvent(Input::Events::KeyReleased event)
	{
		WindowBase::KeyState& keyState = keyStates[(uint)event.key];

		if (keyState.released == false && keyState.up == false)
		{
			releasedKeys[releasedKeyCount] = event.key;
			++releasedKeyCount;
			keyState.released = true;
		}

		keyState.down = false;
		keyState.up = true;

		keyReleasedDispatcher.Call(event);
	}	
}