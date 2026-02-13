#pragma once
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Graphics/Core/GraphicsContextBase.h"
#include "BlazeEngine/Graphics/Renderers/RenderContext.h"

namespace Blaze::Graphics
{
	using RendererTypeID = uint64;

	class BLAZE_API RendererBase
	{
	public:
		virtual void StartRender(const RenderContext& context) = 0;
		virtual void EndRender(const RenderContext& context) = 0;

		virtual RendererTypeID GetTypeID() const = 0;
		virtual GraphicsContextBase& GetGraphicsContext() const = 0;

		template<typename T>
		static RendererTypeID GetTypeID()
		{
			static RendererTypeID const typeID = typeIDCounter++;
			return typeID;
		}
	private:
		inline static std::atomic_uint64_t typeIDCounter = 0;
	};
}