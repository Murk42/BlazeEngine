#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/ImageRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"

namespace Blaze::UI::Nodes
{
	enum class ImageLayout
	{
		Fit,
		Stretch,
		Fill,
		KeepWidth,
		KeepHeight
	};

	class BLAZE_API Image : public Node, public RenderableNode
	{
	public:
		Image();
		Image(Node& parent, const NodeTransform& transform, const ImageStyle& style = {}, ImageLayout layout = ImageLayout::Fit);
		~Image();

		void SetLayout(ImageLayout layout);
		ImageLayout GetLayout() const;

		void SetStyle(const ImageStyle& style);
		ImageStyle GetStyle() const;

		RenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		ImageRenderUnit renderUnit;
		ImageStyle style;
		ImageLayout layout;

		void UpdateImageRenderUnit();

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(Image, FinalTransformUpdatedEvent, finalTransformUpdatedEventDispatcher);
	};
}