#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/Math/Matrix.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"

namespace Blaze::Graphics
{
	class BasicIndexedMeshRenderer : public virtual RendererBase
	{
	public:
		virtual void SetProjectionMatrix(const Mat4f& matrix) = 0;
		virtual void SetViewMatrix(const Mat4f& matrix) = 0;
		virtual void SetShadingOptions(Vec3f lightDirection, ColorRGBf ambientColor = 0x000000) = 0;

		virtual void Render(ArrayView<Vec3f> vertices, ArrayView<uint32> indices, Mat4f modelMatrix, ColorRGBAf color) = 0;

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<BasicIndexedMeshRenderer>(); }
	};
}