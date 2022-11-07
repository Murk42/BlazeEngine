#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "source/BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Core/Startup.h"

#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	void* GetOpenGLInitWindow();
	void* GetOpenGLContext();	

	extern EventDispatcher<Event::ViewportChanged   >   viewportChangedDispatcher;

	namespace Graphics
	{
		void Graphics_ViewportChanged(Event::ViewportChanged);
	}
	
	static Graphics::Core::VertexArray defaultVertexArray;
	static Graphics::Core::VertexArray* selectedVertexArray = nullptr;
	static Graphics::Core::GraphicsBuffer* selectedArrayBuffer = nullptr;
	static Graphics::Core::GraphicsBuffer* selectedUniformBuffer = nullptr;
	static Graphics::Core::ShaderProgram* selectedProgram = nullptr;
	static Graphics::Core::Renderbuffer* selectedRenderbuffer = nullptr;
	static Graphics::Core::Framebuffer* selectedFramebuffer = nullptr;
	static Graphics::Core::Framebuffer* selectedDrawFramebuffer = nullptr;
	static Graphics::Core::Framebuffer* selectedReadFramebuffer = nullptr;

	static Graphics::Core::Texture1D* selectedTexture1D = nullptr;
	static Graphics::Core::Texture2D* selectedTexture2D = nullptr;
	static Graphics::Core::Texture3D* selectedTexture3D = nullptr;
	static Graphics::Core::Texture2DArray* selectedTexture2DArray = nullptr;
	static Graphics::Core::TextureCubemap* selectedTextureCubemap = nullptr;
	static Graphics::Core::TextureBuffer* selectedTextureBuffer = nullptr;

	static void* target = nullptr;

	static Vec2i viewportPos;
	static Vec2i viewportSize;

	static ColorRGBAf clearColor;


	Startup::RendererInitInfo InitializeRenderer()
	{
		Startup::RendererInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		target = GetOpenGLInitWindow();
		selectedVertexArray = &defaultVertexArray;

		int val[4];
		glGetIntegerv(GL_VIEWPORT, val);
		viewportPos = { val[0], val[1] };
		viewportSize = { val[2], val[3] };

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateRenderer()
	{

	}

	namespace Renderer
	{

		GLenum OpenGLRenderPrimitive(PrimitiveType type)
		{
			switch (type)
			{
			case PrimitiveType::Points: return GL_POINTS;
			case PrimitiveType::Lines: return GL_LINES;
			case PrimitiveType::LineStrip: return GL_LINE_STRIP;			
			case PrimitiveType::LineLoop: return GL_LINE_LOOP;
			case PrimitiveType::Triangles: return GL_TRIANGLES;			
			case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
			case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
			case PrimitiveType::Pathes: return GL_PATCHES;
			}
		}
		GLenum OpenGLIndexType(IndexType type)
		{
			switch (type)
			{
			case IndexType::Uint32: return GL_UNSIGNED_INT;
			case IndexType::Uint16: return GL_UNSIGNED_SHORT;
			case IndexType::Uint8: return GL_UNSIGNED_BYTE;		
			}
		}
		GLenum OpenGLPolygonMode(PolygonMode mode)
		{
			switch (mode)
			{
			case PolygonMode::Point: return GL_POINT;
			case PolygonMode::Line: return GL_LINE;
			case PolygonMode::Fill: return GL_FILL;		
			}
		}
		GLenum OpenGLImageAccess(ImageAccess access)
		{
			switch (access)
			{
			case ImageAccess::Read: return GL_READ_ONLY;
			case ImageAccess::Write: return GL_WRITE_ONLY;
			case ImageAccess::ReadWrite: return GL_READ_WRITE;		
			}
		}
		GLenum OpenGLImageFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGBA32F:			return GL_RGBA32F;
			case ImageFormat::RGBA16F:			return GL_RGBA16F;
			case ImageFormat::RG32F:			return GL_RG32F;
			case ImageFormat::RG16F:			return GL_RG16F;
			case ImageFormat::R11F_G11F_B10F:	return GL_R11F_G11F_B10F;
			case ImageFormat::R32F:			return GL_R32F;
			case ImageFormat::R16F:			return GL_R16F;
			case ImageFormat::RGBA32UI:		return GL_RGBA32UI;
			case ImageFormat::RGBA16UI:		return GL_RGBA16UI;
			case ImageFormat::RGB10_A2UI:		return GL_RGB10_A2UI;
			case ImageFormat::RGBA8UI:			return GL_RGBA8UI;
			case ImageFormat::RG32UI:			return GL_RG32UI;
			case ImageFormat::RG16UI:			return GL_RG16UI;
			case ImageFormat::RG8UI:			return GL_RG8UI;
			case ImageFormat::R32UI:			return GL_R32UI;
			case ImageFormat::R16UI:			return GL_R16UI;
			case ImageFormat::R8UI:			return GL_R8UI;
			case ImageFormat::RGBA32I:			return GL_RGBA32I;
			case ImageFormat::RGBA16I:			return GL_RGBA16I;
			case ImageFormat::RGBA8I:			return GL_RGBA8I;
			case ImageFormat::RG32I:			return GL_RG32I;
			case ImageFormat::RG16I:			return GL_RG16I;
			case ImageFormat::RG8I:			return GL_RG8I;
			case ImageFormat::R32I:			return GL_R32I;
			case ImageFormat::R16I:			return GL_R16I;
			case ImageFormat::R8I:				return GL_R8I;
			case ImageFormat::RGBA16:			return GL_RGBA16;
			case ImageFormat::RGB10_A2:		return GL_RGB10_A2;
			case ImageFormat::RGBA8:			return GL_RGBA8;
			case ImageFormat::RG16:			return GL_RG16;
			case ImageFormat::RG8:				return GL_RG8;
			case ImageFormat::R16:				return GL_R16;
			case ImageFormat::R8:				return GL_R8;
			case ImageFormat::RGBA16_SNORM:	return GL_RGBA16_SNORM;
			case ImageFormat::RGBA8_SNORM:		return GL_RGBA8_SNORM;
			case ImageFormat::RG16_SNORM:		return GL_RG16_SNORM;
			case ImageFormat::RG8_SNORM:		return GL_RG8_SNORM;
			case ImageFormat::R16_SNORM:		return GL_R16_SNORM;
			case ImageFormat::R8_SNORM:		return GL_R8_SNORM;			
			}
		}		

		uint GetVideoDisplayCount()
		{
			return SDL_GetNumVideoDisplays();
		}
		uint GetDisplayModeCount(uint videoDisplayIndex)
		{
			return SDL_GetNumDisplayModes(videoDisplayIndex);
		}

		DisplayMode GetDisplayMode(uint videoDisplayIndex, uint displayModeIndex)
		{
			SDL_DisplayMode mode;
			SDL_GetDisplayMode(videoDisplayIndex, displayModeIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		DisplayMode GetCurrentDisplayMode(uint videoDisplayIndex)
		{
			SDL_DisplayMode mode;
			SDL_GetCurrentDisplayMode(videoDisplayIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		DisplayMode GetDesktopDisplayMode(uint videoDisplayIndex)
		{
			SDL_DisplayMode mode;
			SDL_GetDesktopDisplayMode(videoDisplayIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		bool GetClosestDisplayMode(uint displayIndex, DisplayMode mode, DisplayMode& out)
		{
			SDL_DisplayMode x;
			x.format = SDLDisplayPixelFormat(mode.format);
			x.w = mode.size.x;
			x.h = mode.size.y;
			x.refresh_rate = mode.refreshRate;
			x.driverdata = nullptr;

			SDL_DisplayMode closest;

			if (SDL_GetClosestDisplayMode(displayIndex, &x, &closest) == NULL)
				return false;

			out.format = BlazeDisplayPixelFormat(closest.format);
			out.refreshRate = closest.refresh_rate;
			out.size = Vec2i(closest.w, closest.h);
			return true;			
		}

		void SetActiveTextureSlot(uint slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
		}

		void SelectTexture(Graphics::Core::Texture1D* obj)
		{
			glBindTexture(GL_TEXTURE_1D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture1D = obj;
		}
		Graphics::Core::Texture1D* GetSelectedTexture1D()
		{
			return selectedTexture1D;
		}

		void SelectTexture(Graphics::Core::Texture2D* obj)
		{			
			glBindTexture(GL_TEXTURE_2D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture2D = obj;
		}		
		Graphics::Core::Texture2D* GetSelectedTexture2D()
		{
			return selectedTexture2D;
		}

		void SelectTexture(Graphics::Core::Texture2DArray* obj)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture2DArray = obj;
		}		
		Graphics::Core::Texture2DArray* GetSelectedTexture2DArray()
		{			
			return selectedTexture2DArray;
		}

		void SelectTexture(Graphics::Core::TextureCubemap* obj)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, obj == nullptr ? 0 : obj->GetHandle());
			selectedTextureCubemap = obj;
		}
		Graphics::Core::TextureCubemap* GetSelectedTextureCubemap()
		{
			return selectedTextureCubemap;
		}

		void SelectTexture(Graphics::Core::Texture3D* obj)
		{
			glBindTexture(GL_TEXTURE_3D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture3D = obj;
		}
		Graphics::Core::Texture3D* GetSelectedTexture3D()
		{
			return selectedTexture3D;
		}

		void SelectTexture(Graphics::Core::TextureBuffer* obj)
		{
			glBindTexture(GL_TEXTURE_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedTextureBuffer = obj;
		}
		Graphics::Core::TextureBuffer* GetTextureBuffer()
		{
			return selectedTextureBuffer;
		}

		void SelectVertexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_ARRAY_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedArrayBuffer = obj;
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedVertexBuffer()
		{
			return selectedArrayBuffer;
		}

		void SelectIndexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{			
			selectedVertexArray->SetIndexBuffer(*obj);
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedIndexBuffer()
		{
			return selectedVertexArray->GetIndexBuffer();
		}

		void SelectUniformBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedUniformBuffer = obj;
		}
		Graphics::Core::GraphicsBuffer* GetSelectedUniformBuffer()
		{
			return selectedUniformBuffer;
		}

		void SelectVertexArray(Graphics::Core::VertexArray* obj)
		{
			glBindVertexArray(obj == nullptr ? 0 : obj->GetHandle());
			selectedVertexArray = obj;
		}		
		Graphics::Core::VertexArray* GetSelectedVertexArray()
		{
			return selectedVertexArray;
		}

		void SelectProgram(Graphics::Core::ShaderProgram* obj)
		{
			glUseProgram(obj == nullptr ? 0 : obj->GetHandle());
			selectedProgram = obj;
		}
		Graphics::Core::ShaderProgram* GetSelectedProgram()
		{
			return selectedProgram;
		}

		void SelectFramebuffer(Graphics::Core::Framebuffer* obj)
		{ 
			glBindFramebuffer(GL_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedFramebuffer = obj;
		}
		Graphics::Core::Framebuffer* GetSelectedFramebuffer()
		{
			return selectedFramebuffer;;
		}

		void SelectDrawFramebuffer(Graphics::Core::Framebuffer* obj)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedDrawFramebuffer = obj;
		}

		BLAZE_API Graphics::Core::Framebuffer* GetDrawSelectedFramebuffer()
		{
			return selectedDrawFramebuffer;
		}

		BLAZE_API void SelectReadFramebuffer(Graphics::Core::Framebuffer* obj)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedReadFramebuffer = obj;
		}

		BLAZE_API Graphics::Core::Framebuffer* GetReadSelectedFramebuffer()
		{
			return selectedReadFramebuffer;
		}

		void SelectRenderbuffer(Graphics::Core::Renderbuffer* obj)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedRenderbuffer = obj;
		}
		Graphics::Core::Renderbuffer* GetSelectedRenderbuffer()
		{
			return selectedRenderbuffer;
		}


		void SetClearColor(ColorRGBAf color)
		{
			clearColor = color;
			glClearColor(color.r, color.g, color.b, color.a);
		}
		void SetViewport(Vec2i pos, Vec2i size)
		{
			viewportPos = pos;
			viewportSize = size;
			glViewport(pos.x, pos.y, size.x, size.y);
			viewportChangedDispatcher.Call({ pos, size });
			Graphics::Graphics_ViewportChanged({ pos, size });
		}
		void SetPatchSize(uint size)
		{
			glPatchParameteri(GL_PATCH_VERTICES, size);
		}
		void SetPointSize(uint size)
		{
			glPointSize(size);
		}

		void SetScissorRect(Vec2i pos, Vec2i size)
		{
			glScissor(pos.x, pos.y, size.x, size.y);
		}

		uint GetMaxBoundTextures()
		{
			int num;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
			return uint(num);
		}
		ColorRGBAf GetClearColor()
		{
			return clearColor;
		}
		Vec2i GetViewportPos()
		{
			return viewportPos;
		}
		Vec2i GetViewportSize()
		{
			return viewportSize;
		}
		float GetViewportRatio()
		{
			return (float)viewportSize.x / viewportSize.y;
		}

		void ClearTarget()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void ClearTargetColor()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void ClearTargetDepth()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		void SwapWindowBuffers()
		{
			if (target == nullptr)
				BLAZE_WARNING_LOG("Blaze Engine", "No target was selected!");
			else
				SDL_GL_SwapWindow((SDL_Window*)target);
		}

		void SetTarget(Window& win)
		{
			if (target != win.GetHandle())
			{
				target = win.GetHandle();
				SDL_GL_MakeCurrent((SDL_Window*)win.GetHandle(), (SDL_GLContext)GetOpenGLContext());
			}
		}
		void SetPolygonMode(PolygonMode mode)
		{			
			glPolygonMode(GL_FRONT_AND_BACK, OpenGLPolygonMode(mode));			
		}

		void EnableVSync(bool vsync)
		{
			SDL_GL_SetSwapInterval(vsync);
		}		
		void EnableWritingToDepthBuffer(bool enable)
		{
			glDepthMask(enable);
		}		
		void EnableBlending(bool enable)
		{
			if (enable)			
				glEnable(GL_BLEND);							
			else
				glDisable(GL_BLEND);
		}
		void EnableProgramPointSize(bool enable)
		{
			if (enable)
				glEnable(GL_PROGRAM_POINT_SIZE);
			else
				glDisable(GL_PROGRAM_POINT_SIZE);
		}
		void EnableFaceCulling(bool enable)
		{
			if (enable)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		void EnableDepthTest(bool enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		void EnableScissorTest(bool enable)
		{
			if (enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}		
		void EnableStencilTest(bool enable)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
		
		void RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferOffset)
		{
			glDrawElements(OpenGLRenderPrimitive(type), indexCount, OpenGLIndexType(indexType), (void*)indexBufferOffset);
		}
		void RenderPrimitiveArray(PrimitiveType type, uint startIndex, uint primitiveCount)
		{
			glDrawArrays(OpenGLRenderPrimitive(type), startIndex, primitiveCount);
		}										

		void DispatchCompute(uint x, uint y, uint z)
		{
			glDispatchCompute(x, y, z);
		}

		void MemoryBarrier()
		{
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}

		void SelectImage(uint slot, const Graphics::Core::Texture2D& texture, uint level, ImageAccess access, ImageFormat format)
		{
			glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
		}
		void SelectImage(uint slot, const Graphics::Core::Texture1D& texture, uint level, ImageAccess access, ImageFormat format)
		{
			glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
		}
	}
}