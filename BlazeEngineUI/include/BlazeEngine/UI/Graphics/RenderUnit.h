#pragma once
#include "BlazeEngine/Core/String/FixedString.h"
#include "BlazeEngine/Graphics/Renderers/RendererRegistry.h"
#include "BlazeEngine/UI/Graphics/RenderContext.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	class BLAZE_API RenderUnitBase
	{
	public:
		virtual StringView GetRequiredRendererName() const = 0;
		virtual uint64 GetRequiredRendererTypeID() const = 0;
		virtual void Render(const Node& node, Graphics::RendererBase& renderer, const RenderContext& renderContext) = 0;
	};

	template<IsDerivedFrom<Graphics::RendererBase> RendererType, FixedString name = "">
	class BLAZE_API RenderUnit : public RenderUnitBase
	{
	public:
		StringView GetRequiredRendererName() const override final
		{
			return name;
		}
		uint64 GetRequiredRendererTypeID() const override final
		{
			return Graphics::RendererBase::GetTypeID<RendererType>();
		}
		void Render(const Node& node, Graphics::RendererBase& renderer, const RenderContext& renderContext) override final
		{
			Render(node, static_cast<RendererType&>(renderer), renderContext);
		}
		virtual void Render(const Node& node, RendererType& renderer, const RenderContext& renderContext) = 0;
	};
	template<FixedString name>
	class BLAZE_API RenderUnit<Graphics::RendererBase, name> : public RenderUnitBase
	{
	public:
		StringView GetRequiredRendererName() const override final
		{
			return name;
		}
		uint64 GetRequiredRendererTypeID() const override final
		{
			return 0;
		}
	};
}