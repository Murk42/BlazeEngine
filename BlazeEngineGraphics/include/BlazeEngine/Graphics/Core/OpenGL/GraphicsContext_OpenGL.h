 #pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/Graphics/Core/Dynamic/GraphicsContext.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsBuffer;
	class Texture1D;
	class Texture2D;
	class VertexArray;
	class ShaderProgram;
	class Renderbuffer;
}

namespace Blaze::Graphics::OpenGL
{
	class RenderWindow_OpenGL;
	class Framebuffer_OpenGL;

	enum class ProfileType
	{
		Core, Compatibility, ES
	};
	enum class ContextFlags
	{
		None = 0, Debug = 1, ForwardCompatible = 2, RobustAccess = 4, ResetIsolation = 8, Default = 16
	};
	enum class ReleaseBehaviour
	{
		None, Flush
	};

	enum class WindowSwapInterval
	{
		None,
		VSync,
		AdaptiveVSync
	};


	ENUM_CLASS_BITWISE_OPERATIONS(ContextFlags)


	struct GraphicsContextProperties_OpenGL
	{
		uint majorVersion = 4;
		uint minorVersion = 6;
		ProfileType profileType = ProfileType::Core;
		ContextFlags contextFlags = ContextFlags::None;// = ContextFlags::Debug | ContextFlags::RobustAccess | ContextFlags::ForwardCompatible | ContextFlags::ResetIsolation;
		ReleaseBehaviour releaseBehaviour = ReleaseBehaviour::Flush;

		uint depthBufferBitCount = 16;

		uint stencilBufferBitCount = 0;

		bool useSeparateInitWindow = false;
		//If not using a separate init window these are the options the init window will be created with. The
		//first render window that is created by the user will use immutable window options provided in this
		//variable (e.g. utility window, transparent window)
		WindowCreateOptions initWindowCreateOptions = { };
	};

	//Not moveable because external references to the object might be invalidated
	class BLAZE_API GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
	{
	public:
		/*Parity*/GraphicsContext_OpenGL();
		/*Parity*/~GraphicsContext_OpenGL() override;

		/*Parity*/inline String GetImplementationName() const override final { return "OpenGL"; }

		GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties);

		void MakeContextActive();
		void SetActiveRenderWindow(RenderWindow_OpenGL&);

		inline void* GetActiveWindowHandle() const { return activeWindowHandle; }
		inline void* GetSDLOpenGLContext() const { return SDLOpenGLContext; }

		void SelectTexture(const OpenGL::Texture1D*);
		void SelectTexture(const OpenGL::Texture2D*);
		//void SelectTexture(OpenGL::Texture2DArray*);
		//void SelectTexture(OpenGL::TextureCubemap*);
		//void SelectTexture(OpenGL::Texture3D*);
		//void SelectTexture(OpenGL::TextureBuffer*);
		void SelectVertexBuffer(const OpenGL::GraphicsBuffer* buffer);
		void SelectUniformBuffer(const OpenGL::GraphicsBuffer* buffer);
		void SelectVertexArray(const OpenGL::VertexArray* vertexArray);
		void SelectProgram(const OpenGL::ShaderProgram* program);
		void SelectFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectDrawFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectReadFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectRenderbuffer(const OpenGL::Renderbuffer* renderbuffer);
		void SelectShaderStorageBufferToSlot(uint slotIndex, const OpenGL::GraphicsBuffer* buffer, uintMem offset, uintMem size);

		void SetActiveTextureSlot(uint slot);

		void BindUniformBuffer(const OpenGL::GraphicsBuffer& buffer, uint bindingPoint);
		void BindUniformBufferRange(const OpenGL::GraphicsBuffer& buffer, uint bindingPoint, uint offset, uint size);
		void SetShaderUniformBlockBindingPoint(const ShaderProgram& program, uintMem uniformBlockIndex, uint bindingPoint);

		void SelectImage(uint slot, const OpenGL::Texture2D& texture, uint level, OpenGL::ImageAccess access, OpenGL::ImageFormat format);
		void SelectImage(uint slot, const OpenGL::Texture1D& texture, uint level, OpenGL::ImageAccess access, OpenGL::ImageFormat format);

		void SetClearColor(ColorRGBAf);
		void SetRenderArea(Vec2i pos, Vec2u size);
		void SetPatchSize(uint size);
		void SetPointSize(uint size);
		void SetScissorRect(Vec2i pos, Vec2i size);

		//Returns false if AdaptiveVSync is requirested but it isn't supported, in which case VSync is used.
		//Otherwise returns true
		bool SetSwapInterval(WindowSwapInterval swapInterval);

		uint GetMaxBoundTextures();

		void SetPolygonMode(OpenGL::PolygonMode mode);

		void EnableProgramPointSize(bool enable);
		void EnableBlending(bool enable);
		void EnableFaceCulling(bool enable);
		void EnableDepthTest(bool enable);
		void EnableWritingToDepthBuffer(bool enable);
		void EnableStencilTest(bool enable);
		void EnableScissorTest(bool enable);

		void SetStencilOperation(OpenGL::ScreenBufferType bufferType, OpenGL::StencilOperationType bothFail, OpenGL::StencilOperationType depthFailStencilSucceed, OpenGL::StencilOperationType bothSucceed);
		void SetStencilFunction(OpenGL::ScreenBufferType bufferType, OpenGL::StencilComparison comparison, int reference, uint mask);

		void RenderIndexedPrimitives(OpenGL::PrimitiveType type, OpenGL::IndexType indexType, uintMem indexCount, uintMem indexBufferByteOffset);
		void RenderInstancedPrimitiveArray(OpenGL::PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount, uintMem firstInstanceIndex, uintMem instanceCount);
		void RenderPrimitiveArray(OpenGL::PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount);

		void BlitFramebuffer(Framebuffer_OpenGL& writeFramebuffer, Framebuffer_OpenGL& readFramebuffer, Vec2i dstP1, Vec2i dstP2, Vec2i srcP1, Vec2i srcP2, bool copyColor, bool copyDepth, bool copyStencil, OpenGL::TextureSampling sampling);

		void DispatchCompute(uint x, uint y, uint z);

		void Flush();
		void MemoryBarrier();

		Window CreateWindow(const WindowCreateOptions& options);
		void RetrieveWindow(Window& window);

		static bool IsExtensionSupported(StringView name);
	private:
		GraphicsContextProperties_OpenGL properties;

		void* SDLOpenGLContext;

		Window initWindow;
		void* activeWindowHandle;

		void ActiveWindowDestroyed(const Window::DestructionEvent& event);
	};
}