#pragma once
#include "BlazeEngineGraphics\Vulkan/Core/GraphicsContext_Vulkan.h"
#include "vulkan/vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	struct Line2DRenderData_Vulkan
	{
		Vec2f p1, p2;
		ColorRGBAf color;
		float width;
	};

	class Line2DRenderCache_Vulkan;
	
	class VertexAttributeData
	{
	public:
		constexpr VertexAttributeData(uintMem offset, VkFormat format, uint32 location, uint32 binding)
			: offset(offset), format(format), location(location), binding(binding)
		{

		}

		constexpr uintMem GetOffset() const { return offset; }
		constexpr VkFormat GetVulkanFormat() const { return format; }			
		constexpr uint32 GetLocation() const { return location; }
		constexpr uint32 GetBinding() const { return binding; }
	private:
		const uintMem offset;
		const VkFormat format;
		const uint32 location;
		const uint32 binding;
	};

	template<typename _Type, uint32 _location, uint32 _binding>
	class VertexAttribute
	{
	public:
		using Type = _Type;		
		static constexpr uint32 location = _location;
		static constexpr uint32 binding = _binding;
	};

	template<typename T>
	struct _IsVertexAttribute : std::false_type { };
	template<typename _Type, uintMem _location, uintMem _binding>
	struct _IsVertexAttribute<VertexAttribute<_Type, _location, _binding>> : std::true_type { };

	template<typename T>
	concept VertexAttributeType = _IsVertexAttribute<T>::value;

	template<VertexAttributeType ... T>
	class VertexLayout
	{
	public:
		VertexLayout() { }

		constexpr static uintMem AttributeCount() { return sizeof...(T); }		
		constexpr static VertexAttributeData GetAttribute(uint index) 
		{ 
			uintMem offset = 0;
			for (uint i = 0; i < index; ++i)
				offset += sizes[i];

			return VertexAttributeData(offset, formats[index], locations[index], bindings[index]);
		}			

		constexpr static std::array<VkVertexInputAttributeDescription, sizeof...(T)> GetVulkanVertexInputData()
		{
			std::array<VkVertexInputAttributeDescription, sizeof...(T)> vertexAttributeDescriptions;

			for (uint i = 0; i < AttributeCount(); ++i)
			{
				auto attribute = GetAttribute(i);
				vertexAttributeDescriptions[i].offset = (uint32)attribute.GetOffset();
				vertexAttributeDescriptions[i].binding = attribute.GetBinding();
				vertexAttributeDescriptions[i].location = attribute.GetLocation();
				vertexAttributeDescriptions[i].format = attribute.GetVulkanFormat();
			}

			return vertexAttributeDescriptions;
		}
	private:
		template<VertexAttributeType T>
		constexpr static VkFormat GetVulkanFormat()
		{
			using Type = typename T::template Type;
			if constexpr (std::same_as<Type, float>)
				return VK_FORMAT_R32_SFLOAT;
			else if constexpr (std::same_as<Type, Vec2f>)
				return VK_FORMAT_R32G32_SFLOAT;
			else if constexpr (std::same_as<Type, Vec3f>)
				return VK_FORMAT_R32G32B32_SFLOAT;
			else if constexpr (std::same_as<Type, Vec4f>)
				return VK_FORMAT_R32G32B32A32_SFLOAT;
		}

		static constexpr std::array<uintMem, sizeof...(T)> sizes = { { sizeof(typename T::template Type)... } };
		static constexpr std::array<VkFormat, sizeof...(T)> formats = { { GetVulkanFormat<T>()... } };
		static constexpr std::array<uint32, sizeof...(T)> locations = { { T::template location... } };
		static constexpr std::array<uint32, sizeof...(T)> bindings = { { T::template binding... } };

	};

	class BLAZE_GRAPHICS_API Line2DRenderer_Vulkan
	{
	public:
		Line2DRenderer_Vulkan(GraphicsContext_Vulkan& graphicsContext, VkRenderPass renderPass);
		~Line2DRenderer_Vulkan();
				
		void Render(Line2DRenderCache_Vulkan& renderCache, Vec2u targetSize, VkCommandBuffer commandBuffer, VkSemaphore& commandBufferFinishedSignalSemaphores, uint64& commandBufferFinishedSignalValue);

		inline GraphicsContext_Vulkan& GetGraphicsContext() { return graphicsContext; }
	private:				
		struct Vertex
		{
			Vec2f pos;
			Vec2f pos1;
			Vec2f pos2;
			Vec4f color;
			float width;

			using VertexLayout = VertexLayout<
				VertexAttribute<Vec2f, 0, 0>,
				VertexAttribute<Vec2f, 1, 0>,
				VertexAttribute<Vec2f, 2, 0>,
				VertexAttribute<Vec4f, 3, 0>,
				VertexAttribute<float, 4, 0>
			>;
		};

		GraphicsContext_Vulkan& graphicsContext;		
		VkPipelineLayout pipelineLayout;
		VkPipeline pipeline;				

		friend class Line2DRenderCache_Vulkan;
	};

	class BLAZE_GRAPHICS_API Line2DRenderCache_Vulkan
	{
	public:
		Line2DRenderCache_Vulkan(GraphicsContext_Vulkan& graphicsContext);
		~Line2DRenderCache_Vulkan();

		/*
		Returns a semaphore that should be signaled when the frame finishes
		*/
		void CreateCache(const Array<Line2DRenderData_Vulkan>& renderData, VkCommandBuffer commandBuffer);		
	private:
		using Vertex = Line2DRenderer_Vulkan::Vertex;
		GraphicsContext_Vulkan& graphicsContext;

		uintMem vertexCount;

		VkBuffer buffer;
		VkDeviceMemory bufferMemory;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		uint64 nextSemaphoreSignalValue;
		VkSemaphore semaphore;		

		friend class Line2DRenderer_Vulkan;
	};
}