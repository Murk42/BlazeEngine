#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Core/Startup.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	void* GetOpenGLInitWindow();
	void* GetOpenGLContext();		

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


	void InitializeCoreGraphics()
	{		
		target = GetOpenGLInitWindow();
		selectedVertexArray = &defaultVertexArray;

		int val[4];
		glGetIntegerv(GL_VIEWPORT, val);
		viewportPos = { val[0], val[1] };
		viewportSize = { val[2], val[3] };		
	}
	void TerminateCoreGraphics()
	{

	}

	namespace Graphics::Core
	{

		template<typename T>
		inline std::underlying_type_t<T> ToInteger(T value)
		{
			return static_cast<std::underlying_type_t<T>>(value);
		}

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
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", 
				"Invalid PrimitiveType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type))));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLIndexType(IndexType type)
		{
			switch (type)
			{
			case IndexType::Uint32: return GL_UNSIGNED_INT;
			case IndexType::Uint16: return GL_UNSIGNED_SHORT;
			case IndexType::Uint8: return GL_UNSIGNED_BYTE;		
			}
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", 
				"Invalid IndexType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type))));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLPolygonMode(PolygonMode mode)
		{
			switch (mode)
			{
			case PolygonMode::Point: return GL_POINT;
			case PolygonMode::Line: return GL_LINE;
			case PolygonMode::Fill: return GL_FILL;		
			}
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", 
				"Invalid PolygonMode. The integer value was: " + StringParsing::Convert(ToInteger(mode))));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLImageAccess(ImageAccess access)
		{
			switch (access)
			{
			case ImageAccess::Read: return GL_READ_ONLY;
			case ImageAccess::Write: return GL_WRITE_ONLY;
			case ImageAccess::ReadWrite: return GL_READ_WRITE;		
			}
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine",
				"Invalid ImageAccess enum value. The integer value was: " + StringParsing::Convert(ToInteger(access))));
			return std::numeric_limits<GLenum>::max();
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
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine",
				"Invalid ImageFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format))));
			return std::numeric_limits<GLenum>::max();
		}		

		GLenum OpenGLStencilOperationType(StencilOperationType type)
		{
			switch (type)
			{
			case Blaze::Graphics::Core::StencilOperationType::Keep: return GL_KEEP;
			case Blaze::Graphics::Core::StencilOperationType::Zero: return GL_ZERO;
			case Blaze::Graphics::Core::StencilOperationType::Replace: return GL_REPLACE;
			case Blaze::Graphics::Core::StencilOperationType::Increase: return GL_INCR;
			case Blaze::Graphics::Core::StencilOperationType::IncreaseWrap: return GL_INCR_WRAP;
			case Blaze::Graphics::Core::StencilOperationType::Decrease: return GL_DECR;
			case Blaze::Graphics::Core::StencilOperationType::DecreaseWrap: return GL_DECR_WRAP;
			case Blaze::Graphics::Core::StencilOperationType::Invert: return GL_INVERT;			
			}					
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine",
				"Invalid StencilOperationType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type))));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLStencilComparison(StencilComparison comparison)
		{
			switch (comparison)
			{
			case Blaze::Graphics::Core::StencilComparison::Never: return GL_NEVER;
			case Blaze::Graphics::Core::StencilComparison::Less: return GL_LESS;
			case Blaze::Graphics::Core::StencilComparison::LessEqual: return GL_LEQUAL;
			case Blaze::Graphics::Core::StencilComparison::Greater: return GL_GREATER;
			case Blaze::Graphics::Core::StencilComparison::GreaterEqual: return GL_GEQUAL;
			case Blaze::Graphics::Core::StencilComparison::Equal: return GL_EQUAL;
			case Blaze::Graphics::Core::StencilComparison::NotEqual: return GL_NOTEQUAL;
			case Blaze::Graphics::Core::StencilComparison::Always: return GL_ALWAYS;			
			}
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", "Invalid StencilComparison enum value. The integer value was: " + StringParsing::Convert(ToInteger(comparison))));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLScreenBufferType(ScreenBufferType type)
		{
			switch (type)
			{
			case Blaze::Graphics::Core::ScreenBufferType::Front: return GL_FRONT;
			case Blaze::Graphics::Core::ScreenBufferType::Back: return GL_BACK;
			case Blaze::Graphics::Core::ScreenBufferType::BackAndFront: return GL_FRONT_AND_BACK;			
			}
			Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", "Invalid ScreenBufferType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type))));
			return std::numeric_limits<GLenum>::max();
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
			Result result;

			SDL_DisplayMode mode;
			SDL_GetDisplayMode(videoDisplayIndex, displayModeIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format, result);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		DisplayMode GetCurrentDisplayMode(uint videoDisplayIndex)
		{
			Result result;

			SDL_DisplayMode mode;
			SDL_GetCurrentDisplayMode(videoDisplayIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format, result);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		DisplayMode GetDesktopDisplayMode(uint videoDisplayIndex)
		{
			Result result;

			SDL_DisplayMode mode;
			SDL_GetDesktopDisplayMode(videoDisplayIndex, &mode);
			DisplayMode out;

			out.format = BlazeDisplayPixelFormat(mode.format, result);
			out.refreshRate = mode.refresh_rate;
			out.size = Vec2i(mode.w, mode.h);
			return out;
		}

		bool GetClosestDisplayMode(uint displayIndex, DisplayMode mode, DisplayMode& out)
		{
			Result result;

			SDL_DisplayMode x;
			x.format = SDLDisplayPixelFormat(mode.format, result);
			x.w = mode.size.x;
			x.h = mode.size.y;
			x.refresh_rate = mode.refreshRate;
			x.driverdata = nullptr;

			SDL_DisplayMode closest;

			if (SDL_GetClosestDisplayMode(displayIndex, &x, &closest) == NULL)
				return false;

			out.format = BlazeDisplayPixelFormat(closest.format, result);
			out.refreshRate = closest.refresh_rate;
			out.size = Vec2i(closest.w, closest.h);
			return true;			
		}

		void SetActiveTextureSlot(uint slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
		}

		void BindUniformBuffer(const GraphicsBuffer& buffer, uint binding)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer.GetHandle());			
		}

		void BindUniformBufferRange(const GraphicsBuffer& buffer, uint binding, uint offset, uint size)
		{
			glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer.GetHandle(), offset, size);
		}

	

		Result SelectTexture(Graphics::Core::Texture1D* obj)
		{
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_1D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture1D = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::Texture1D* GetSelectedTexture1D()
		{
			return selectedTexture1D;
		}

		Result SelectTexture(Graphics::Core::Texture2D* obj)
		{			
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_2D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture2D = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}		
		Graphics::Core::Texture2D* GetSelectedTexture2D()
		{
			return selectedTexture2D;
		}

		Result SelectTexture(Graphics::Core::Texture2DArray* obj)
		{
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_2D_ARRAY, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture2DArray = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}		
		Graphics::Core::Texture2DArray* GetSelectedTexture2DArray()
		{			
			return selectedTexture2DArray;
		}

		Result SelectTexture(Graphics::Core::TextureCubemap* obj)
		{
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_CUBE_MAP, obj == nullptr ? 0 : obj->GetHandle());
			selectedTextureCubemap = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::TextureCubemap* GetSelectedTextureCubemap()
		{
			return selectedTextureCubemap;
		}

		Result SelectTexture(Graphics::Core::Texture3D* obj)
		{
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_3D, obj == nullptr ? 0 : obj->GetHandle());
			selectedTexture3D = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::Texture3D* GetSelectedTexture3D()
		{
			return selectedTexture3D;
		}

		Result SelectTexture(Graphics::Core::TextureBuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindTexture(GL_TEXTURE_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedTextureBuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::TextureBuffer* GetTextureBuffer()
		{
			return selectedTextureBuffer;
		}

		Result SelectVertexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindBuffer(GL_ARRAY_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedArrayBuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedVertexBuffer()
		{
			return selectedArrayBuffer;
		}

		Result SelectIndexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{			
			return ADD_STACK_FRAME(selectedVertexArray->SetIndexBuffer(*obj));
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedIndexBuffer()
		{
			return selectedVertexArray->GetIndexBuffer();
		}

		Result SelectUniformBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindBuffer(GL_UNIFORM_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedUniformBuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::GraphicsBuffer* GetSelectedUniformBuffer()
		{
			return selectedUniformBuffer;
		}

		Result SelectVertexArray(Graphics::Core::VertexArray* obj)
		{
			LoggerListener listener{ true, true };

			glBindVertexArray(obj == nullptr ? 0 : obj->GetHandle());
			selectedVertexArray = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}		
		Graphics::Core::VertexArray* GetSelectedVertexArray()
		{
			return selectedVertexArray;
		}

		Result SelectProgram(Graphics::Core::ShaderProgram* obj)
		{
			LoggerListener listener{ true, true };

			glUseProgram(obj == nullptr ? 0 : obj->GetHandle());
			selectedProgram = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::ShaderProgram* GetSelectedProgram()
		{
			return selectedProgram;
		}

		Result SelectFramebuffer(Graphics::Core::Framebuffer* obj)
		{ 
			LoggerListener listener{ true, true };

			glBindFramebuffer(GL_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedFramebuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}
		Graphics::Core::Framebuffer* GetSelectedFramebuffer()
		{
			return selectedFramebuffer;;
		}
		 
		Result SelectDrawFramebuffer(Graphics::Core::Framebuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedDrawFramebuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}

		Graphics::Core::Framebuffer* GetDrawSelectedFramebuffer()
		{
			return selectedDrawFramebuffer;
		}

		Result SelectReadFramebuffer(Graphics::Core::Framebuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindFramebuffer(GL_READ_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedReadFramebuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
		}

		Graphics::Core::Framebuffer* GetReadSelectedFramebuffer()
		{
			return selectedReadFramebuffer;
		}

		Result SelectRenderbuffer(Graphics::Core::Renderbuffer* obj)
		{
			LoggerListener listener{ true, true };

			glBindRenderbuffer(GL_RENDERBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			selectedRenderbuffer = obj;

			return ADD_STACK_FRAME(listener.ToResult());
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

			engineData->viewportChangedDispatcher.Call({ pos, size });
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
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}
		void ClearTargetColor()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void ClearTargetDepth()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		void ClearTargetStencil()
		{
			glClear(GL_STENCIL_BUFFER_BIT);
		}
		Result SwapWindowBuffers()
		{
			if (target == nullptr)
				return BLAZE_WARNING_RESULT("Blaze Engine", "No target was selected!");
			else
				SDL_GL_SwapWindow((SDL_Window*)target);
			return Result();
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
		void EnableStencilTest(bool enable) 
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
		void EnableScissorTest(bool enable)
		{
			if (enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}	

		void SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed)
		{
			glStencilOpSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilOperationType(bothFail), OpenGLStencilOperationType(depthFailStencilSucceed), OpenGLStencilOperationType(bothSucceed));
		}

		void SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask)
		{									
			glStencilFuncSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilComparison(comparison), reference, mask);
		}
		
		void RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset)
		{
			glDrawElements(OpenGLRenderPrimitive(type), indexCount, OpenGLIndexType(indexType), reinterpret_cast<void*>(static_cast<uintptr_t>(indexBufferByteOffset)));
		}
		void RenderPrimitiveArray(PrimitiveType type, uint startIndex, uint vertexCount)
		{
			glDrawArrays(OpenGLRenderPrimitive(type), startIndex, vertexCount);
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