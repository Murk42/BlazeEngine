#pragma once
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Graphics/Renderers/RenderContext.h"

namespace Blaze::Graphics
{
	class BLAZE_API RendererBase
	{
	public:
		virtual void StartRender(const RenderContext& context) = 0;
		virtual void EndRender(const RenderContext& context) = 0;

		virtual uint64 GetTypeID() const = 0;

		template<typename T>
		static uint64 GetTypeID()
		{
			static uint64 const typeID = typeIDCounter++;
			return typeID;
		}
	private:
		inline static std::atomic_uint64_t typeIDCounter = 0;
	};
}