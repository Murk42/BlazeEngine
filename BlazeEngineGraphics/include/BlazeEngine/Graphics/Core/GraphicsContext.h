#pragma once
#include "BlazeEngine/Core/String/String.h"

namespace Blaze::Graphics
{
	class BLAZE_API GraphicsContext
	{
	public:
		GraphicsContext() = default;
		GraphicsContext(const GraphicsContext& other) = default;
		GraphicsContext(GraphicsContext&& other) noexcept = default;
		virtual ~GraphicsContext() { }

		virtual StringView GetImplementationName() const = 0;

		GraphicsContext& operator=(const GraphicsContext& other) = default;
		GraphicsContext& operator=(GraphicsContext&& other) noexcept = default;
	};
}