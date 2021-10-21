#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Application.h"
#include "BlazeEngine/Graphics/GraphicsLibrary.h"

#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	namespace Renderer
	{
		void BLAZE_API SetActiveTextureSlot(uint slot)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
		}
		void SelectTexture(Graphics::Core::Texture2D* obj)
		{			
			glBindTexture(GL_TEXTURE_2D, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedTexture2D = obj;
		}		
		Graphics::Core::Texture2D* GetSelectedTexture2D()
		{
			return engine->Renderer.selectedTexture2D;
		}

		void SelectTexture(Graphics::Core::Texture2DArray* obj)
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedTexture2DArray = obj;
		}		
		Graphics::Core::Texture2DArray* GetSelectedTexture2DArray()
		{			
			return engine->Renderer.selectedTexture2DArray;
		}

		void SelectTexture(Graphics::Core::TextureCubemap* obj)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedTextureCubemap = obj;
		}
		Graphics::Core::TextureCubemap* GetSelectedTextureCubemap()
		{
			return engine->Renderer.selectedTextureCubemap;
		}

		void SelectVertexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_ARRAY_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedArrayBuffer = obj;
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedVertexBuffer()
		{
			return engine->Renderer.selectedArrayBuffer;
		}

		void SelectIndexBuffer(Graphics::Core::GraphicsBuffer* obj)
		{			
			engine->Renderer.selectedVertexArray->SetIndexBuffer(*obj);
		}		
		Graphics::Core::GraphicsBuffer* GetSelectedIndexBuffer()
		{
			return engine->Renderer.selectedVertexArray->GetIndexBuffer();
		}

		void SelectUniformBuffer(Graphics::Core::GraphicsBuffer* obj)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedUniformBuffer = obj;
		}
		Graphics::Core::GraphicsBuffer* GetSelectedUniformBuffer()
		{
			return engine->Renderer.selectedUniformBuffer;
		}

		void SelectVertexArray(Graphics::Core::VertexArray* obj)
		{
			glBindVertexArray(obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedVertexArray = obj;
		}		
		Graphics::Core::VertexArray* GetSelectedVertexArray()
		{
			return engine->Renderer.selectedVertexArray;
		}

		void SelectProgram(Graphics::Core::Program* obj)
		{
			glUseProgram(obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedProgram = obj;
		}
		Graphics::Core::Program* GetSelectedProgram()
		{
			return engine->Renderer.selectedProgram;
		}

		void SelectFramebuffer(Graphics::Core::Framebuffer* obj)
		{ 
			glBindFramebuffer(GL_FRAMEBUFFER, obj == nullptr ? 0 : obj->GetHandle());
			engine->Renderer.selectedFramebuffer = obj;
		}
		Graphics::Core::Framebuffer* GetSelectedFramebuffer()
		{
			return engine->Renderer.selectedFramebuffer;;
		}


		void SetClearColor(ColorRGBAf color)
		{
			engine->Renderer.clearColor = color;
			glClearColor(color.r, color.g, color.b, color.a);
		}
		void SetViewport(Vec2i pos, Vec2i size)
		{
			engine->Renderer.viewportPos = pos;
			engine->Renderer.viewportSize = size;
			glViewport(pos.x, pos.y, size.x, size.y);
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
			return engine->Renderer.clearColor;
		}
		Vec2i GetViewportPos()
		{
			return engine->Renderer.viewportPos;
		}
		Vec2i GetViewportSize()
		{
			return engine->Renderer.viewportSize;
		}
		float GetViewportRatio()
		{
			return (float)engine->Renderer.viewportSize.x / engine->Renderer.viewportSize.y;
		}

		void ClearTarget()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
		void ClearTargetColor()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}
		void ClearTargetDepth()
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
		void UpdateTarget()
		{
			if (engine->Renderer.target == nullptr)
				BLAZE_WARNING_LOG("Blaze Engine", "No target was selected!");
			else
				SDL_GL_SwapWindow((SDL_Window*)engine->Renderer.target);
		}

		void SetTarget(Window& win)
		{
			if (engine->Renderer.target != win.ptr)
			{
				engine->Renderer.target = win.ptr;
				SDL_GL_MakeCurrent((SDL_Window*)win.ptr, (SDL_GLContext)engine->App.openGLContext);
			}
		}
		void SetPolygonMode(PolygonMode mode)
		{			
			glPolygonMode(GL_FRONT_AND_BACK, (GLenum)mode);			
		}

		void EnableVSync(bool vsync)
		{
			SDL_GL_SetSwapInterval(vsync);
		}		
		void EnableDepthBuffer(bool enable)
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
		void EnableScissorTest(bool enable)
		{
			if (enable)
				glEnable(GL_SCISSOR_TEST);
			else
				glDisable(GL_SCISSOR_TEST);
		}
		void EnableStencilTest(bool enable)
		{
			if (enable)
				glEnable(GL_STENCIL_TEST);
			else
				glDisable(GL_STENCIL_TEST);
		}
		
		void RenderTriangles(IndexType indexType, uint count)
		{								
			glDrawElements(GL_TRIANGLES, count, (unsigned)indexType, (void*)0);			
		}
		void RenderPatches(IndexType indexType, uint count)
		{
			glDrawElements(GL_PATCHES, count, (unsigned)indexType, (void*)0);
		}
		void RenderTriangleArray(uint count, size_t offset)
		{
			glDrawArrays(GL_TRIANGLES, int(offset), count);
		}
		void RenderPointArray(uint count, size_t offset)
		{
			glDrawArrays(GL_POINTS, int(offset), count);
		}
		void RenderPatchArray(uint count, size_t offset)
		{
			glDrawArrays(GL_PATCHES, int(offset), count);
		}
	}
}