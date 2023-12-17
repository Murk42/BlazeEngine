#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"

#include "BlazeEngineCore/Graphics/GraphicsLibrary.h"

#include "BlazeEngineCore/Graphics/GraphicsEnums.h"
#include "BlazeEngineCore/DataStructures/Rect.h"
#include "BlazeEngineCore/Core/Window.h"

namespace Blaze
{			
	namespace Graphics::Renderer
	{						
		BLAZE_CORE_API uint GetVideoDisplayCount();

		BLAZE_CORE_API uint GetDisplayModeCount(uint videoDisplayIndex);
		BLAZE_CORE_API DisplayMode GetDisplayMode(uint videoDisplayIndex, uint index);

		//See GetDesktopDisplayMode for further information
		BLAZE_CORE_API DisplayMode GetCurrentDisplayMode(uint videoDisplayIndex);

		//There's a difference between this function and GetCurrentDisplayMode() when the program 
		//runs fullscreen and has changed the resolution. In that case this function will return 
		//the previous native display mode, and not the current display mode.
		BLAZE_CORE_API DisplayMode GetDesktopDisplayMode(uint videoDisplayIndex);

		//Returns false if no matching display mode was available;		
		BLAZE_CORE_API bool GetClosestDisplayMode(uint displayIndex, DisplayMode target, DisplayMode& closest);

		BLAZE_CORE_API Result SetActiveTextureSlot(uint slot);
		BLAZE_CORE_API Result BindUniformBuffer(const GraphicsLibrary::GraphicsBuffer& buffer, uint binding);		
		BLAZE_CORE_API Result BindUniformBufferRange(const GraphicsLibrary::GraphicsBuffer& buffer, uint binding, uint offset, uint size);

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::Texture1D*);
		BLAZE_CORE_API GraphicsLibrary::Texture1D* GetSelectedTexture1D();

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::Texture2D*);
		BLAZE_CORE_API GraphicsLibrary::Texture2D* GetSelectedTexture2D();

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::Texture2DArray*);
		BLAZE_CORE_API GraphicsLibrary::Texture2DArray* GetSelectedTexture2DArray();

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::TextureCubemap*);
		BLAZE_CORE_API GraphicsLibrary::TextureCubemap* GetSelectedTextureCubemap();

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::Texture3D*);
		BLAZE_CORE_API GraphicsLibrary::Texture3D* GetSelectedTexture3D();

		BLAZE_CORE_API Result SelectTexture(GraphicsLibrary::TextureBuffer*);
		BLAZE_CORE_API GraphicsLibrary::TextureBuffer* GetTextureBuffer();

		BLAZE_CORE_API Result SelectVertexBuffer(GraphicsLibrary::GraphicsBuffer* buffer);
		BLAZE_CORE_API GraphicsLibrary::GraphicsBuffer* GetSelectedVertexBuffer();

		BLAZE_CORE_API Result SelectIndexBuffer(GraphicsLibrary::GraphicsBuffer* buffer);
		BLAZE_CORE_API GraphicsLibrary::GraphicsBuffer* GetSelectedIndexBuffer();

		BLAZE_CORE_API Result SelectUniformBuffer(GraphicsLibrary::GraphicsBuffer* buffer);
		BLAZE_CORE_API GraphicsLibrary::GraphicsBuffer* GetSelectedUniformBuffer();

		BLAZE_CORE_API Result SelectVertexArray(GraphicsLibrary::VertexArray* vertexArray);		
		BLAZE_CORE_API GraphicsLibrary::VertexArray* GetSelectedVertexArray();

		BLAZE_CORE_API Result SelectProgram(GraphicsLibrary::ShaderProgram* program);		
		BLAZE_CORE_API GraphicsLibrary::ShaderProgram* GetSelectedProgram();

		BLAZE_CORE_API Result SelectFramebuffer(GraphicsLibrary::Framebuffer* framebuffer);
		BLAZE_CORE_API GraphicsLibrary::Framebuffer* GetSelectedFramebuffer();

		BLAZE_CORE_API Result SelectDrawFramebuffer(GraphicsLibrary::Framebuffer* framebuffer);
		BLAZE_CORE_API GraphicsLibrary::Framebuffer* GetDrawSelectedFramebuffer();
		BLAZE_CORE_API Result SelectReadFramebuffer(GraphicsLibrary::Framebuffer* framebuffer);
		BLAZE_CORE_API GraphicsLibrary::Framebuffer* GetReadSelectedFramebuffer();

		BLAZE_CORE_API Result SelectRenderbuffer(GraphicsLibrary::Renderbuffer* renderbuffer);
		BLAZE_CORE_API GraphicsLibrary::Renderbuffer* GetSelectedRenderbuffer();

		BLAZE_CORE_API Result SetClearColor(ColorRGBAf);
		BLAZE_CORE_API Result SetRenderArea(Vec2i pos, Vec2i size);
		BLAZE_CORE_API Result SetPatchSize(uint size);
		BLAZE_CORE_API Result SetPointSize(uint size);
		BLAZE_CORE_API Result SetScissorRect(Vec2i pos, Vec2i size);

		BLAZE_CORE_API uint GetMaxBoundTextures();
		BLAZE_CORE_API ColorRGBAf GetClearColor();
		BLAZE_CORE_API Recti GetRenderArea();
		BLAZE_CORE_API float GetRenderAreaAspectRatio();

		BLAZE_CORE_API Result ClearTarget();
		BLAZE_CORE_API Result ClearTargetColor();
		BLAZE_CORE_API Result ClearTargetDepth();
		BLAZE_CORE_API Result ClearTargetStencil();
		BLAZE_CORE_API Result SwapWindowBuffers();

		BLAZE_CORE_API Result SetTarget(Window& win);
		BLAZE_CORE_API Result SetPolygonMode(PolygonMode mode);
				  
		BLAZE_CORE_API Result EnableVSync(bool enable);
		BLAZE_CORE_API Result EnableProgramPointSize(bool enable);
		BLAZE_CORE_API Result EnableBlending(bool enable);		
		BLAZE_CORE_API Result EnableFaceCulling(bool enable);		
				  
		BLAZE_CORE_API Result EnableDepthTest(bool enable);
		BLAZE_CORE_API Result EnableWritingToDepthBuffer(bool enable);		
		BLAZE_CORE_API Result EnableStencilTest(bool enable);
		BLAZE_CORE_API Result EnableScissorTest(bool enable);
				  
		BLAZE_CORE_API Result SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed);
		BLAZE_CORE_API Result SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask);
				  
		BLAZE_CORE_API Result RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset);
		BLAZE_CORE_API Result RenderPrimitiveArray(PrimitiveType type, uint statIndex, uint vertexCount);		
				  
		BLAZE_CORE_API Result DispatchCompute(uint x, uint y, uint z);
				  
		BLAZE_CORE_API Result MemoryBarrier();
				  
		BLAZE_CORE_API Result SelectImage(uint slot, const GraphicsLibrary::Texture2D& texture, uint level, ImageAccess access, ImageFormat format);		
		BLAZE_CORE_API Result SelectImage(uint slot, const GraphicsLibrary::Texture1D& texture, uint level, ImageAccess access, ImageFormat format);

		namespace Events
		{
			struct RenderAreaChanged
			{
				Vec2i pos;
				Vec2i size;
			};
			BLAZE_CORE_API void AddRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>&);
			BLAZE_CORE_API void RemoveRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>&);
		}
	}
}