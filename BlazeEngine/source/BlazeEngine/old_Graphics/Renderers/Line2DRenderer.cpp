#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::Graphics
{																	  	
	Line2DRenderer::Line2DRenderer()		
	{		
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, GraphicsLibrary::VertexAttributeType::Float, 1, false, sizeof(float) * 8);
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(16, sizeof(Vertex));
	}
	Line2DRenderer::~Line2DRenderer()
	{		
	}

	Result Line2DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
		return Result();
	}
	
	Result Line2DRenderer::Draw(Vec2f pos1, Vec2f pos2, ColorRGBAf color, float width) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));		
		CHECK_RESULT(program.SetUniform(0, proj));

		Vertex vertex;
		vertex.pos1 = pos1;
		vertex.pos2 = pos2;
		vertex.width = width;
		vertex.color = (Vec4f)color;

		CHECK_RESULT(renderer.Render(ArrayView(&vertex, 1), [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}
	Result Line2DRenderer::Draw(ArrayView<Vertex> vertices) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));		
		CHECK_RESULT(program.SetUniform(0, proj));

		CHECK_RESULT(renderer.Render(vertices, [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}
	Result Line2DRenderer::SetShaders(std::initializer_list<GraphicsLibrary::Shader*> shaders)
	{
		if (program.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			program = std::move(GraphicsLibrary::ShaderProgram());

		return program.LinkShaders(shaders);
	}
	Result Line2DRenderer::LoadDefaultShaders() const
	{
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;

		CHECK_RESULT(vert.Load("assets/default/shaders/line2d/line2d.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/line2d/line2d.geom"));
		CHECK_RESULT(frag.Load("assets/default/shaders/line2d/line2d.frag"));

		if (program.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			program = std::move(GraphicsLibrary::ShaderProgram());

		CHECK_RESULT(program.LinkShaders({ &vert, &geom, &frag }));

		return Result();
	}
	Result Line2DRenderer::CheckProgram() const
	{
		switch (program.GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaders());
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			return Result();
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to draw a 2D line but the shader program wasn't succesfully linked");
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid ShaderProgramState value");
		}
	}
}