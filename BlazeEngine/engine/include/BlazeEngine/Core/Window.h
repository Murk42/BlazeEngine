#pragma once
#include <vector>
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze
{	
	class Window;

	namespace Renderer
	{
		void BLAZE_API SetTarget(Window&);
	}
	namespace Input
	{
		void BLAZE_API Update();
		void BLAZE_API SetMousePos(const Vec2i&);
	}

	class BLAZE_API Window
	{		
		void* ptr = nullptr;		

		Vec2i lockedMousePos;
		bool lockedMouse = false;		

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;		
	public:		
		Window();
		~Window();

		Vec2i GetPos() const;
		Vec2i GetSize() const;
		String GetTitle() const;

		void SetPos(const Vec2i& pos);
		void SetSize(const Vec2i& size);
		void SetTitle(const String& title);

		void Minimize();
		void Maximize();
		
		void Raise();

		void ShowWindow(bool show);
		void ShowCursor(bool show);
		void LockCursor(const Vec2i& pos);
		void UnlockCursor();

		void SetFullscreen();
		void SetBorderless(bool lockMouseInside);
		void SetWindowed(bool resizable, bool lockMouseInside);		

		bool IsFullscreen();
		bool IsBorderless();
		bool IsWindowed();
		bool IsResizable();
		bool IsMouseLockedInside();
		bool IsMinmized();
		bool IsMaximized();
		bool IsShown();		

		friend void Renderer::SetTarget(Window&);
		friend void Input::Update();
		friend void Input::SetMousePos(const Vec2i&);		
	};
}