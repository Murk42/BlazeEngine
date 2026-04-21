#pragma once
#include "BlazeEngine/Core/String/FixedString.h"
#include "BlazeEngine/Graphics/Core/RenderContext.h"
#include "BlazeEngine/Graphics/Core/RendererBase.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	class BLAZE_API RenderUnitBase
	{
	public:
		virtual StringView GetRequiredRendererName() const = 0;
		virtual Graphics::RendererTypeID GetRequiredRendererTypeID() const = 0;
		virtual void Render(const Node& node, Graphics::RendererBase& renderer) = 0;
	};

	template<IsDerivedFrom<Graphics::RendererBase> RendererType, FixedString name = "">
	class BLAZE_API RenderUnit : public RenderUnitBase
	{
	public:
		StringView GetRequiredRendererName() const override
		{
			return name;
		}
		Graphics::RendererTypeID GetRequiredRendererTypeID() const override
		{
			return Graphics::RendererBase::GetTypeIDFor<RendererType>();
		}
		void Render(const Node& node, Graphics::RendererBase& renderer) override final
		{
			RendererType* renderer_specific = dynamic_cast<RendererType*>(&renderer);

			if (renderer_specific == nullptr)
			{
				BLAZE_LOG_ERROR("Trying to render a render unit with an innapropriate renderer type");
				return;
			}

			Render(node, *renderer_specific);
		}
		virtual void Render(const Node& node, RendererType& renderer)  = 0;
	};
	template<FixedString name>
	class BLAZE_API RenderUnit<Graphics::RendererBase, name> : public RenderUnitBase
	{
	public:
		StringView GetRequiredRendererName() const override
		{
			return name;
		}
		Graphics::RendererTypeID GetRequiredRendererTypeID() const override
		{
			return 0;
		}
	};
}