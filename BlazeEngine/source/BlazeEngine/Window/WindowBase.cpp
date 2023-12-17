#include "pch.h"

#include "BlazeEngine/Window/WindowBase.h"

namespace Blaze
{
	WindowBase::WindowBase()
		: implementation(nullptr)
	{

	}	
	WindowBase::WindowBase(WindowBase&& other) noexcept :
		resizedEventDispatcher(std::move(other.resizedEventDispatcher)),
		movedEventDispatcher(std::move(other.movedEventDispatcher)),
		minimizedEventDispatcher(std::move(other.minimizedEventDispatcher)),
		maximizedEventDispatcher(std::move(other.maximizedEventDispatcher)),
		focusGainedEventDispatcher(std::move(other.focusGainedEventDispatcher)),
		focusLostEventDispatcher(std::move(other.focusLostEventDispatcher)),
		closeEventDispatcher(std::move(other.closeEventDispatcher)),
		mouseEnterDispatcher(std::move(other.mouseEnterDispatcher)),
		mouseLeaveDispatcher(std::move(other.mouseLeaveDispatcher)), 
		implementation(other.implementation)
	{
		other.implementation = nullptr;
	}
	WindowBase::~WindowBase()
	{
		if (implementation != nullptr)
		{
			std::destroy_at(implementation);
			allocator.Free(implementation);			
			implementation = nullptr;
		}
	}
	Vec2i WindowBase::GetPos() const		
	{
		if (implementation != nullptr)
			return implementation->GetPos();
		return Vec2i();
	}
	Vec2u WindowBase::GetSize() const
	{
		if (implementation != nullptr)
			return implementation->GetSize();
		return Vec2u();
	}
	String WindowBase::GetTitle() const
	{
		if (implementation != nullptr)
			return implementation->GetTitle();
		return String();
	}
	Vec2i WindowBase::GetMousePos() const
	{
		if (implementation != nullptr)
			return implementation->GetMousePos();
		return Vec2i(0, 0);
	}
	Result WindowBase::SetIcon(BitmapView bitmap)
	{
		if (implementation != nullptr)
			return implementation->SetIcon(bitmap);
		return Result();
	}
	void WindowBase::SetOpacity(float opacity)
	{
		if (implementation != nullptr)
			implementation->SetOpacity(opacity);		
	}
	void WindowBase::SetPos(Vec2i pos)
	{
		if (implementation != nullptr)
			implementation->SetPos(pos);
	}
	void WindowBase::SetSize(Vec2u size)
	{
		if (implementation != nullptr)
			implementation->SetSize(size);		
	}
	void WindowBase::SetTitle(StringView title)
	{
		if (implementation != nullptr)
			implementation->SetTitle(title);		
	}
	void WindowBase::SetMinimumSize(Vec2u size)
	{
		if (implementation != nullptr)
			implementation->SetMinimumSize(size);		
	}
	void WindowBase::SetMaximumSize(Vec2u size)
	{
		if (implementation != nullptr)
			implementation->SetMaximumSize(size);		
	}
		
	void WindowBase::Minimize()
	{
		if (implementation != nullptr)
			implementation->Minimize();		
	}

	void WindowBase::Maximize()
	{
		if (implementation != nullptr)
			implementation->Maximize();		
	}
		
	void WindowBase::Raise()
	{
		if (implementation != nullptr)
			implementation->Raise();
	}
		
	void WindowBase::ShowWindow(bool show)
	{
		if (implementation != nullptr)
			implementation->ShowWindow(show);
	}
		
	void WindowBase::SetWindowFullscreenMode(bool fullscreen)
	{
		if (implementation != nullptr)
			implementation->SetWindowFullscreenMode(fullscreen);
	}
	void WindowBase::SetWindowBorderFlag(bool hasBorder)
	{
		if (implementation != nullptr)
			implementation->SetWindowBorderFlag(hasBorder);
	}
	void WindowBase::SetWindowResizableFlag(bool resizable)
	{
		if (implementation != nullptr)
			implementation->SetWindowResizableFlag(resizable);
	}
	void WindowBase::SetLockMouseFlag(bool lockMouse)
	{
		if (implementation != nullptr)
			implementation->SetLockMouseFlag(lockMouse);
	}
		
	bool WindowBase::IsFullscreen()
	{
		if (implementation != nullptr)
			return implementation->IsFullscreen();
		return false;
	}
	bool WindowBase::IsBorderless()
	{
		if (implementation != nullptr)
			return implementation->IsBorderless();
		return false;
	}
	bool WindowBase::IsResizable()
	{
		if (implementation != nullptr)
			return implementation->IsResizable();
		return false;
	}
	bool WindowBase::IsMouseLocked()
	{
		if (implementation != nullptr)
			return implementation->IsMouseLocked();
		return false;
	}
	bool WindowBase::IsMinmized()
	{
		if (implementation != nullptr)
			return implementation->IsMinmized();
		return false;
	}
	bool WindowBase::IsMaximized()
	{
		if (implementation != nullptr)
			return implementation->IsMaximized();
		return false;
	}
	bool WindowBase::IsShown()
	{
		if (implementation != nullptr)
			return implementation->IsShown();
		return false;
	}
		
	uint WindowBase::GetWindowVideoDisplayIndex()
	{
		if (implementation != nullptr)
			return implementation->GetWindowVideoDisplayIndex();
		return 0;
	}
	DisplayMode WindowBase::GetWindowDisplayMode()
	{
		if (implementation != nullptr)
			return implementation->GetWindowDisplayMode();

		return DisplayMode();
	}
	void WindowBase::SetWindowDisplayMode(DisplayMode mode)
	{
		if (implementation != nullptr)
			implementation->SetWindowDisplayMode(mode);
	}
	
	WindowBase& WindowBase::operator=(WindowBase&& other) noexcept
	{
		resizedEventDispatcher = std::move(other.resizedEventDispatcher);
		movedEventDispatcher = std::move(other.movedEventDispatcher);
		minimizedEventDispatcher = std::move(other.minimizedEventDispatcher);
		maximizedEventDispatcher = std::move(other.maximizedEventDispatcher);
		focusGainedEventDispatcher = std::move(other.focusGainedEventDispatcher);
		focusLostEventDispatcher = std::move(other.focusLostEventDispatcher);
		closeEventDispatcher = std::move(other.closeEventDispatcher);
		mouseEnterDispatcher = std::move(other.mouseEnterDispatcher);
		mouseLeaveDispatcher = std::move(other.mouseLeaveDispatcher);
		implementation = other.implementation;
		other.implementation = nullptr;

		return *this;
	}
}