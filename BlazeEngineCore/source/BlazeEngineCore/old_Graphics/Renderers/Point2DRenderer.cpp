#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::Graphics
{		
	Point2DRenderer::Point2DRenderer()		
	{		
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 1, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(4, sizeof(Vertex));
	}
	Point2DRenderer::~Point2DRenderer()
	{		
	}

	Result Point2DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
		return Result();
	}
		
	Result Point2DRenderer::Draw(Vec2f pos, ColorRGBAf color, float radius) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));
		CHECK_RESULT(program.SetUniform(0, proj));

		Vertex vertex;
		vertex.pos = pos;		
		vertex.color = (Vec4f)color;
		vertex.radius = radius;

		CHECK_RESULT(renderer.Render(ArrayView<Vertex>(&vertex, 1), [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}
	Result Point2DRenderer::Draw(ArrayView<Vertex> instances) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));
		CHECK_RESULT(program.SetUniform(0, proj));

		CHECK_RESULT(renderer.Render(instances, [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}
	Result Point2DRenderer::SetShaderProgram(GraphicsLibrary::ShaderProgram&& program)
	{
		this->program = std::move(program);
		return Result();
	}
	Result Point2DRenderer::LoadDefaultShaderProgram() const
	{
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;

		CHECK_RESULT(vert.Load("assets/default/shaders/point2d/point2d.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/point2d/point2d.geom"));
		CHECK_RESULT(frag.Load("assets/default/shaders/point2d/point2d.frag"));

		if (program.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			program = std::move(GraphicsLibrary::ShaderProgram());

		CHECK_RESULT(program.LinkShaders({ &vert, &geom, &frag }));

		return Result();
	}
	Result Point2DRenderer::CheckProgram() const
	{
		switch (program.GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaderProgram());
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			return Result();
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to draw a 2D line but the shader program wasn't succesfully linked");
		default:			
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid ShaderProgramState value");
		}
	}
}