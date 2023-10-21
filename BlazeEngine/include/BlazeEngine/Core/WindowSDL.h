#pragma once
#include "BlazeEngine/Core/WindowBase.h"

namespace Blaze
{
	enum class WindowHandleGraphicsaAPI
	{
		OpenGL,
		Vulkan,
		Metal,
		None
	};

	class BLAZE_API WindowSDL : public WindowBase
	{
	public:
		using WindowSDLHandle = void*;
		
		WindowSDL();
		WindowSDL(StringView title, Vec2i pos, Vec2i size, WindowHandleGraphicsaAPI graphicsAPI);
		WindowSDL(const WindowSDL&) = delete;
		WindowSDL(WindowSDL&&) noexcept;
		~WindowSDL() override;

		void Destroy();

		Vec2i GetPos() const override final;
		Vec2i GetSize() const override final;
		String GetTitle() const override final;

		Result SetIcon(BitmapView bitmap) override final;
		void SetOpacity(float opacity) override final;
		void SetPos(Vec2i pos) override final;
		void SetSize(Vec2i size) override final;
		void SetTitle(StringView title) override final;
		void SetMinimumSize(Vec2i size) override final;
		void SetMaximumSize(Vec2i size) override final;

		void Minimize() override final;
		void Maximize() override final;

		void Raise() override final;

		void ShowWindow(bool show) override final;

		void SetWindowFullscreenMode(bool fullscreen) override final;
		void SetWindowBorderFlag(bool hasBorder) override final;
		void SetWindowResizableFlag(bool resizable) override final;
		void SetLockMouseFlag(bool lockMouse) override final;

		bool IsFullscreen() override final;
		bool IsBorderless() override final;
		bool IsResizable() override final;
		bool IsMouseLocked() override final;
		bool IsMinmized() override final;
		bool IsMaximized() override final;
		bool IsShown() override final;

		uint GetWindowVideoDisplayIndex() override final;
		DisplayMode GetWindowDisplayMode() override final;
		void SetWindowDisplayMode(DisplayMode mode) override final;

		inline WindowSDLHandle GetHandle() const { return handle; }
		inline bool IsNullWindow() const { return handle == nullptr; }

		WindowSDL& operator=(const WindowSDL&) = delete;
		WindowSDL& operator=(WindowSDL&&) noexcept;
	private:		
		WindowSDLHandle handle;
	};
}