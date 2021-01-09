#include "Engine.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/Graphics/VertexLayout.h"
#include "BlazeEngine/Graphics/ShaderProgram.h"
#include "BlazeEngine/Graphics/Material.h"
#include "BlazeEngine/Core/Application.h"

#define SDL_MAIN_HANDLED
#include "SDL/SDL.h"
#include "GL/glew.h"

namespace Blaze
{	
	namespace Renderer
	{		
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
				SDL_GL_MakeCurrent((SDL_Window*)win.ptr, (SDL_GLContext)engine->Application.openGLContext);
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

		void RenderPointArray(const ShaderProgram& sp, const VertexLayout& vl, uint count, uint offset)
		{
			if (vl.GetLayout().size() != 0)
			{
				sp.Bind();
				vl.Bind();

				if (count == 0)
					count = vl.GetVertexBuffer()->GetSize() / vl.GetStride();

				glDrawArrays(GL_POINTS, offset, count);
				vl.Unbind();
			}
			else
				Logger::AddLog(LogType::Warning, __FUNCTION__, "Trying to render a VertexLayout object that has no layout set");
		}
		void RenderPointArray(const ShaderProgram& sp, const Mesh& mesh, uint count, uint offset)
		{
			RenderPointArray(sp, mesh.vl, count, offset);
		}
		void RenderPointArray(BaseMaterial& material, const VertexLayout& vl, uint count, uint offset)
		{			
			material.UpdateShaderProgramUniforms();
			material.BindTextures();			

			RenderPointArray(material.GetShaderProgram(), vl, count, offset);
		}
		void RenderPointArray(BaseMaterial& material, const Mesh& mesh, uint count, uint offset)
		{
			material.UpdateShaderProgramUniforms();
			material.BindTextures();

			RenderPointArray(material.GetShaderProgram(), mesh.vl, count, offset);
		}

		void RenderPoints(const ShaderProgram& sp, const VertexLayout& vl, uint pointCount, uint indexOffset)
		{
			if (vl.GetLayout().size() != 0)
			{
				sp.Bind();
				vl.Bind();

				if (pointCount == 0)
					pointCount = vl.GetIndexBuffer()->GetIndexCount();

				Type indexType = vl.GetIndexBuffer()->GetIndexType();
				glDrawElements(GL_POINTS, pointCount, (unsigned)indexType, (void*)(indexOffset * SizeOf(indexType)));
				vl.Unbind();
			}
		}
		void RenderPoints(const ShaderProgram& sp, const Mesh& mesh, uint pointCount, uint indexOffset)
		{
			RenderPoints(sp, mesh.vl, pointCount, indexOffset);			
		}
		void RenderPoints(BaseMaterial& material, const VertexLayout& vl, uint pointCount, uint indexOffset)
		{
			material.UpdateShaderProgramUniforms();
			material.BindTextures();

			RenderPoints(material.GetShaderProgram(), vl, pointCount, indexOffset);
		}
		void RenderPoints(BaseMaterial& material, const Mesh& mesh, uint pointCount, uint indexOffset)
		{			
			material.UpdateShaderProgramUniforms();
			material.BindTextures();
			
			RenderPoints(material.GetShaderProgram(), mesh.vl, pointCount, indexOffset);			
		}

		void RenderTriangles(ShaderProgram& sp, VertexLayout& vl, uint triangleCount, uint indexOffset)
		{
			sp.Bind();
			vl.Bind();

			if (triangleCount == 0)
				triangleCount = vl.GetIndexBuffer()->GetIndexCount() / 3;

			Type indexType = vl.GetIndexBuffer()->GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(indexOffset * SizeOf(indexType)));
			vl.Unbind();
		}
		void RenderTriangles(ShaderProgram& sp, Mesh& mesh, uint triangleCount, uint indexOffset)
		{
			sp.Bind();
			mesh.vl.Bind();

			if (triangleCount == 0)
				triangleCount = mesh.ib.GetIndexCount() / 3;

			Type indexType = mesh.ib.GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(indexOffset * SizeOf(indexType)));
			mesh.vl.Unbind();
		}
		void RenderTriangles(BaseMaterial& material, Mesh& mesh, uint triangleCount, uint indexOffset)
		{
			material.GetShaderProgram().Bind();
			material.UpdateShaderProgramUniforms();
			material.BindTextures();
			mesh.vl.Bind();

			if (triangleCount == 0)
				triangleCount = mesh.ib.GetIndexCount() / 3;
			
			Type indexType = mesh.ib.GetIndexType();
			glDrawElements(GL_TRIANGLES, triangleCount * 3, (unsigned)indexType, (void*)(indexOffset * SizeOf(indexType)));
			mesh.vl.Unbind();
		}		
	}
}