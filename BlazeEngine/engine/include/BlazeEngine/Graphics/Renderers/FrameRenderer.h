#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Renderers/BatchStreamRenderer.h"

namespace Blaze::Graphics
{
	class FrameRenderer
	{
	public:
		FrameRenderer();		
		~FrameRenderer();

		Result Render(Vec2f pos, Vec2f size, ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius);

		void SetProjectionMatrix(Mat4f mat);

		Result SetShaders(std::initializer_list<Graphics::Core::Shader*> shaders);		
		Result LoadDefaultShaders();
	private:
		Graphics::Core::ShaderProgram program;
		Graphics::Core::VertexArray va;
		Mat4f proj;

		//BatchStreamRenderer renderer;		
	};
}