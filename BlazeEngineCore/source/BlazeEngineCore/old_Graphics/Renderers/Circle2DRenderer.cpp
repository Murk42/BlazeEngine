#include "BlazeEngine/Graphics/Renderers/Circle2DRenderer.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze::Graphics
{
	Circle2DRenderer::Circle2DRenderer()
	{
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.EnableVertexAttribute(4);
		va.EnableVertexAttribute(5);
		va.EnableVertexAttribute(6);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 8);
		va.SetVertexAttributeFormat(3, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 12);
		va.SetVertexAttributeFormat(4, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 16);
		va.SetVertexAttributeFormat(5, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 20);
		va.SetVertexAttributeFormat(6, GraphicsLibrary::VertexAttributeType::Float, 1, false, sizeof(float) * 24);		
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(5, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(6, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(16, sizeof(Vertex));
	}
	Circle2DRenderer::~Circle2DRenderer()
	{
	}
	Result Circle2DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		this->proj = mat;
		return Result();
	}
	Result Circle2DRenderer::Draw(Vec2f center, float outerRadius, float innerRadius, ColorRGBAf outerColor, ColorRGBAf innerColor) const
	{
		CHECK_RESULT(CheckProgram());

		CHECK_RESULT(Graphics::Renderer::SelectVertexArray(&va));
		CHECK_RESULT(Graphics::Renderer::SelectProgram(&program));
		CHECK_RESULT(program.SetUniform(0, proj));

		Vertex vertex;
		vertex.matrix = (Mat4f::TranslationMatrix(Vec3f(center, 0)) * Mat4f::ScalingMatrix(outerRadius)).Transposed();
		vertex.innerColor = innerColor;
		vertex.outerColor = outerColor;
		vertex.innerRadius = innerRadius;

		CHECK_RESULT(renderer.Render(ArrayView(&vertex, 1), [&](uint vertexIndex, uint vertexCount) {
			Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, vertexIndex, vertexCount);
			}));

		return Result();
	}

	Result Circle2DRenderer::Draw(ArrayView<Vertex> vertices) const
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

	Result Circle2DRenderer::SetShaderProgram(GraphicsLibrary::ShaderProgram&& program)
	{
		if (program.GetState() != GraphicsLibrary::ShaderProgramState::SuccesfullyLinked)
			return BLAZE_ERROR_RESULT("Blaze Engine", "The provided shader program needs to be succesfully linked for it to be set");

		this->program = std::move(program);
		return Result();
	}

	Result Circle2DRenderer::LoadDefaultShaderProgram() const
	{		
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;

		CHECK_RESULT(vert.Load("assets/default/shaders/circle2d/circle2d.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/circle2d/circle2d.geom"));
		CHECK_RESULT(frag.Load("assets/default/shaders/circle2d/circle2d.frag"));		

		CHECK_RESULT(program.LinkShaders({ &vert, &geom, &frag }));

		return Result();
	}

	Result Circle2DRenderer::CheckProgram() const
	{
		switch (program.GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaderProgram());
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			return Result();
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Current program wasn't succesfully linked");
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid ShaderProgramState value");
		}
	}

}