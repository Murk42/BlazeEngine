#include "pch.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLRenderbuffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture1D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngine/Graphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include <GL/glew.h>
#include <SDL3/SDL_video.h>
using namespace Blaze::Graphics::OpenGL;

namespace Blaze
{
	StringView SDL_GetError();
}

namespace Blaze::Graphics::OpenGL
{
	template<typename T>
	inline std::underlying_type_t<T> ToInteger(T value)
	{
		return static_cast<std::underlying_type_t<T>>(value);
	}

	String GetOpenGLErrorString(GLenum error)
	{
		switch (error)
		{
		case GL_NO_ERROR:                      return String();
		case GL_INVALID_ENUM:                  return "INVALID_ENUM";
		case GL_INVALID_VALUE:                 return "INVALID_VALUE";
		case GL_INVALID_OPERATION:             return "INVALID_OPERATION";
		case GL_STACK_OVERFLOW:                return "STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:               return "STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:                 return "OUT_OF_MEMORY";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
		default: return "UNRECOGNIZED_ERROR_VALUE";
		}
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
		default:
			BLAZE_LOG_ERROR("Invalid PrimitiveType enum value. The integer value was: {}", ToInteger(type));
			return 0;
		}
	}
	GLenum OpenGLIndexType(IndexType type)
	{
		switch (type)
		{
		case IndexType::Uint32: return GL_UNSIGNED_INT;
		case IndexType::Uint16: return GL_UNSIGNED_SHORT;
		case IndexType::Uint8: return GL_UNSIGNED_BYTE;
		default:
			BLAZE_LOG_ERROR("Invalid IndexType enum value. The integer value was: {}", ToInteger(type));
			return 0;
		}
	}
	GLenum OpenGLPolygonMode(PolygonMode mode)
	{
		switch (mode)
		{
		case PolygonMode::Point: return GL_POINT;
		case PolygonMode::Line: return GL_LINE;
		case PolygonMode::Fill: return GL_FILL;
		default:
			BLAZE_LOG_ERROR("Invalid PolygonMode. The integer value was: {}", ToInteger(mode));
			return 0;
		}
	}
	GLenum OpenGLImageAccess(ImageAccess access)
	{
		switch (access)
		{
		case ImageAccess::Read: return GL_READ_ONLY;
		case ImageAccess::Write: return GL_WRITE_ONLY;
		case ImageAccess::ReadWrite: return GL_READ_WRITE;
		default:
			BLAZE_LOG_ERROR("Invalid ImageAccess enum value. The integer value was: {}", ToInteger(access));
			return 0;
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
		default:
			BLAZE_LOG_ERROR("Invalid ImageFormat enum value. The integer value was: {}", ToInteger(format));
			return 0;
		}
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
		BLAZE_LOG_ERROR("Invalid StencilOperationType enum value. The integer value was: {}", ToInteger(type));
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
		BLAZE_LOG_ERROR("Invalid StencilComparison enum value. The integer value was: {}", ToInteger(comparison));
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
		BLAZE_LOG_ERROR("Invalid ScreenBufferType enum value. The integer value was: {}", ToInteger(type));
		return std::numeric_limits<GLenum>::max();
	}

	void GraphicsContext_OpenGL::SelectTexture(const Texture1D* obj)
	{
		glBindTexture(GL_TEXTURE_1D, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectTexture(const Texture2D* obj)
	{
		glBindTexture(GL_TEXTURE_2D, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectVertexBuffer(const GraphicsBuffer* obj)
	{
		glBindBuffer(GL_ARRAY_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectUniformBuffer(const GraphicsBuffer* obj)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectVertexArray(const VertexArray* obj)
	{
		glBindVertexArray(obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectProgram(const ShaderProgram* obj)
	{
		glUseProgram(obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectFramebuffer(const Framebuffer_OpenGL* obj)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectDrawFramebuffer(const Framebuffer_OpenGL* obj)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectReadFramebuffer(const Framebuffer_OpenGL* obj)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectRenderbuffer(const Renderbuffer* obj)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, obj == nullptr ? 0 : obj->GetHandle());
	}
	void GraphicsContext_OpenGL::SelectShaderStorageBufferToSlot(uint slotIndex, const GraphicsBuffer* buffer, uintMem offset, uintMem size)
	{
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, slotIndex, buffer->GetHandle(), offset, size);
	}

	void GraphicsContext_OpenGL::SetActiveTextureSlot(uint slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
	}
	void GraphicsContext_OpenGL::BindUniformBuffer(const GraphicsBuffer& buffer, uint bindingPoint)
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer.GetHandle());
	}
	void GraphicsContext_OpenGL::BindUniformBufferRange(const GraphicsBuffer& buffer, uint bindingPoint, uint offset, uint size)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, buffer.GetHandle(), offset, size);
	}
	void GraphicsContext_OpenGL::SetShaderUniformBlockBindingPoint(const ShaderProgram& program, uintMem uniformBlockIndex, uint bindingPoint)
	{
		glUniformBlockBinding(program.GetHandle(), uniformBlockIndex, bindingPoint);
	}
	void GraphicsContext_OpenGL::SelectImage(uint slot, const Texture2D& texture, uint level, ImageAccess access, ImageFormat format)
	{
		glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
	}
	void GraphicsContext_OpenGL::SelectImage(uint slot, const Texture1D& texture, uint level, ImageAccess access, ImageFormat format)
	{
		glBindImageTexture(slot, texture.GetHandle(), level, GL_FALSE, 0, OpenGLImageAccess(access), OpenGLImageFormat(format));
	}

	void GraphicsContext_OpenGL::SetClearColor(ColorRGBAf color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}
	void GraphicsContext_OpenGL::SetRenderArea(Vec2i pos, Vec2u size)
	{
		glViewport(pos.x, pos.y, size.x, size.y);
	}
	void GraphicsContext_OpenGL::SetPatchSize(uint size)
	{
		glPatchParameteri(GL_PATCH_VERTICES, size);
	}
	void GraphicsContext_OpenGL::SetPointSize(uint size)
	{
		glPointSize((float)size);
	}
	void GraphicsContext_OpenGL::SetScissorRect(Vec2i pos, Vec2i size)
	{
		glScissor(pos.x, pos.y, size.x, size.y);
	}

	bool GraphicsContext_OpenGL::SetSwapInterval(WindowSwapInterval swapInterval)
	{
		//TODO do error checking

		switch (swapInterval)
		{
		case Blaze::Graphics::OpenGL::WindowSwapInterval::None:
			if (!SDL_GL_SetSwapInterval(0))
				BLAZE_LOG_ERROR("SDL_GL_SetSwapInterval failed. SDL returned error: \"" + SDL_GetError() + "\"");
			return true;
		case Blaze::Graphics::OpenGL::WindowSwapInterval::VSync:
			if (!SDL_GL_SetSwapInterval(1))
				BLAZE_LOG_ERROR("SDL_GL_SetSwapInterval failed. SDL returned error: \"" + SDL_GetError() + "\"");
			return true;
		case Blaze::Graphics::OpenGL::WindowSwapInterval::AdaptiveVSync:
			if (!SDL_GL_SetSwapInterval(-1))
			{
				if (!SDL_GL_SetSwapInterval(1))
					BLAZE_LOG_ERROR("SDL_GL_SetSwapInterval failed. SDL returned error: \"" + SDL_GetError() + "\"");

				return false;
			}

			return true;
		default:
			BLAZE_LOG_FATAL("Invalid WindowSwapInterval enum value");
			return false;
		}
	}

	uint GraphicsContext_OpenGL::GetMaxBoundTextures()
	{
		int num;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
		return uint(num);
	}

	void GraphicsContext_OpenGL::SetPolygonMode(PolygonMode mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, OpenGLPolygonMode(mode));
	}

	void GraphicsContext_OpenGL::EnableWritingToDepthBuffer(bool enable)
	{
		glDepthMask(enable);
	}
	void GraphicsContext_OpenGL::EnableBlending(bool enable)
	{
		if (enable)
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}
	void GraphicsContext_OpenGL::EnableProgramPointSize(bool enable)
	{
		if (enable)
			glEnable(GL_PROGRAM_POINT_SIZE);
		else
			glDisable(GL_PROGRAM_POINT_SIZE);
	}
	void GraphicsContext_OpenGL::EnableFaceCulling(bool enable)
	{
		if (enable)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}
	void GraphicsContext_OpenGL::EnableDepthTest(bool enable)
	{
		if (enable)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	void GraphicsContext_OpenGL::EnableStencilTest(bool enable)
	{
		if (enable)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
	void GraphicsContext_OpenGL::EnableScissorTest(bool enable)
	{
		if (enable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	void GraphicsContext_OpenGL::SetStencilOperation(ScreenBufferType bufferType, StencilOperationType bothFail, StencilOperationType depthFailStencilSucceed, StencilOperationType bothSucceed)
	{
		glStencilOpSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilOperationType(bothFail), OpenGLStencilOperationType(depthFailStencilSucceed), OpenGLStencilOperationType(bothSucceed));
	}
	void GraphicsContext_OpenGL::SetStencilFunction(ScreenBufferType bufferType, StencilComparison comparison, int reference, uint mask)
	{
		glStencilFuncSeparate(OpenGLScreenBufferType(bufferType), OpenGLStencilComparison(comparison), reference, mask);
	}

	void GraphicsContext_OpenGL::RenderIndexedPrimitives(PrimitiveType type, IndexType indexType, uintMem indexCount, uintMem indexBufferByteOffset)
	{
		glDrawElements(OpenGLRenderPrimitive(type), static_cast<GLsizei>(indexCount), OpenGLIndexType(indexType), reinterpret_cast<void*>(static_cast<uintptr_t>(indexBufferByteOffset)));
	}
	void GraphicsContext_OpenGL::RenderInstancedPrimitiveArray(PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount, uintMem firstInstanceIndex, uintMem instanceCount)
	{
		glDrawArraysInstancedBaseInstance(OpenGLRenderPrimitive(type), static_cast<GLint>(firstVertexIndex), static_cast<GLint>(vertexCount), static_cast<GLsizei>(instanceCount), static_cast<GLuint>(firstInstanceIndex));
	}
	void GraphicsContext_OpenGL::RenderPrimitiveArray(PrimitiveType type, uintMem firstVertexIndex, uintMem vertexCount)
	{
		glDrawArrays(OpenGLRenderPrimitive(type), static_cast<GLint>(firstVertexIndex), static_cast<GLsizei>(vertexCount));
	}

	void GraphicsContext_OpenGL::BlitFramebuffer(Framebuffer_OpenGL& writeFramebuffer, Framebuffer_OpenGL& readFramebuffer, Vec2i dstP1, Vec2i dstP2, Vec2i srcP1, Vec2i srcP2, bool copyColor, bool copyDepth, bool copyStencil, TextureSampling sampling)
	{
		GLenum mask = 0;
		if (copyColor) mask |= GL_COLOR_BUFFER_BIT;
		if (copyDepth) mask |= GL_DEPTH_BUFFER_BIT;
		if (copyStencil) mask |= GL_STENCIL_BUFFER_BIT;

		GLenum filter = 0;
		switch (sampling)
		{
		case Blaze::Graphics::OpenGL::TextureSampling::Nearest:
			filter = GL_NEAREST;
			break;
		case Blaze::Graphics::OpenGL::TextureSampling::Linear:
			filter = GL_LINEAR;
			break;
		default:
			BLAZE_LOG_ERROR("Invalid TextureSampling enum value");
			break;
		}

		glBlitNamedFramebuffer(readFramebuffer.GetHandle(), writeFramebuffer.GetHandle(), srcP1.x, srcP1.y, srcP2.x, srcP2.y, dstP1.x, dstP1.y, dstP2.x, dstP2.y, mask, filter);
	}

	void GraphicsContext_OpenGL::DispatchCompute(uint x, uint y, uint z)
	{
		glDispatchCompute(x, y, z);
	}

	void GraphicsContext_OpenGL::Flush()
	{
		glFlush();
	}
	void GraphicsContext_OpenGL::MemoryBarrier()
	{
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}

	bool GraphicsContext_OpenGL::IsExtensionSupported(StringView name)
	{
		return glewIsSupported(name.Ptr());
	}

}