#pragma once
#include "BlazeEngine/Core/String/FixedString.h"
#include "BlazeEngine/Graphics/Renderers/RendererRegistry.h"
#include "BlazeEngine/UI/Graphics/UIRenderContext.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	class BLAZE_API UIRenderUnitBase
	{
	public:
		virtual StringView GetRequiredRendererName() const = 0;
		virtual uint64 GetRequiredRendererTypeID() const = 0;
		virtual void Render(const Node& node, Graphics::RendererBase& renderer, const UIRenderContext& renderContext) = 0;
	};

	template<IsBaseOf<Graphics::RendererBase> RendererType, FixedString name = "">
	class BLAZE_API UIRenderUnit : public UIRenderUnitBase
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
		void Render(const Node& node, Graphics::RendererBase& renderer, const UIRenderContext& renderContext) override final
		{
			Render(node, static_cast<RendererType&>(renderer), renderContext);
		}
		virtual void Render(const Node& node, RendererType& renderer, const UIRenderContext& renderContext) = 0;
	};
	template<FixedString name>
	class BLAZE_API UIRenderUnit<Graphics::RendererBase, name> : public UIRenderUnitBase
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