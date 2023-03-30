#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze
{
	class BLAZE_API  Window
	{		
		void* ptr = nullptr;					

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;		
	public:		
		Window();
		Window(StringView title);
		Window(StringView title, Vec2i size);
		~Window();

		Vec2i GetPos() const;
		Vec2i GetSize() const;
		String GetTitle() const;

		void SetIcon(BitmapView bitmap);
		void SetOpacity(float opacity);
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

		uint GetWindowVideoDisplayIndex();
		DisplayMode GetWindowDisplayMode();
		void SetWindowDisplayMode(DisplayMode mode);

		inline void* GetHandle() const { return ptr; }				
	};
}