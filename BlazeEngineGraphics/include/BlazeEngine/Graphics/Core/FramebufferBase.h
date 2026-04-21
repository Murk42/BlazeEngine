#pragma once
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::Graphics
{
	class BLAZE_API FramebufferBase
	{
	public:
		FramebufferBase() = default;
		FramebufferBase(const FramebufferBase& other) = default;
		FramebufferBase(FramebufferBase&& other) noexcept = default;
		virtual ~FramebufferBase() = default;

		virtual Vec2u GetSize() const = 0;

		FramebufferBase& operator=(const FramebufferBase& other) = default;
		FramebufferBase& operator=(FramebufferBase && other) noexcept = default;
	};
}