#pragma once
#include "BlazeEngine/Graphics/Core/OpenGL/FramebufferBase_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"
#include "BlazeEngine/Core/Container/ArrayView.h"

namespace Blaze::Graphics::OpenGL
{		
	class RenderWindow_OpenGL;

	struct RenderWindowFramebufferDrawBufferInfo
	{
		uint drawBufferIndex;
		RenderWindowFramebufferBufferTarget target;
	};

	class BLAZE_API RenderWindowFramebuffer_OpenGL : public FramebufferBase_OpenGL
	{
	public:
		~RenderWindowFramebuffer_OpenGL() override;

		inline Vec2u GetSize() const override { return size; }

		void SetDrawBuffer(RenderWindowFramebufferBufferTargets target);
		void SetDrawBuffers(const ArrayView<RenderWindowFramebufferDrawBufferInfo>& drawBuffers);
		void DisableDrawBuffers();
	private:
		Vec2u size;

		RenderWindowFramebuffer_OpenGL(uint32 id, Vec2u size);
		friend class RenderWindow_OpenGL;
	};

}