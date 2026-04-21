#pragma once
#include "BlazeEngine/Graphics/Core/FramebufferBase.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API FramebufferBase_OpenGL : public FramebufferBase
	{
	public:
		FramebufferBase_OpenGL(const FramebufferBase_OpenGL&) = delete;
		FramebufferBase_OpenGL(FramebufferBase_OpenGL&&) noexcept;
		FramebufferBase_OpenGL(uint32 id);
		~FramebufferBase_OpenGL() override = default;

		inline uint GetHandle() const { return id; }

		void ClearColor(Vec4i color = Vec4i(0), uint drawBuffer = 0);
		void ClearColor(Vec4u color = Vec4u(0), uint drawBuffer = 0);
		void ClearColor(Vec4f color = Vec4f(0.0f), uint drawBuffer = 0);
		void ClearDepth(float value = 1.0f);
		void ClearStencil(int value = 0);
		void ClearDepthStencil(float depthValue = 1.0f, int stencilValue = 0);		

		FramebufferBase_OpenGL& operator=(const FramebufferBase_OpenGL&) = delete;
		FramebufferBase_OpenGL& operator=(FramebufferBase_OpenGL&&) noexcept;
	protected:
		uint32 id;
	};
}