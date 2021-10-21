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
		void BLAZE_API SetMousePos(const Vec2i&);
	}

	class BLAZE_API Window
	{		
		void* ptr = nullptr;				

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;		
	public:		
		Window();
		~Window();

		Vec2i GetPos() const;
		Vec2i GetSize() const;
		String GetTitle() const;

		void SetPos(Vec2i pos);
		void SetSize(Vec2i size);
		void SetTitle(StringView title);
		void SetMinimumSize(Vec2i size);
		void SetMaximumSize(Vec2i size);

		void Minimize();
		void Maximize();
		
		void Raise();

		void ShowWindow(bool show);				

		void SetFullscreen(bool fullscreen);
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

		inline void* GetHandle() const { return ptr; }		

		friend void Renderer::SetTarget(Window&);		
		friend void Input::SetMousePos(const Vec2i&);		
	};
}