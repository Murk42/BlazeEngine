#pragma once
#include "BlazeEngineGraphics/Core/Dynamic/GraphicsContext.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/SDLOpenGLContext_OpenGL.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	class GraphicsBuffer;
	class Texture1D;
	class Texture2D;	
	class VertexArray;
	class ShaderProgram;
	class Framebuffer;
	class Renderbuffer;
}

namespace Blaze::Graphics::OpenGL
{		

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

		inline WindowSDL::WindowSDLHandle GetActiveWindowSDLHandle() const { return activeWindowSDLHandle; }
		inline SDLOpenGLContext_OpenGL GetSDLOpenGLContext() const { return SDLOpenGLContext; }

		void SelectTexture(OpenGLWrapper::Texture1D*);
		void SelectTexture(OpenGLWrapper::Texture2D*);
		//void SelectTexture(OpenGLWrapper::Texture2DArray*);
		//void SelectTexture(OpenGLWrapper::TextureCubemap*);
		//void SelectTexture(OpenGLWrapper::Texture3D*);
		//void SelectTexture(OpenGLWrapper::TextureBuffer*);
		void SelectVertexBuffer(OpenGLWrapper::GraphicsBuffer* buffer);		
		void SelectUniformBuffer(OpenGLWrapper::GraphicsBuffer* buffer);
		void SelectVertexArray(OpenGLWrapper::VertexArray* vertexArray);
		void SelectProgram(OpenGLWrapper::ShaderProgram* program);
		void SelectFramebuffer(OpenGLWrapper::Framebuffer* framebuffer);
		void SelectDrawFramebuffer(OpenGLWrapper::Framebuffer* framebuffer);
		void SelectReadFramebuffer(OpenGLWrapper::Framebuffer* framebuffer);
		void SelectRenderbuffer(OpenGLWrapper::Renderbuffer* renderbuffer);
		void SelectShaderStorageBufferToSlot(uint slotIndex, OpenGLWrapper::GraphicsBuffer* buffer, uintMem offset, uintMem size);		

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

		void DispatchCompute(uint x, uint y, uint z);

		void Flush();
		void MemoryBarrier();		
	private:
		WindowSDL CreateWindowSDL(WindowSDLCreateOptions_OpenGL createOptions);
		void DestroyWindowSDL(WindowSDL& window);

		uint majorVersion;
		uint minorVersion;
		ProfileType profileType;
		ContextFlags contextFlags;
		ReleaseBehaviour releaseBehaviour;
		uint depthBufferBitCount;		
		uint stencilBufferBitCount;

		SDLOpenGLContext_OpenGL SDLOpenGLContext;

		WindowSDL initWindowSDL;
		WindowSDL::WindowSDLHandle activeWindowSDLHandle;

		friend class RenderWindow_OpenGL;
	};
}