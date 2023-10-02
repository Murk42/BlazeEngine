#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

#include "BlazeEngine/Core/Window.h"

#include "BlazeEngine/Internal/Conversions.h"

#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	SDL_Window* GetInitWindow();

#ifdef BLAZE_GRAPHICS_OPENGL
	void* GetOpenGLContext();		
	namespace Debug::Logger
	{
		void OpenGLCallbackFunc(unsigned source, unsigned type, int id, unsigned severity, unsigned lenght, const char* message);
	}
#endif

	namespace Graphics::Renderer
	{
#ifdef BLAZE_GRAPHICS_OPENGL 		
		
		static OpenGL::VertexArray defaultVertexArray;
		static OpenGL::VertexArray* selectedVertexArray = nullptr;
		static OpenGL::GraphicsBuffer* selectedArrayBuffer = nullptr;
		static OpenGL::GraphicsBuffer* selectedUniformBuffer = nullptr;
		static OpenGL::ShaderProgram* selectedProgram = nullptr;
		static OpenGL::Renderbuffer* selectedRenderbuffer = nullptr;
		static OpenGL::Framebuffer* selectedFramebuffer = nullptr;
		static OpenGL::Framebuffer* selectedDrawFramebuffer = nullptr;
		static OpenGL::Framebuffer* selectedReadFramebuffer = nullptr;
			   
		static OpenGL::Texture1D* selectedTexture1D = nullptr;
		static OpenGL::Texture2D* selectedTexture2D = nullptr;
		static OpenGL::Texture3D* selectedTexture3D = nullptr;
		static OpenGL::Texture2DArray* selectedTexture2DArray = nullptr;
		static OpenGL::TextureCubemap* selectedTextureCubemap = nullptr;
		static OpenGL::TextureBuffer* selectedTextureBuffer = nullptr;
#endif		
		static SDL_Window* targetWindow = nullptr;

		static Recti renderArea;
		static EventDispatcher<Events::RenderAreaChanged> renderAreaChangedDispatcher;

		static ColorRGBAf clearColor;
	}


	TimingResult InitializeCoreGraphics()
	{		
		using namespace Graphics::Renderer;
		Timing timing{ "Graphics core" };

		targetWindow = GetInitWindow();
		selectedVertexArray = &defaultVertexArray;
		
		renderArea.pos = Vec2i();
		SDL_GetWindowSize(targetWindow, &renderArea.size.x, &renderArea.size.y);
		
#ifdef BLAZE_GRAPHICS_OPENGL
		//Enable callback debug
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)Debug::Logger::OpenGLCallbackFunc, nullptr);

		//Default settings
		glFrontFace(GL_CW);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, renderArea.size.x, renderArea.size.y);		
#endif		

		return timing.GetTimingResult();
	}
	void TerminateCoreGraphics()
	{

	}

	namespace Graphics::Renderer
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
			Debug::Logger::LogError("Blaze Engine", "Invalid PrimitiveType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
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
			Debug::Logger::LogError("Blaze Engine", "Invalid IndexType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
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
			Debug::Logger::LogError("Blaze Engine", "Invalid PolygonMode. The integer value was: " + StringParsing::Convert(ToInteger(mode)));
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
			Debug::Logger::LogError("Blaze Engine", "Invalid ImageAccess enum value. The integer value was: " + StringParsing::Convert(ToInteger(access)));
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
			Debug::Logger::LogError("Blaze Engine", "Invalid ImageFormat enum value. The integer value was: " + StringParsing::Convert(ToInteger(format)));
			return std::numeric_limits<GLenum>::max();
		}		

		GLenum OpenGLStencilOperationType(StencilOperationType type)
		{
			switch (type)
			{
			case StencilOperationType::Keep: return GL_KEEP;
			case StencilOperationType::Zero: return GL_ZERO;
			case StencilOperationType::Replace: return GL_REPLACE;
			case StencilOperationType::Increase: return GL_INCR;
			case StencilOperationType::IncreaseWrap: return GL_INCR_WRAP;
			case StencilOperationType::Decrease: return GL_DECR;
			case StencilOperationType::DecreaseWrap: return GL_DECR_WRAP;
			case StencilOperationType::Invert: return GL_INVERT;			
			}					
			Debug::Logger::LogError("Blaze Engine", "Invalid StencilOperationType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLStencilComparison(StencilComparison comparison)
		{
			switch (comparison)
			{
			case StencilComparison::Never: return GL_NEVER;
			case StencilComparison::Less: return GL_LESS;
			case StencilComparison::LessEqual: return GL_LEQUAL;
			case StencilComparison::Greater: return GL_GREATER;
			case StencilComparison::GreaterEqual: return GL_GEQUAL;
			case StencilComparison::Equal: return GL_EQUAL;
			case StencilComparison::NotEqual: return GL_NOTEQUAL;
			case StencilComparison::Always: return GL_ALWAYS;			
			}
			Debug::Logger::LogError("Blaze Engine", "Invalid StencilComparison enum value. The integer value was: " + StringParsing::Convert(ToInteger(comparison)));
			return std::numeric_limits<GLenum>::max();
		}
		GLenum OpenGLScreenBufferType(ScreenBufferType type)
		{
			switch (type)
			{
			case ScreenBufferType::Front: return GL_FRONT;
			case ScreenBufferType::Back: return GL_BACK;
			case ScreenBufferType::BackAndFront: return GL_FRONT_AND_BACK;			
			}
			Debug::Logger::LogError("Blaze Engine", "Invalid ScreenBufferType enum value. The integer value was: " + StringParsing::Convert(ToInteger(type)));
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

		Result SetActiveTextureSlot(uint slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			CHECK_OPENGL_RESULT();

			return Result();
		}

		Result BindUniformBuffer(const GraphicsLibrary::GraphicsBuffer& buffer, uint binding)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer.GetHandle());			
			CHECK_OPENGL_RESULT();

			return Result();
		}

		Result BindUniformBufferRange(const GraphicsLibrary::GraphicsBuffer& buffer, uint binding, uint offset, uint size)
		{
			glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer.GetHandle(), offset, size);
			CHECK_OPENGL_RESULT();

			return Result();
		}

	

		Result SelectTexture(GraphicsLibrary::Texture1D* obj)
		{
			glBindTexture(GL_TEXTURE_1D, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTexture1D = obj;

			return Result();
		}
		GraphicsLibrary::Texture1D* GetSelectedTexture1D()
		{
			return selectedTexture1D;
		}

		Result SelectTexture(GraphicsLibrary::Texture2D* obj)
		{			
			glBindTexture(GL_TEXTURE_2D, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTexture2D = obj;

			return Result();
		}		
		GraphicsLibrary::Texture2D* GetSelectedTexture2D()
		{
			return selectedTexture2D;
		}

		Result SelectTexture(GraphicsLibrary::Texture2DArray* obj)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTexture2DArray = obj;

			return Result();
		}		
		GraphicsLibrary::Texture2DArray* GetSelectedTexture2DArray()
		{			
			return selectedTexture2DArray;
		}

		Result SelectTexture(GraphicsLibrary::TextureCubemap* obj)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTextureCubemap = obj;

			return Result();
		}
		GraphicsLibrary::TextureCubemap* GetSelectedTextureCubemap()
		{
			return selectedTextureCubemap;
		}

		Result SelectTexture(GraphicsLibrary::Texture3D* obj)
		{
			glBindTexture(GL_TEXTURE_3D, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTexture3D = obj;

			return Result();
		}
		GraphicsLibrary::Texture3D* GetSelectedTexture3D()
		{
			return selectedTexture3D;
		}

		Result SelectTexture(GraphicsLibrary::TextureBuffer* obj)
		{
			glBindTexture(GL_TEXTURE_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedTextureBuffer = obj;

			return Result();
		}
		GraphicsLibrary::TextureBuffer* GetTextureBuffer()
		{
			return selectedTextureBuffer;
		}

		Result SelectVertexBuffer(GraphicsLibrary::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_ARRAY_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedArrayBuffer = obj;

			return Result();
		}		
		GraphicsLibrary::GraphicsBuffer* GetSelectedVertexBuffer()
		{
			return selectedArrayBuffer;
		}

		Result SelectIndexBuffer(GraphicsLibrary::GraphicsBuffer* obj)
		{			
			return selectedVertexArray->SetIndexBuffer(*obj);
		}		
		GraphicsLibrary::GraphicsBuffer* GetSelectedIndexBuffer()
		{
			return selectedVertexArray->GetIndexBuffer();
		}

		Result SelectUniformBuffer(GraphicsLibrary::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedUniformBuffer = obj;

			return Result();
		}
		GraphicsLibrary::GraphicsBuffer* GetSelectedUniformBuffer()
		{
			return selectedUniformBuffer;
		}

		Result SelectVertexArray(GraphicsLibrary::VertexArray* obj)
		{
			glBindVertexArray(obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedVertexArray = obj;

			return Result();
		}		
		GraphicsLibrary::VertexArray* GetSelectedVertexArray()
		{
			return selectedVertexArray;
		}

		Result SelectProgram(GraphicsLibrary::ShaderProgram* obj)
		{			
			glUseProgram(obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();

			selectedProgram = obj;

			return Result();
		}
		GraphicsLibrary::ShaderProgram* GetSelectedProgram()
		{
			return selectedProgram;
		}

		Result SelectFramebuffer(GraphicsLibrary::Framebuffer* obj)
		{ 
			glBindFramebuffer(GL_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedFramebuffer = obj;

			return Result();
		}
		GraphicsLibrary::Framebuffer* GetSelectedFramebuffer()
		{
			return selectedFramebuffer;;
		}
		 
		Result SelectDrawFramebuffer(GraphicsLibrary::Framebuffer* obj)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedDrawFramebuffer = obj;

			return Result();
		}

		GraphicsLibrary::Framebuffer* GetDrawSelectedFramebuffer()
		{
			return selectedDrawFramebuffer;
		}

		Result SelectReadFramebuffer(GraphicsLibrary::Framebuffer* obj)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();

			selectedReadFramebuffer = obj;

			return Result();
		}

		GraphicsLibrary::Framebuffer* GetReadSelectedFramebuffer()
		{
			return selectedReadFramebuffer;
		}

		Result SelectRenderbuffer(GraphicsLibrary::Renderbuffer* obj)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			CHECK_OPENGL_RESULT();
			selectedRenderbuffer = obj;

			return Result();
		}
		GraphicsLibrary::Renderbuffer* GetSelectedRenderbuffer()
		{
			return selectedRenderbuffer;
		}


		Result SetClearColor(ColorRGBAf color)
		{
			clearColor = color;
			glClearColor(color.r, color.g, color.b, color.a);
			CHECK_OPENGL_RESULT();

			return Result();
		}
		Result SetRenderArea(Vec2i pos, Vec2i size)
		{
			renderArea = { pos, size };
			glViewport(pos.x, pos.y, size.x, size.y);
			CHECK_OPENGL_RESULT();
			
			CHECK_RESULT(renderAreaChangedDispatcher.Call({ pos, size }));			

			return Result();
		}
		Result SetPatchSize(uint size)
		{
			glPatchParameteri(GL_PATCH_VERTICES, size);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result SetPointSize(uint size)
		{
			glPointSize(size);
			CHECK_OPENGL_RESULT();
			return Result();
		}

		Result SetScissorRect(Vec2i pos, Vec2i size)
		{
			glScissor(pos.x, pos.y, size.x, size.y);
			CHECK_OPENGL_RESULT();
			return Result();
		}

		uint GetMaxBoundTextures()
		{
			int num;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
			FlushOpenGLResult();

			return uint(num);
		}
		ColorRGBAf GetClearColor()
		{
			return clearColor;
		}
		Recti GetRenderArea()
		{			
			return renderArea;
		}

		float GetRenderAreaAspectRatio()
		{
			return (float)renderArea.size.x / renderArea.size.y;
		}

		Result ClearTarget()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result ClearTargetColor()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result ClearTargetDepth()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result ClearTargetStencil()
		{
			glClear(GL_STENCIL_BUFFER_BIT);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result SwapWindowBuffers()
		{
			if (targetWindow == nullptr)
				return BLAZE_WARNING_RESULT("Blaze Engine", "No target was selected!");
			else
				SDL_GL_SwapWindow(targetWindow);
			return Result();
		}

		Result SetTarget(Window& win)
		{
			if (targetWindow != (SDL_Window*)win.GetHandle())
			{
				targetWindow = (SDL_Window*)win.GetHandle();
					(targetWindow, (SDL_GLContext)GetOpenGLContext());
			}

			return Result();
		}
		Result SetPolygonMode(PolygonMode mode)
		{			
			glPolygonMode(GL_FRONT_AND_BACK, OpenGLPolygonMode(mode));			
			CHECK_OPENGL_RESULT();
			return Result();
		}

		Result EnableVSync(bool vsync)
		{
			SDL_GL_SetSwapInterval(vsync);
			CHECK_OPENGL_RESULT();
			return Result();
		}		
		Result EnableWritingToDepthBuffer(bool enable)
		{
			glDepthMask(enable);
			CHECK_OPENGL_RESULT();
			return Result();
		}		
		Result EnableBlending(bool enable)
		{
			if (enable)			
				glEnable(GL_BLEND);							
			else
				glDisable(GL_BLEND);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result EnableProgramPointSize(bool enable)
		{
			if (enable)
				glEnable(GL_PROGRAM_POINT_SIZE);
			else
				glDisable(GL_PROGRAM_POINT_SIZE);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result EnableFaceCulling(bool enable)
		{
			if (enable)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result EnableDepthTest(bool enable)
		{
			if (enable)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result EnableStencilTest(bool enable)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result EnableScissorTest(bool enable)
		{
			if (enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
			CHECK_OPENGL_RESULT();
			return Result();
		}	

		Result SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed)
		{
			glStencilOpSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilOperationType(bothFail), OpenGLStencilOperationType(depthFailStencilSucceed), OpenGLStencilOperationType(bothSucceed));
			CHECK_OPENGL_RESULT();
			return Result();
		}

		Result SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask)
		{									
			glStencilFuncSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilComparison(comparison), reference, mask);
			CHECK_OPENGL_RESULT();
			return Result();
		}
		
		Result RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset)
		{
			glDrawElements(OpenGLRenderPrimitive(type), indexCount, OpenGLIndexType(indexType), reinterpret_cast<void*>(static_cast<uintptr_t>(indexBufferByteOffset)));
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result RenderPrimitiveArray(PrimitiveType type, uint startIndex, uint vertexCount)
		{
			glDrawArrays(OpenGLRenderPrimitive(type), startIndex, vertexCount);
			CHECK_OPENGL_RESULT();
			return Result();
		}										

		Result DispatchCompute(uint x, uint y, uint z)
		{
			glDispatchCompute(x, y, z);
			CHECK_OPENGL_RESULT();
			return Result();
		}

		Result MemoryBarrier()
		{
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
			CHECK_OPENGL_RESULT();
			return Result();
		}

		Result SelectImage(uint slot, const GraphicsLibrary::Texture2D& texture, uint level, ImageAccess access, ImageFormat format)
		{
			glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
			CHECK_OPENGL_RESULT();
			return Result();
		}
		Result SelectImage(uint slot, const GraphicsLibrary::Texture1D& texture, uint level, ImageAccess access, ImageFormat format)
		{
			glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
			CHECK_OPENGL_RESULT();
			return Result();
		}

		namespace Events
		{
			void AddRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>& handler)
			{
				renderAreaChangedDispatcher.AddHandler(handler);
			}
			void RemoveRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>& handler)
			{
				renderAreaChangedDispatcher.RemoveHandler(handler);
			}
		}
	}
}