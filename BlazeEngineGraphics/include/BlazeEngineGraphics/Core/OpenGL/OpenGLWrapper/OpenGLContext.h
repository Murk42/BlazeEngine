#pragma once
#include "OpenGLCommon.h"
#include "OpenGLGraphicsBuffer.h"
#include "OpenGLProgram.h"
#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLRenderbuffer.h"
#include "Textures/OpenGLTexture1D.h"
#include "Textures/OpenGLTexture2D.h"
#include "Textures/OpenGLTexture3D.h"
#include "Textures/OpenGLTexture2DArray.h"
#include "Textures/OpenGLTextureCubemap.h"
#include "Textures/OpenGLTextureBuffer.h"
#include "Textures/OpenGLTextureEnums.h"
#include "OpenGLFence.h"
#include "OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{			
	struct RenderAreaChangedEvent
	{
		Vec2i pos;
		Vec2u size;
	};		

	BLAZE_GRAPHICS_API  Result SetActiveTextureSlot(uint slot);
	BLAZE_GRAPHICS_API  Result BindUniformBuffer(const GraphicsBuffer& buffer, uint binding);
	BLAZE_GRAPHICS_API  Result BindUniformBufferRange(const GraphicsBuffer& buffer, uint binding, uint offset, uint size);

	BLAZE_GRAPHICS_API  Result SelectTexture(Texture1D*);
	BLAZE_GRAPHICS_API  Texture1D* GetSelectedTexture1D();

	BLAZE_GRAPHICS_API  Result SelectTexture(Texture2D*);
	BLAZE_GRAPHICS_API  Texture2D* GetSelectedTexture2D();

	BLAZE_GRAPHICS_API  Result SelectTexture(Texture2DArray*);
	BLAZE_GRAPHICS_API  Texture2DArray* GetSelectedTexture2DArray();

	BLAZE_GRAPHICS_API  Result SelectTexture(TextureCubemap*);
	BLAZE_GRAPHICS_API  TextureCubemap* GetSelectedTextureCubemap();

	BLAZE_GRAPHICS_API  Result SelectTexture(Texture3D*);
	BLAZE_GRAPHICS_API  Texture3D* GetSelectedTexture3D();

	BLAZE_GRAPHICS_API  Result SelectTexture(TextureBuffer*);
	BLAZE_GRAPHICS_API  TextureBuffer* GetTextureBuffer();

	BLAZE_GRAPHICS_API  Result SelectVertexBuffer(GraphicsBuffer* buffer);
	BLAZE_GRAPHICS_API  GraphicsBuffer* GetSelectedVertexBuffer();

	BLAZE_GRAPHICS_API  Result SelectIndexBuffer(GraphicsBuffer* buffer);
	BLAZE_GRAPHICS_API  GraphicsBuffer* GetSelectedIndexBuffer();

	BLAZE_GRAPHICS_API  Result SelectUniformBuffer(GraphicsBuffer* buffer);
	BLAZE_GRAPHICS_API  GraphicsBuffer* GetSelectedUniformBuffer();

	BLAZE_GRAPHICS_API  Result SelectVertexArray(VertexArray* vertexArray);
	BLAZE_GRAPHICS_API  VertexArray* GetSelectedVertexArray();

	BLAZE_GRAPHICS_API  Result SelectProgram(ShaderProgram* program);
	BLAZE_GRAPHICS_API  ShaderProgram* GetSelectedProgram();

	BLAZE_GRAPHICS_API  Result SelectFramebuffer(Framebuffer* framebuffer);
	BLAZE_GRAPHICS_API  Framebuffer* GetSelectedFramebuffer();

	BLAZE_GRAPHICS_API  Result SelectDrawFramebuffer(Framebuffer* framebuffer);
	BLAZE_GRAPHICS_API  Framebuffer* GetDrawSelectedFramebuffer();
	BLAZE_GRAPHICS_API  Result SelectReadFramebuffer(Framebuffer* framebuffer);
	BLAZE_GRAPHICS_API  Framebuffer* GetReadSelectedFramebuffer();

	BLAZE_GRAPHICS_API  Result SelectRenderbuffer(Renderbuffer* renderbuffer);
	BLAZE_GRAPHICS_API  Renderbuffer* GetSelectedRenderbuffer();

	BLAZE_GRAPHICS_API  Result SetClearColor(ColorRGBAf);
	BLAZE_GRAPHICS_API  Result SetRenderArea(Vec2i pos, Vec2u size);
	BLAZE_GRAPHICS_API  Result SetPatchSize(uint size);
	BLAZE_GRAPHICS_API  Result SetPointSize(uint size);
	BLAZE_GRAPHICS_API  Result SetScissorRect(Vec2i pos, Vec2i size);

	BLAZE_GRAPHICS_API  uint GetMaxBoundTextures();
	BLAZE_GRAPHICS_API  ColorRGBAf GetClearColor();
	BLAZE_GRAPHICS_API  Recti GetRenderArea();
	BLAZE_GRAPHICS_API  float GetRenderAreaAspectRatio();

	BLAZE_GRAPHICS_API  Result ClearTarget();
	BLAZE_GRAPHICS_API  Result ClearTargetColor();
	BLAZE_GRAPHICS_API  Result ClearTargetDepth();
	BLAZE_GRAPHICS_API  Result ClearTargetStencil();

	BLAZE_GRAPHICS_API  Result SetPolygonMode(PolygonMode mode);

	BLAZE_GRAPHICS_API  Result EnableProgramPointSize(bool enable);
	BLAZE_GRAPHICS_API  Result EnableBlending(bool enable);
	BLAZE_GRAPHICS_API  Result EnableFaceCulling(bool enable);

	BLAZE_GRAPHICS_API  Result EnableDepthTest(bool enable);
	BLAZE_GRAPHICS_API  Result EnableWritingToDepthBuffer(bool enable);
	BLAZE_GRAPHICS_API  Result EnableStencilTest(bool enable);
	BLAZE_GRAPHICS_API  Result EnableScissorTest(bool enable);

	BLAZE_GRAPHICS_API  Result SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed);
	BLAZE_GRAPHICS_API  Result SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask);

	BLAZE_GRAPHICS_API  Result RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset);
	BLAZE_GRAPHICS_API  Result RenderInstancedPrimitiveArray(PrimitiveType type, uint statIndex, uint vertexCount, uint instanceCount);
	BLAZE_GRAPHICS_API  Result RenderPrimitiveArray(PrimitiveType type, uint statIndex, uint vertexCount);

	BLAZE_GRAPHICS_API  Result DispatchCompute(uint x, uint y, uint z);

	BLAZE_GRAPHICS_API  Result MemoryBarrier();

	BLAZE_GRAPHICS_API  Result SelectImage(uint slot, const Texture2D& texture, uint level, ImageAccess access, ImageFormat format);
	BLAZE_GRAPHICS_API  Result SelectImage(uint slot, const Texture1D& texture, uint level, ImageAccess access, ImageFormat format);	
}