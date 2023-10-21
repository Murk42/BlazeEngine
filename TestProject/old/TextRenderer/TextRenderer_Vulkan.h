#pragma once
#include "../Graphics3/Vulkan/API_Vulkan.h"
#include "TextRendererBase.h"
#include "TextRenderer_Dynamic.h"

namespace Graphics3::Vulkan
{
	class TextRenderer_Vulkan : public TextRendererBase
	{
		struct Vertex
		{
			Vec2f pos;
			Vec2f uv;			
			Vec4f color;
		};

		GraphicsContext* graphicsContext;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;
		VkCommandPool commandPool;

		VmaAllocation stagingVertexBufferAllocation;
		VkBuffer stagingVertexBuffer;
		void* vertexBufferPtr;
		uint vertexBufferSize;

		VmaAllocation vertexBufferAllocation;
		VkBuffer vertexBuffer;
		
		static constexpr uint commandBufferCount = 5;
		VkCommandBuffer commandBuffers[commandBufferCount];
		VkFence commandBufferFences[commandBufferCount];
		uint commandBufferIndex;
	public:			
		TextRenderer_Vulkan(GraphicsContext& graphicsContext, RenderContext& transferRenderContext);
		~TextRenderer_Vulkan();

		void RenderText(Dynamic::RenderContextBase& context, Dynamic::FramebufferBase& target, TextRenderData renderData, std::initializer_list<Dynamic::Semaphore*> waitSemaphores, std::initializer_list<Dynamic::Semaphore*> signalSemaphores) override
		{
			Array<Semaphore*> _waitSemaphores { (uint)waitSemaphores.size() };
			Array<Semaphore*> _signalSemaphores { (uint)signalSemaphores.size() };
			for (uint i = 0; i < (uint)waitSemaphores.size(); ++i) _waitSemaphores[i] = (Semaphore*)waitSemaphores.begin()[i]->GetPtr();
			for (uint i = 0; i < (uint)signalSemaphores.size(); ++i) _signalSemaphores[i] = (Semaphore*)signalSemaphores.begin()[i]->GetPtr();
			RenderText((RenderContext&)context, (Framebuffer&)target, renderData, 
				std::initializer_list<Semaphore*>(_waitSemaphores.Ptr(), _waitSemaphores.Ptr() + _waitSemaphores.Count()), 
				std::initializer_list<Semaphore*>(_signalSemaphores.Ptr(), _signalSemaphores.Ptr() + _signalSemaphores.Count())
			);
		}
		void RenderText(RenderContext& context, Framebuffer& target, TextRenderData renderData, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores);
	};

	using TextRenderer = TextRenderer_Vulkan;
}