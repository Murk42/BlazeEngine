 #pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Core/Dynamic/GraphicsContext.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
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
	};	

	//Not moveable because external references to the object might be invalidated
	class BLAZE_GRAPHICS_API GraphicsContext_OpenGL : public Dynamic::GraphicsContextBase
	{		
	public:
		/*Parity*/GraphicsContext_OpenGL();
		/*Parity*/~GraphicsContext_OpenGL() override;

		/*Parity*/inline String GetImplementationName() const override final { return "OpenGL"; }

		GraphicsContext_OpenGL(const GraphicsContextProperties_OpenGL& properties);						

		void SetActiveRenderWindow(RenderWindow_OpenGL&);

		inline void* GetActiveWindowHandle() const { return activeWindowHandle; }
		inline void* GetSDLOpenGLContext() const { return SDLOpenGLContext; }

		void SelectTexture(const OpenGLWrapper::Texture1D*);
		void SelectTexture(const OpenGLWrapper::Texture2D*);
		//void SelectTexture(OpenGLWrapper::Texture2DArray*);
		//void SelectTexture(OpenGLWrapper::TextureCubemap*);
		//void SelectTexture(OpenGLWrapper::Texture3D*);
		//void SelectTexture(OpenGLWrapper::TextureBuffer*);
		void SelectVertexBuffer(const OpenGLWrapper::GraphicsBuffer* buffer);
		void SelectUniformBuffer(const OpenGLWrapper::GraphicsBuffer* buffer);
		void SelectVertexArray(const OpenGLWrapper::VertexArray* vertexArray);
		void SelectProgram(const OpenGLWrapper::ShaderProgram* program);
		void SelectFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectDrawFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectReadFramebuffer(const Framebuffer_OpenGL* framebuffer);
		void SelectRenderbuffer(const OpenGLWrapper::Renderbuffer* renderbuffer);
		void SelectShaderStorageBufferToSlot(uint slotIndex, const OpenGLWrapper::GraphicsBuffer* buffer, uintMem offset, uintMem size);

		void SetActiveTextureSlot(uint slot);

		void BindUniformBuffer(const OpenGLWrapper::GraphicsBuffer& buffer, uint binding);
		void BindUniformBufferRange(const OpenGLWrapper::GraphicsBuffer& buffer, uint binding, uint offset, uint size);
		void SelectImage(uint slot, const OpenGLWrapper::Texture2D& texture, uint level, OpenGLWrapper::ImageAccess access, OpenGLWrapper::ImageFormat format);
		void SelectImage(uint slot, const OpenGLWrapper::Texture1D& texture, uint level, OpenGLWrapper::ImageAccess access, OpenGLWrapper::ImageFormat format);

		void SetClearColor(ColorRGBAf);
		void SetRenderArea(Vec2i pos, Vec2u size);
		void SetPatchSize(uint size);
		void SetPointSize(uint size);
		void SetScissorRect(Vec2i pos, Vec2i size);

		//Returns false if AdaptiveVSync is requirested but it isn't supported, in which case VSync is used. 
		//Otherwise returns true
		bool SetSwapInterval(WindowSwapInterval swapInterval);

		uint GetMaxBoundTextures();		

		void ClearTarget();
		void ClearTargetColor();
		void ClearTargetDepth();
		void ClearTargetStencil();

		void SetPolygonMode(OpenGLWrapper::PolygonMode mode);

		void EnableProgramPointSize(bool enable);
		void EnableBlending(bool enable);
		void EnableFaceCulling(bool enable);
		void EnableDepthTest(bool enable);
		void EnableWritingToDepthBuffer(bool enable);
		void EnableStencilTest(bool enable);
		void EnableScissorTest(bool enable);

		void SetStencilOperation(OpenGLWrapper::ScreenBufferType bufferType, OpenGLWrapper::StencilOperationType bothFail, OpenGLWrapper::StencilOperationType depthFailStencilSucceed, OpenGLWrapper::StencilOperationType bothSucceed);
		void SetStencilFunction(OpenGLWrapper::ScreenBufferType bufferType, OpenGLWrapper::StencilComparison comparison, int reference, uint mask);

		void RenderIndexedPrimitives(OpenGLWrapper::PrimitiveType type, OpenGLWrapper::IndexType indexType, uintMem indexCount, uintMem indexBufferByteOffset);
		void RenderInstancedPrimitiveArray(OpenGLWrapper::PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount, uintMem firstInstanceIndex, uintMem instanceCount);
		void RenderPrimitiveArray(OpenGLWrapper::PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount);

		void BlitFramebuffer(Framebuffer_OpenGL& writeFramebuffer, Framebuffer_OpenGL& readFramebuffer, Vec2i dstP1, Vec2i dstP2, Vec2i srcP1, Vec2i srcP2, bool copyColor, bool copyDepth, bool copyStencil, OpenGLWrapper::TextureSampling sampling);

		void DispatchCompute(uint x, uint y, uint z);

		void Flush();
		void MemoryBarrier();		

		static bool IsExtensionSupported(StringView name);
	private: 
		GraphicsContextProperties_OpenGL properties;

		void* SDLOpenGLContext;

		void* initWindow;
		void* activeWindowHandle;
		
		void ActiveWindowDestroyed(const Window::WindowDestroyedEvent& event);

		friend class RenderWindow_OpenGL;
	};
}