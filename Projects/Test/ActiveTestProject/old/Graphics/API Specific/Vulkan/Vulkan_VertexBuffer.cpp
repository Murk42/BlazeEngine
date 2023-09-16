#include "Vulkan_VertexBuffer.h"
#include "Vulkan_CommandBufferPool.h"
#include "Vulkan_CommandQueue.h"

namespace Experimental::Graphics::Vulkan
{
    uint32_t FindMemoryType(RenderContextImpl* renderContext, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateBuffer(RenderContextImpl* renderContext, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void CopyBuffer(RenderContextImpl* renderContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, VkQueue queue);

    VertexBufferImpl::VertexBufferImpl(RenderContextImpl* renderContext, const void* data, uint size, CommandBufferPoolImpl* commandPool, CommandQueueImpl* queue)
        : renderContext(renderContext)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        CreateBuffer(renderContext, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* map;
        vkMapMemory(renderContext->device, stagingBufferMemory, 0, size, 0, &map);
        memcpy(map, data, size);
        vkUnmapMemory(renderContext->device, stagingBufferMemory);

        CreateBuffer(renderContext, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, memory);

        CopyBuffer(renderContext, stagingBuffer, vertexBuffer, size, commandPool->commandPool, queue->queue);

        vkDestroyBuffer(renderContext->device, stagingBuffer, nullptr);
        vkFreeMemory(renderContext->device, stagingBufferMemory, nullptr);
    }
    VertexBufferImpl::~VertexBufferImpl()
    {
        vkDestroyBuffer(renderContext->device, vertexBuffer, nullptr);
        vkFreeMemory(renderContext->device, memory, nullptr);
    }
}