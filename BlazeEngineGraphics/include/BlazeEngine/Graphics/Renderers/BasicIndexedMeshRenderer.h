#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Math/Matrix.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"

namespace Blaze::Graphics
{
	class BasicIndexedMeshRenderer : public RendererBase
	{
	public:
		virtual void SetProjectionMatrix(const Mat4f& matrix) = 0;
		virtual void SetViewMatrix(const Mat4f& matrix) = 0;
		virtual void SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor = 0x000000) = 0;

		virtual void Render(ArrayView<Vec3f> vertices, ArrayView<uint32> indices, Mat4f modelMatrix, ColorRGBAf color, const RenderContext& context) = 0;

		uint64 GetTypeID() const { return RendererBase::GetTypeID<BasicIndexedMeshRenderer>(); }
	};
}