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

namespace OpenGLWrapper
{	
	OPENGL_WRAPPER_API Result SetActiveTextureSlot(uint slot);
	OPENGL_WRAPPER_API Result BindUniformBuffer(const GraphicsBuffer& buffer, uint binding);
	OPENGL_WRAPPER_API Result BindUniformBufferRange(const GraphicsBuffer& buffer, uint binding, uint offset, uint size);

	OPENGL_WRAPPER_API Result SelectTexture(Texture1D*);
	OPENGL_WRAPPER_API Texture1D* GetSelectedTexture1D();

	OPENGL_WRAPPER_API Result SelectTexture(Texture2D*);
	OPENGL_WRAPPER_API Texture2D* GetSelectedTexture2D();

	OPENGL_WRAPPER_API Result SelectTexture(Texture2DArray*);
	OPENGL_WRAPPER_API Texture2DArray* GetSelectedTexture2DArray();

	OPENGL_WRAPPER_API Result SelectTexture(TextureCubemap*);
	OPENGL_WRAPPER_API TextureCubemap* GetSelectedTextureCubemap();

	OPENGL_WRAPPER_API Result SelectTexture(Texture3D*);
	OPENGL_WRAPPER_API Texture3D* GetSelectedTexture3D();

	OPENGL_WRAPPER_API Result SelectTexture(TextureBuffer*);
	OPENGL_WRAPPER_API TextureBuffer* GetTextureBuffer();

	OPENGL_WRAPPER_API Result SelectVertexBuffer(GraphicsBuffer* buffer);
	OPENGL_WRAPPER_API GraphicsBuffer* GetSelectedVertexBuffer();

	OPENGL_WRAPPER_API Result SelectIndexBuffer(GraphicsBuffer* buffer);
	OPENGL_WRAPPER_API GraphicsBuffer* GetSelectedIndexBuffer();

	OPENGL_WRAPPER_API Result SelectUniformBuffer(GraphicsBuffer* buffer);
	OPENGL_WRAPPER_API GraphicsBuffer* GetSelectedUniformBuffer();

	OPENGL_WRAPPER_API Result SelectVertexArray(VertexArray* vertexArray);
	OPENGL_WRAPPER_API VertexArray* GetSelectedVertexArray();

	OPENGL_WRAPPER_API Result SelectProgram(ShaderProgram* program);
	OPENGL_WRAPPER_API ShaderProgram* GetSelectedProgram();

	OPENGL_WRAPPER_API Result SelectFramebuffer(Framebuffer* framebuffer);
	OPENGL_WRAPPER_API Framebuffer* GetSelectedFramebuffer();

	OPENGL_WRAPPER_API Result SelectDrawFramebuffer(Framebuffer* framebuffer);
	OPENGL_WRAPPER_API Framebuffer* GetDrawSelectedFramebuffer();
	OPENGL_WRAPPER_API Result SelectReadFramebuffer(Framebuffer* framebuffer);
	OPENGL_WRAPPER_API Framebuffer* GetReadSelectedFramebuffer();

	OPENGL_WRAPPER_API Result SelectRenderbuffer(Renderbuffer* renderbuffer);
	OPENGL_WRAPPER_API Renderbuffer* GetSelectedRenderbuffer();

	OPENGL_WRAPPER_API Result SetClearColor(ColorRGBAf);
	OPENGL_WRAPPER_API Result SetRenderArea(Vec2i pos, Vec2i size);
	OPENGL_WRAPPER_API Result SetPatchSize(uint size);
	OPENGL_WRAPPER_API Result SetPointSize(uint size);
	OPENGL_WRAPPER_API Result SetScissorRect(Vec2i pos, Vec2i size);

	OPENGL_WRAPPER_API uint GetMaxBoundTextures();
	OPENGL_WRAPPER_API ColorRGBAf GetClearColor();
	OPENGL_WRAPPER_API Recti GetRenderArea();
	OPENGL_WRAPPER_API float GetRenderAreaAspectRatio();

	OPENGL_WRAPPER_API Result ClearTarget();
	OPENGL_WRAPPER_API Result ClearTargetColor();
	OPENGL_WRAPPER_API Result ClearTargetDepth();
	OPENGL_WRAPPER_API Result ClearTargetStencil();

	OPENGL_WRAPPER_API Result SetPolygonMode(PolygonMode mode);

	OPENGL_WRAPPER_API Result EnableProgramPointSize(bool enable);
	OPENGL_WRAPPER_API Result EnableBlending(bool enable);
	OPENGL_WRAPPER_API Result EnableFaceCulling(bool enable);

	OPENGL_WRAPPER_API Result EnableDepthTest(bool enable);
	OPENGL_WRAPPER_API Result EnableWritingToDepthBuffer(bool enable);
	OPENGL_WRAPPER_API Result EnableStencilTest(bool enable);
	OPENGL_WRAPPER_API Result EnableScissorTest(bool enable);

	OPENGL_WRAPPER_API Result SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed);
	OPENGL_WRAPPER_API Result SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask);

	OPENGL_WRAPPER_API Result RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uint indexCount, uint indexBufferByteOffset);
	OPENGL_WRAPPER_API Result RenderPrimitiveArray(PrimitiveType type, uint statIndex, uint vertexCount);

	OPENGL_WRAPPER_API Result DispatchCompute(uint x, uint y, uint z);

	OPENGL_WRAPPER_API Result MemoryBarrier();

	OPENGL_WRAPPER_API Result SelectImage(uint slot, const Texture2D& texture, uint level, ImageAccess access, ImageFormat format);
	OPENGL_WRAPPER_API Result SelectImage(uint slot, const Texture1D& texture, uint level, ImageAccess access, ImageFormat format);

	namespace Events
	{
		struct RenderAreaChanged
		{
			Vec2i pos;
			Vec2i size;
		};
		OPENGL_WRAPPER_API void AddRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>&);
		OPENGL_WRAPPER_API void RemoveRenderAreaChangedEventHandler(EventHandler<RenderAreaChanged>&);
	}
}