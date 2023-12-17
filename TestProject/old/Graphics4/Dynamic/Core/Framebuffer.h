#pragma once
#include "DynamicImplementation.h"

namespace Graphics4::Dynamic
{
	class FramebufferBase : public DynamicImplementation<FramebufferBase>
	{
	public:		
		FramebufferBase(FramebufferBase&& other) noexcept;
		virtual ~FramebufferBase();

		virtual Vec2u GetSize() const;
		
		FramebufferBase& operator=(FramebufferBase&& other) noexcept;
	protected:
		FramebufferBase();
	};
}