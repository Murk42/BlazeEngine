#pragma once
#include "BlazeEngine/Core/String/String.h"

namespace Blaze::Graphics::Dynamic
{
	class BLAZE_API GraphicsContextBase
	{
	public:
		GraphicsContextBase() { }
		GraphicsContextBase(const GraphicsContextBase& other) { }
		GraphicsContextBase(GraphicsContextBase&& other) noexcept { }
		virtual ~GraphicsContextBase() { }

		virtual String GetImplementationName() const = 0;

		GraphicsContextBase& operator=(const GraphicsContextBase& other) { return *this; }
		GraphicsContextBase& operator=(GraphicsContextBase&& other) noexcept { return *this; }
	};
}