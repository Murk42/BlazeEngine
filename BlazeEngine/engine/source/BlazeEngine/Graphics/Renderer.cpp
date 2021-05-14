#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Core/Application.h"

#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	namespace Renderer
	{
		uint MaxBoundTextures()
		{
			int num;
			glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &num);
			return uint(num);
		}
		void SetViewport(const Vec2i& pos, const Vec2i& size)
		{
			engine->Renderer.viewportPos = pos;
			engine->Renderer.viewportSize = size;
			glViewport(pos.x, pos.y, size.x, size.y);			
		}

		void SetClearColor(Color color)
		{
			glClearColor(color.r, color.g, color.b, color.a);
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
				Logger::AddLog(LogType::Warning, "UpdateTarget", "No target was selected!");
			else
				SDL_GL_SwapWindow((SDL_Window*)engine->Renderer.target);
			}
		void SwapInterval(bool vsync)
		{
			SDL_GL_SetSwapInterval(vsync);
		}
		void SetTarget(Window& win)
		{
			if (engine->Renderer.target != win.ptr)
			{
				engine->Renderer.target = win.ptr;
				SDL_GL_MakeCurrent((SDL_Window*)win.ptr, (SDL_GLContext)engine->App.openGLContext);
			}
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

		void UseDepthBuffer(bool use)
		{
			glDepthMask(use);
		}
		void UseFaceCulling(bool use)
		{
			if (use)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		void UseBlending(bool use)
		{
			if (use)
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			
			}
			else		
				glDisable(GL_BLEND);
		}

		void RenderTriangles(IndexType indexType, uint triangleCount)
		{					
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)0);			
		}
		void RenderPointArray(uint count, size_t offset)
		{
			glDrawArrays(GL_POINTS, int(offset), count);
		}
		 /*
		void RenderPointArray(const ShaderProgram& sp, const VertexLayout& vl, uint count, size_t offset)
		{
			if (vl.GetLayout().size() != 0)
			{
				sp.Bind();
				vl.Bind();

				if (count == 0)
					count = uint(vl.GetVertexBuffer()->GetSize() / vl.GetStride());

				glDrawArrays(GL_POINTS, int(offset), count);
				vl.Unbind();
			}
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to render a VertexLayout object that has no layout set");
		}
		void RenderPointArray(const ShaderProgram& sp, const Mesh& mesh, uint count, size_t offset)
		{
			RenderPointArray(sp, mesh.vl, count, offset);
		}
		void RenderPointArray(BaseMaterial& material, const VertexLayout& vl, uint count, size_t offset)
		{			
			material.UpdateShaderProgramUniforms();
			material.BindTextures();			

			RenderPointArray(material.GetShaderProgram(), vl, count, offset);
		}
		void RenderPointArray(BaseMaterial& material, const Mesh& mesh, uint count, size_t offset)
		{
			material.UpdateShaderProgramUniforms();
			material.BindTextures();

			RenderPointArray(material.GetShaderProgram(), mesh.vl, count, offset);
		}

		void RenderPoints(const ShaderProgram& sp, const VertexLayout& vl, uint pointCount, size_t offset)
		{
			if (vl.GetLayout().size() != 0)
			{
				sp.Bind();
				vl.Bind();

				if (pointCount == 0)
					pointCount = vl.GetIndexBuffer()->GetIndexCount();

				Type indexType = vl.GetIndexBuffer()->GetIndexType();
				glDrawElements(GL_POINTS, pointCount, (unsigned)indexType, (void*)(offset * SizeOf(indexType)));
				vl.Unbind();
			}
		}
		void RenderPoints(const ShaderProgram& sp, const Mesh& mesh, uint pointCount, size_t offset)
		{
			RenderPoints(sp, mesh.vl, pointCount, offset);			
		}
		void RenderPoints(BaseMaterial& material, const VertexLayout& vl, uint pointCount, size_t offset)
		{
			material.UpdateShaderProgramUniforms();
			material.BindTextures();

			RenderPoints(material.GetShaderProgram(), vl, pointCount, offset);
		}
		void RenderPoints(BaseMaterial& material, const Mesh& mesh, uint pointCount, size_t offset)
		{			
			material.UpdateShaderProgramUniforms();
			material.BindTextures();
			
			RenderPoints(material.GetShaderProgram(), mesh.vl, pointCount, offset);			
		}

		void RenderTriangles(ShaderProgram& sp, VertexLayout& vl, uint triangleCount, size_t offset)
		{
			sp.Bind();
			vl.Bind();

			if (triangleCount == 0)
				triangleCount = vl.GetIndexBuffer()->GetIndexCount() / 3;

			Type indexType = vl.GetIndexBuffer()->GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(offset * SizeOf(indexType)));
			vl.Unbind();
		}
		void RenderTriangles(ShaderProgram& sp, Mesh& mesh, uint triangleCount, size_t offset)
		{
			sp.Bind();
			mesh.vl.Bind();

			if (triangleCount == 0)
				triangleCount = mesh.ib.GetIndexCount() / 3;

			Type indexType = mesh.ib.GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(offset * SizeOf(indexType)));
			mesh.vl.Unbind();
		}
		void RenderTriangles(BaseMaterial& material, Mesh& mesh, uint triangleCount, size_t offset)
		{
			material.GetShaderProgram().Bind();
			material.UpdateShaderProgramUniforms();
			material.BindTextures();
			mesh.vl.Bind();

			if (triangleCount == 0)
				triangleCount = mesh.ib.GetIndexCount() / 3;
			
			Type indexType = mesh.ib.GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(offset * SizeOf(indexType)));
			mesh.vl.Unbind();
		}		
		*/
	}
}