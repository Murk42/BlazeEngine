#pragma once
#include "BlazeEngine/Graphics/GraphicsLibrary.h"
#include "BlazeEngine/Graphics/Transform.h"
#include "BlazeEngine/Graphics/Utility/BatchStreamRenderer.h"

namespace Blaze::Graphics
{
	struct FrameGraphicsData
	{
		Vec2f size = Vec2f(100, 100);
		ColorRGBAf fillColor = 0x333333ff;
		ColorRGBAf borderColor = 0x222222ff;
		float borderWidth = 2.0f;
		float cornerRadius = 5.0f;
	};

	class FrameRenderer
	{
	public:
		FrameRenderer();		
		~FrameRenderer();

		Result SetProjectionMatrix(Mat4f mat);

		Result Render(Transform2D transform, const FrameGraphicsData& data) const;		
		Result Render(Mat4f matrix, const FrameGraphicsData& data) const;

		Result SetShaderProgram(GraphicsLibrary::ShaderProgram&&);
		Result LoadDefaultShaderProgram() const;
	private:
		mutable GraphicsLibrary::ShaderProgram program;
		mutable GraphicsLibrary::VertexArray va;
		Mat4f proj;

		mutable BatchStreamRenderer renderer;		

		Result CheckProgram() const;
	};
}