#pragma once
#include "BlazeEngine/DataStructures/Color.h"

#include "BlazeEngine/Graphics/GraphicsLibraryDeclaration.h"

#include "BlazeEngine/Graphics/GraphicsEnums.h"

namespace Blaze
{		
	class Window;

	struct DisplayMode
	{
		DisplayPixelFormat format = DisplayPixelFormat::RGB24;
		Vec2i size = Vec2i(1920, 1080);
		uint refreshRate = 60;
	};	

	namespace Graphics::Core
	{				
		BLAZE_API uint GetVideoDisplayCount();

		BLAZE_API uint GetDisplayModeCount(uint videoDisplayIndex);
		BLAZE_API DisplayMode GetDisplayMode(uint videoDisplayIndex, uint index);

		//See GetDesktopDisplayMode for further information
		BLAZE_API DisplayMode GetCurrentDisplayMode(uint videoDisplayIndex);

		//There's a difference between this function and GetCurrentDisplayMode() when the program 
		//runs fullscreen and has changed the resolution. In that case this function will return 
		//the previous native display mode, and not the current display mode.
		BLAZE_API DisplayMode GetDesktopDisplayMode(uint videoDisplayIndex);

		//Returns false if no matching display mode was available;		
		BLAZE_API bool GetClosestDisplayMode(uint displayIndex, DisplayMode target, DisplayMode& closest);

		BLAZE_API void SetActiveTextureSlot(uint slot);
		BLAZE_API void BindUniformBuffer(const GraphicsBuffer& buffer, uint binding);		
		BLAZE_API void BindUniformBufferRange(const GraphicsBuffer& buffer, uint binding, uint offset, uint size);

		BLAZE_API Result SelectTexture(Graphics::Core::Texture1D*);
		BLAZE_API Graphics::Core::Texture1D* GetSelectedTexture1D();

		BLAZE_API Result SelectTexture(Graphics::Core::Texture2D*);
		BLAZE_API Graphics::Core::Texture2D* GetSelectedTexture2D();

		BLAZE_API Result SelectTexture(Graphics::Core::Texture2DArray*);
		BLAZE_API Graphics::Core::Texture2DArray* GetSelectedTexture2DArray();

		BLAZE_API Result SelectTexture(Graphics::Core::TextureCubemap*);
		BLAZE_API Graphics::Core::TextureCubemap* GetSelectedTextureCubemap();

		BLAZE_API Result SelectTexture(Graphics::Core::Texture3D*);
		BLAZE_API Graphics::Core::Texture3D* GetSelectedTexture3D();

		BLAZE_API Result SelectTexture(Graphics::Core::TextureBuffer*);
		BLAZE_API Graphics::Core::TextureBuffer* GetTextureBuffer();

		BLAZE_API Result SelectVertexBuffer(Graphics::Core::GraphicsBuffer* buffer);
		BLAZE_API Graphics::Core::GraphicsBuffer* GetSelectedVertexBuffer();

		BLAZE_API Result SelectIndexBuffer(Graphics::Core::GraphicsBuffer* buffer);
		BLAZE_API Graphics::Core::GraphicsBuffer* GetSelectedIndexBuffer();

		BLAZE_API Result SelectUniformBuffer(Graphics::Core::GraphicsBuffer* buffer);
		BLAZE_API Graphics::Core::GraphicsBuffer* GetSelectedUniformBuffer();

		BLAZE_API Result SelectVertexArray(Graphics::Core::VertexArray* vertexArray);		
		BLAZE_API Graphics::Core::VertexArray* GetSelectedVertexArray();

		BLAZE_API Result SelectProgram(Graphics::Core::ShaderProgram* program);		
		BLAZE_API Graphics::Core::ShaderProgram* GetSelectedProgram();

		BLAZE_API Result SelectFramebuffer(Graphics::Core::Framebuffer* framebuffer);
		BLAZE_API Graphics::Core::Framebuffer* GetSelectedFramebuffer();

		BLAZE_API Result SelectDrawFramebuffer(Graphics::Core::Framebuffer* framebuffer);
		BLAZE_API Graphics::Core::Framebuffer* GetDrawSelectedFramebuffer();
		BLAZE_API Result SelectReadFramebuffer(Graphics::Core::Framebuffer* framebuffer);
		BLAZE_API Graphics::Core::Framebuffer* GetReadSelectedFramebuffer();

		BLAZE_API Result SelectRenderbuffer(Graphics::Core::Renderbuffer* renderbuffer);
		BLAZE_API Graphics::Core::Renderbuffer* GetSelectedRenderbuffer();

		BLAZE_API void SetClearColor(ColorRGBAf);
		BLAZE_API void SetViewport(Vec2i pos, Vec2i size);
		BLAZE_API void SetPatchSize(uint size);
		BLAZE_API void SetPointSize(uint size);
		BLAZE_API void SetScissorRect(Vec2i pos, Vec2i size);

		BLAZE_API uint GetMaxBoundTextures();
		BLAZE_API ColorRGBAf GetClearColor();
		BLAZE_API Vec2i GetViewportPos();
		BLAZE_API Vec2i GetViewportSize(); 
		BLAZE_API float GetViewportRatio();

		BLAZE_API void ClearTarget();
		BLAZE_API void ClearTargetColor();
		BLAZE_API void ClearTargetDepth();
		BLAZE_API void ClearTargetStencil();
		BLAZE_API Result SwapWindowBuffers();

		BLAZE_API void SetTarget(Window& win);
		BLAZE_API void SetPolygonMode(PolygonMode mode);

		BLAZE_API void EnableVSync(bool enable);
		BLAZE_API void EnableProgramPointSize(bool enable);
		BLAZE_API void EnableBlending(bool enable);		
		BLAZE_API void EnableFaceCulling(bool enable);		

		BLAZE_API void EnableDepthTest(bool enable);
		BLAZE_API void EnableWritingToDepthBuffer(bool enable);		
		BLAZE_API void EnableStencilTest(bool enable);
		BLAZE_API void EnableScissorTest(bool enable);

		BLAZE_API void SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed);
		BLAZE_API void SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask);
		
		BLAZE_API void RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset);
		BLAZE_API void RenderPrimitiveArray(PrimitiveType type, uint statIndex, uint vertexCount);		

		BLAZE_API void DispatchCompute(uint x, uint y, uint z);

		BLAZE_API void MemoryBarrier();

		BLAZE_API void SelectImage(uint slot, const Graphics::Core::Texture2D& texture, uint level, ImageAccess access, ImageFormat format);		
		BLAZE_API void SelectImage(uint slot, const Graphics::Core::Texture1D& texture, uint level, ImageAccess access, ImageFormat format);
	}
}