#pragma once
#include "BlazeEngine/Window/WindowBase.h"

namespace Blaze
{
	enum class WindowSDLGraphicsAPI
	{
		OpenGL,
		Vulkan,
		Metal,
		None
	};
	enum class WindowSDLOpenMode
	{
		Fullscreen, Maximized, Minimized, Normal
	};
	enum class WindowSDLStyleFlags
	{
		Resizable, Borderless, LockMouse
	};
	ENUM_CLASS_BITWISE_OPERATIONS(WindowSDLStyleFlags)

	struct WindowSDLCreateOptions
	{
		WindowSDLGraphicsAPI graphicsAPI;
		StringUTF8 title = StringUTF8();
		Vec2i pos = Vec2i(INT_MAX, INT_MAX);
		Vec2u size = Vec2u(1, 1);
		WindowSDLOpenMode openMode = WindowSDLOpenMode::Normal;
		WindowSDLStyleFlags styleFlags = WindowSDLStyleFlags::Resizable;
	};

	class BLAZE_API WindowSDL : public WindowBase
	{
	public:
		using WindowSDLHandle = void*;
		
		WindowSDL();
		WindowSDL(const WindowSDL&) = delete;
		WindowSDL(WindowSDL&&) noexcept;
		WindowSDL(WindowSDLCreateOptions createOptions);
		~WindowSDL() override;

		void Destroy();
		
		void SwapBuffers();

		Vec2i GetPos() const override final;
		Vec2u GetSize() const override final;
		StringUTF8 GetTitle() const override final;
		Vec2i GetMousePos() const override final;

		Result SetIcon(BitmapView bitmap) override final;
		void SetOpacity(float opacity) override final;
		void SetPos(Vec2i pos) override final;
		void SetSize(Vec2u size) override final;
		void SetTitle(StringViewUTF8 title) override final;
		void SetMinimumSize(Vec2u size) override final;
		void SetMaximumSize(Vec2u size) override final;

		void Minimize() override final;
		void Maximize() override final;

		void Raise() override final;

		void ShowWindow(bool show) override final;

		void SetWindowFullscreenMode(bool fullscreen) override final;
		void SetWindowBorderFlag(bool hasBorder) override final;
		void SetWindowResizableFlag(bool resizable) override final;
		void SetWindowLockMouseFlag(bool lockMouse) override final;

		bool IsFullscreen() const override final;
		bool IsBorderless() const override final;
		bool IsResizable() const override final;
		bool IsMouseLocked() const override final;
		bool IsMinimized() const override final;
		bool IsMaximized() const override final;
		bool IsShown() const override final;

		uint GetWindowVideoDisplayIndex() const override final;
		DisplayMode GetWindowDisplayMode() const override final;
		void SetWindowDisplayMode(DisplayMode mode) override final;

		inline WindowSDLHandle GetHandle() const { return handle; }
		inline bool IsNullWindow() const { return handle == nullptr; }
		
		static uint GetDisplayCount();
		static uint GetDisplayModeCount(uint videoDisplayIndex);
		static DisplayMode GetDisplayMode(uint videoDisplayIndex, uint index);

		WindowSDL& operator=(const WindowSDL&) = delete;
		WindowSDL& operator=(WindowSDL&&) noexcept;
	private:				
		WindowSDLHandle handle;
		WindowSDLGraphicsAPI graphicsAPI;		
		std::condition_variable resizeCV;
		Vec2u latestWindowSize;
		int resizeState;

		static void WindowResizeFunction(WindowSDL* window);
		void HandleResizeEvent(const Input::Events::WindowResizedEvent& event) override final;
	};
}