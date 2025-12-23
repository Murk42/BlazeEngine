#include "pch.h"
#include "LayoutControl.h"

static Vec2f ConvertAlignToPivot(Align align)
{
	switch (align)
	{
	case Blaze::Align::Top:         return Vec2f(0.5f, 1.0f);
	case Blaze::Align::TopRight:    return Vec2f(1.0f, 1.0f);
	case Blaze::Align::Right:       return Vec2f(1.0f, 0.5f);
	case Blaze::Align::BottomRight: return Vec2f(1.0f, 0.0f);
	case Blaze::Align::Bottom:      return Vec2f(0.5f, 0.0f);
	case Blaze::Align::BottomLeft:  return Vec2f(0.0f, 0.0f);
	case Blaze::Align::Left:        return Vec2f(0.0f, 0.5f);
	case Blaze::Align::TopLeft:     return Vec2f(0.0f, 1.0f);
	case Blaze::Align::Center:      return Vec2f(0.5f, 0.5f);
	default: return Vec2f(0, 0);
	}
}
static VerticalAlign GetVerticalAlignFromAlign(Align align)
{
	switch (align)
	{
	case Blaze::Align::TopLeft:
	case Blaze::Align::Top:
	case Blaze::Align::TopRight:
		return VerticalAlign::Top;
	case Blaze::Align::Left:
	case Blaze::Align::Center:
	case Blaze::Align::Right:
		return VerticalAlign::Center;
	case Blaze::Align::BottomLeft:
	case Blaze::Align::Bottom:
	case Blaze::Align::BottomRight:
		return VerticalAlign::Bottom;
	default: return VerticalAlign::Top;
	}
}
static float ConvertVerticalAlignToPivot(VerticalAlign align)
{
	switch (align)
	{
	case VerticalAlign::Bottom:    return 0.0f;
	case VerticalAlign::Center:    return 0.5f;
	case VerticalAlign::Top:       return 1.0f;
	default: return 0.0f;
	}
}
static float ConvertHorizontalAlignToPivot(HorizontalAlign align)
{
	switch (align)
	{
	case HorizontalAlign::Left:        return 0.0f;
	case HorizontalAlign::Center:      return 0.5f;
	case HorizontalAlign::Right:       return 1.0f;
	default: return 0.0f;
	}
}
/*
static PixelSpacing MaxSpacing(const PixelSpacing& a, const PixelSpacing& b)
{
	return {
		.top = std::max(a.top, b.top),
		.bottom = std::max(a.bottom, b.bottom),
		.left = std::max(a.left, b.left),
		.right = std::max(a.right, b.right)
	};
}
*/
namespace Blaze::UI::Layout
{
	constexpr Measure::Measure()
		: value(), type(MeasureType::Percent)
	{
	}

	constexpr Measure::Measure(float value, MeasureType type)
		: value(value), type(type)
	{
	}

	constexpr Measure::Measure(const Measure& other)
		: value(other.value), type(other.type)
	{
	}

	constexpr bool Measure::operator==(const Measure& other) const
	{
		return value == other.value && type == other.type;
	}

	constexpr bool Measure::operator!=(const Measure& other) const
	{
		return value != other.value || type != other.type;
	}

	constexpr Measure& Measure::operator=(const Measure& other)
	{
		value = other.value;
		type = other.type;
		return *this;
	}

	HorizontalWeightedFillLayout::HorizontalWeightedFillLayout()
		: layoutDirty(false), verticalAlign(VerticalAlign::Center)
	{
	}
	HorizontalWeightedFillLayout::HorizontalWeightedFillLayout(HorizontalWeightedFillLayout&& other) noexcept
		: layoutDirty(other.layoutDirty), verticalAlign(other.verticalAlign), nodesData(std::move(other.nodesData))
	{
		SwapHandlers(other);
	}
	HorizontalWeightedFillLayout::HorizontalWeightedFillLayout(ArrayView<NodeSetupData> nodes, VerticalAlign verticalAlign)
		: layoutDirty(false), verticalAlign(VerticalAlign::Center)
	{
		SetLayoutChildrenVerticalAlign(verticalAlign);
		SetNodes(nodes);
	}
	HorizontalWeightedFillLayout::~HorizontalWeightedFillLayout()
	{
		RemoveAllHandlers();
	}
	void HorizontalWeightedFillLayout::SetNodes(ArrayView<NodeSetupData> nodesSetupData)
	{
		RemoveAllHandlers();

		if (!nodesSetupData.Empty())
		{
			nodesData.ResizeWithFunction(nodesSetupData.Count(), [&](NodeData* data, uintMem index) {
				float weight = std::abs(nodesSetupData[index].weight);

				std::construct_at(data, NodeData{
					.node = nodesSetupData[index].node,
					.weight = weight,
					.offset = 0.0f,
					.width = weight != 0.0f ? 0.0f : nodesSetupData[index].node.GetTransform().size.x,
					});
				});

			AddAllHandlers();
		}
		else
			nodesData.Clear();

		MarkLayoutDirty();
	}
	void HorizontalWeightedFillLayout::SetLayoutChildrenVerticalAlign(VerticalAlign newVerticalAlign)
	{
		if (verticalAlign != newVerticalAlign)
		{
			verticalAlign = newVerticalAlign;
			MarkLayoutDirty();
		}
	}
	uintMem HorizontalWeightedFillLayout::GetNodeCount() const
	{
		return nodesData.Count();
	}
	UI::Node& HorizontalWeightedFillLayout::GetNode(uintMem index) const
	{
		if (index >= nodesData.Count())
		{
			Debug::Logger::LogFatal("Client", "Trying to get node with a index that is outside bounds");
			return *(UI::Node*)nullptr;
		}

		return nodesData[index].node;
	}
	float HorizontalWeightedFillLayout::GetNodeWeight(uintMem index) const
	{
		if (index >= nodesData.Count())
		{
			Debug::Logger::LogFatal("Client", "Trying to get node with a index that is outside bounds");
			return 0.0f;
		}

		return nodesData[index].weight;
	}
	HorizontalWeightedFillLayout& HorizontalWeightedFillLayout::operator=(HorizontalWeightedFillLayout&& other) noexcept
	{
		verticalAlign = other.verticalAlign;
		layoutDirty = other.layoutDirty;
		nodesData = std::move(other.nodesData);

		SwapHandlers(other);
		return *this;
	}
	void HorizontalWeightedFillLayout::Apply(UI::Node& parent, ArrayView<NodeSetupData> nodes, VerticalAlign verticalAlign)
	{
		parent.dataMap.map.Insert<HorizontalWeightedFillLayout>("HorizontalWeightedFillLayout", nodes, verticalAlign);
	}
	void HorizontalWeightedFillLayout::RemoveAllHandlers()
	{
		if (nodesData.Empty())
			return;

		nodesData.First().node.surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::FirstNodeSurroundingNodeTreeChangedEvent>(*this);

		if (auto parent = nodesData.First().node.GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(*this);

		for (auto& nodeData : nodesData)
		{
			nodeData.node.transformUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::NodeTransformUpdatedEvent>(*this);
			nodeData.node.enabledStateChangedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::ChildEnabledStateUpdated>(*this);
		}
	}
	void HorizontalWeightedFillLayout::AddAllHandlers()
	{
		if (nodesData.Empty())
			return;

		for (auto& nodeData : nodesData)
		{
			nodeData.node.transformUpdatedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::NodeTransformUpdatedEvent>(*this);
			nodeData.node.enabledStateChangedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::ChildEnabledStateUpdated>(*this);
		}

		nodesData.First().node.surroundingNodeTreeChangedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::FirstNodeSurroundingNodeTreeChangedEvent>(*this);

		if (auto parent = nodesData.First().node.GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(*this);
	}
	void HorizontalWeightedFillLayout::SwapHandlers(HorizontalWeightedFillLayout& other)
	{
		for (auto& nodeData : nodesData)
		{
			nodeData.node.transformUpdatedEventDispatcher.SwapHandlers<&HorizontalWeightedFillLayout::NodeTransformUpdatedEvent>(other, *this);
			nodeData.node.enabledStateChangedEventDispatcher.SwapHandlers<&HorizontalWeightedFillLayout::ChildEnabledStateUpdated>(other, *this);
		}

		nodesData.First().node.surroundingNodeTreeChangedEventDispatcher.SwapHandlers<&HorizontalWeightedFillLayout::FirstNodeSurroundingNodeTreeChangedEvent>(other, *this);

		if (auto parent = nodesData.First().node.GetParent())
			parent->transformUpdatedEventDispatcher.SwapHandlers<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(other, *this);
	}
	void HorizontalWeightedFillLayout::RecalculateLayout()
	{
		layoutDirty = false;

		auto parent = nodesData.Empty() ? nullptr : nodesData.First().node.GetParent();

		if (parent == nullptr)
			return;

		auto transform = parent->GetTransform();

		float availableWidth = transform.size.x * transform.scale;
		float weightSum = 0.0f;

		for (auto& nodeData : nodesData)
			if (nodeData.node.IsEnabled() && nodeData.node.GetParent() == parent)
				if (nodeData.weight == 0)
					availableWidth -= nodeData.width;
				else
					weightSum += nodeData.weight;

		if (availableWidth < 0.0f)
			weightSum = 0.0f;

		float offset = 0;
		for (auto& nodeData : nodesData)
		{
			if (!nodeData.node.IsEnabled() || nodeData.node.GetParent() != parent)
				continue;

			if (nodeData.weight != 0)
				if (weightSum == 0)
					nodeData.width = 0.0f;
				else
					nodeData.width = std::floor(availableWidth * nodeData.weight / weightSum);

			nodeData.offset = offset;

			offset += nodeData.width;
		}
	}
	void HorizontalWeightedFillLayout::MarkLayoutDirty()
	{
		if (nodesData.Empty() || nodesData.First().node.GetParent() == nullptr)
			return;

		layoutDirty = true;
		for (auto& nodeData : nodesData)
			nodeData.node.MarkTransformDirty();
	}
	void HorizontalWeightedFillLayout::ParentTransformUpdatedEvent(const UI::Node::TransformUpdatedEvent& event)
	{
		MarkLayoutDirty();
	}
	void HorizontalWeightedFillLayout::NodeTransformUpdatedEvent(const UI::Node::TransformUpdatedEvent& event)
	{
		auto parent = event.node.GetParent();
		if (!event.node.IsEnabled() || parent == nullptr || parent != nodesData.First().node.GetParent())
			return;

		uintMem i = 0;
		for (; i < nodesData.Count(); ++i)
			if (&nodesData[i].node == &event.node)
				break;

		if (i >= nodesData.Count())
		{
			BLAZE_LOG_ERROR("Internal error. Gotten an event for a node that is not a child to the current node.");
			return;
		}

		float verticalPivot = ConvertVerticalAlignToPivot(verticalAlign);
		auto nodeTransform = event.node.GetTransform();
		auto parentTransform = event.node.GetTransform();

		if (nodesData[i].weight != 0)
		{
			if (layoutDirty)
				RecalculateLayout();

			UI::NodeTransform transform{
				.pos = Vec2f(nodesData[i].offset / parentTransform.scale, 0),
				.parentPivot = Vec2f(0, verticalPivot),
				.pivot = Vec2f(0, verticalPivot),
				.size = Vec2f(nodesData[i].width / nodeTransform.scale, nodeTransform.size.y),
				.scale = 1.0f,
				.rotation = 0.0f
			};

			event.node.SetTransform(transform);
		}
		else
		{
			float newWidth = nodeTransform.size.x * nodeTransform.scale;
			if (nodesData[i].width != newWidth)
			{
				nodesData[i].width = newWidth;
				MarkLayoutDirty();
			}

			if (layoutDirty)
				RecalculateLayout();

			UI::NodeTransform transform{
				.pos = Vec2f(nodesData[i].offset / parentTransform.scale, 0),
				.parentPivot = Vec2f(0, verticalPivot),
				.pivot = Vec2f(0, verticalPivot),
				.size = nodeTransform.size,
				.scale = 1.0f,
				.rotation = 0.0f
			};

			event.node.SetTransform(transform);
		}
	}
	void HorizontalWeightedFillLayout::ChildEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event)
	{
		MarkLayoutDirty();
	}
	void HorizontalWeightedFillLayout::FirstNodeSurroundingNodeTreeChangedEvent(const UI::Node::SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
		{
			if (event.otherNode != nullptr)
				event.otherNode->transformUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(*this);

			if (auto newParent = event.node.GetParent())
				newParent->transformUpdatedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(*this);

			MarkLayoutDirty();
		}
	}

	VerticalArrayLayoutNode::VerticalArrayLayoutNode(UI::Node& parent, const NodeTransform& transform, float distance, Align childNodesAlignment)
		: layoutDirty(false), childNodesAlignment(Align::TopLeft), distance(5)
	{
		SetTransform(transform);
		SetParent(&parent);

		surroundingNodeTreeChangedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::TransformUpdated>(*this);

		SetChildNodesAlign(childNodesAlignment);
		SetChildNodesDistance(distance);
	}
	VerticalArrayLayoutNode::~VerticalArrayLayoutNode()
	{
		surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::TransformUpdated>(*this);
	}
	void VerticalArrayLayoutNode::SetChildNodesAlign(Align newChildNodesAlignment)
	{
		if (childNodesAlignment != newChildNodesAlignment)
		{
			childNodesAlignment = newChildNodesAlignment;
			MarkLayoutDirty();
		}
	}
	void VerticalArrayLayoutNode::SetChildNodesDistance(float newDistance)
	{
		if (distance != newDistance)
		{
			distance = newDistance;
			MarkLayoutDirty();
		}
	}
	Align VerticalArrayLayoutNode::GetChildNodesAlign() const
	{
		return childNodesAlignment;
	}
	float VerticalArrayLayoutNode::GetNodesDistance() const
	{
		return distance;
	}
	void VerticalArrayLayoutNode::RecalculateLayout()
	{
		layoutDirty = false;

		if (GetChildren().Empty())
			return;

		float totalHeight = 0;
		for (auto& child : GetChildren())
			if (child.IsEnabled())
				totalHeight += child.GetTransform().size.y + distance;

		totalHeight -= distance;

		const float verticalAlignPivot = ConvertVerticalAlignToPivot(GetVerticalAlignFromAlign(childNodesAlignment));

		float offset = totalHeight;
		uintMem i = 0;
		for (auto& child : GetChildren())
		{
			if (child.IsEnabled())
			{
				Vec2f pivot = ConvertAlignToPivot(childNodesAlignment);

				auto transform = child.GetTransform();

				transform.pos = Vec2f(0, offset - verticalAlignPivot * totalHeight);
				transform.parentPivot = pivot;
				transform.pivot = Vec2f(pivot.x, 1.0f);
				transform.scale = 1.0f;
				transform.rotation = 0.0f;

				child.SetTransform(transform);

				offset -= child.GetTransform().size.y + distance;
			}

			++i;
		}

	}
	void VerticalArrayLayoutNode::MarkLayoutDirty()
	{
		layoutDirty = true;
		for (auto& child : GetChildren())
			child.MarkTransformDirty();
	}
	void VerticalArrayLayoutNode::SurroungingTreeChanged(const SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildAdded)
		{
			event.node.transformFilterEventDispatcher.AddHandler<&VerticalArrayLayoutNode::FilterChildTransform>(*this);
			event.node.enabledStateChangedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::ChildEnabledStateUpdated>(*this);
			MarkLayoutDirty();
		}
		else if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildRemoved)
		{
			event.node.transformFilterEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::FilterChildTransform>(*this);
			event.node.enabledStateChangedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::ChildEnabledStateUpdated>(*this);
			MarkLayoutDirty();
		}
	}
	void VerticalArrayLayoutNode::TransformUpdated(const TransformUpdatedEvent& event)
	{
		//MarkLayoutDirty();
	}
	void VerticalArrayLayoutNode::FilterChildTransform(const TransformFilterEvent& event)
	{
		if (layoutDirty)
			RecalculateLayout();
	}
	void VerticalArrayLayoutNode::ChildEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event)
	{
		MarkLayoutDirty();
	}

	ParentTransformBinding::ParentTransformBinding()
		: node(nullptr)
	{
	}
	ParentTransformBinding::ParentTransformBinding(UI::Node* node)
		: ParentTransformBinding()
	{
 		SetNode(node);
	}
	ParentTransformBinding::~ParentTransformBinding()
	{
		SetNode(nullptr);
	}
	void ParentTransformBinding::SetNode(UI::Node* newNode)
	{
		if (node == newNode)
			return;

		if (node != nullptr)
		{
			node->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildSurroungingTreeChanged>(*this);
			node->transformFilterEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildTransformFilter>(*this);
			if (auto parent = node->GetParent())
				parent->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
		}

		node = newNode;

		if (node != nullptr)
		{
			node->surroundingNodeTreeChangedEventDispatcher.AddHandler<&ParentTransformBinding::ChildSurroungingTreeChanged>(*this);
			node->transformFilterEventDispatcher.AddHandler<&ParentTransformBinding::ChildTransformFilter>(*this);
			if (auto parent = node->GetParent())
				parent->transformUpdatedEventDispatcher.AddHandler<&ParentTransformBinding::ParentTransformChanged>(*this);

			node->MarkTransformDirty();
		}
	}
	void ParentTransformBinding::ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
		{
			if (auto parent = event.otherNode)
				parent->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ParentTransformChanged>(*this);

			if (auto parent = node->GetParent())
			{
				parent->transformUpdatedEventDispatcher.AddHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
				node->MarkTransformDirty();
			}
		}
	}
	void ParentTransformBinding::ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event)
	{
		node->MarkTransformDirty();
	}
	void ParentTransformBinding::ChildTransformFilter(const UI::Node::TransformFilterEvent& event)
	{
		ApplyTransformToChild();
	}

	ParentWidthBinding::ParentWidthBinding(UI::Node* node)
		: ParentTransformBinding(node)
	{
	}
	void ParentWidthBinding::ApplyTransformToChild()
	{
		auto childTransform = GetNode()->GetTransform();
		childTransform.size.x = GetNode()->GetParent()->GetTransform().size.x;
		GetNode()->SetTransform(childTransform);
	}

	ParentHeightBinding::ParentHeightBinding(UI::Node* node)
		: ParentTransformBinding(node)
	{
	}
	void ParentHeightBinding::ApplyTransformToChild()
	{
		auto childTransform = GetNode()->GetTransform();
		childTransform.size.x = GetNode()->GetParent()->GetTransform().size.x;
		GetNode()->SetTransform(childTransform);
	}

	ParentSizeBinding::ParentSizeBinding(UI::Node* node)
		: ParentTransformBinding(node)
	{
	}
	void ParentSizeBinding::ApplyTransformToChild()
	{
		auto childTransform = GetNode()->GetTransform();
		childTransform.size = GetNode()->GetParent()->GetTransform().size;
		GetNode()->SetTransform(childTransform);
	}


	DivisionNode::DivisionNode()
	{
		surroundingNodeTreeChangedEventDispatcher.AddHandler<&DivisionNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.AddHandler<&DivisionNode::TransformChanged>(*this);
	}
	DivisionNode::DivisionNode(Node& parent)
		: DivisionNode()
	{
		SetParent(&parent);

		for (auto& child : GetChildren())
			child.MarkTransformDirty();
	}
	DivisionNode::~DivisionNode()
	{
		surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&DivisionNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.RemoveHandler<&DivisionNode::TransformChanged>(*this);
	}
	void DivisionNode::SurroungingTreeChanged(const SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildAdded)
		{
			event.node.transformFilterEventDispatcher.AddHandler<&DivisionNode::ChildTransformFilter>(*this);

			for (auto& child : GetChildren())
				child.MarkTransformDirty();
		}
		else if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildRemoved)
		{
			event.node.transformFilterEventDispatcher.RemoveHandler<&DivisionNode::ChildTransformFilter>(*this);

			for (auto& child : GetChildren())
				child.MarkTransformDirty();
		}

	}
	void DivisionNode::TransformChanged(const TransformUpdatedEvent& event)
	{
		for (auto& child : GetChildren())
			child.MarkTransformDirty();
	}
	void DivisionNode::ChildTransformFilter(const TransformFilterEvent& event)
	{
		CleanTransform();
		FilterChildTransform(event.node);
	}

	HorizontalDivisonNode::HorizontalDivisonNode(Node& parent, uintMem flexibleNodeIndex, bool topToDownOrder)
		: DivisionNode(parent), flexibleNodeIndex(flexibleNodeIndex), topToDownOrder(topToDownOrder)
	{
	}
	void HorizontalDivisonNode::FilterChildTransform(Node& child)
	{
		uintMem i = 0;
		for (auto& c : GetChildren())
			if (&c == &child)
				break;
			else
				++i;

		auto transform = GetTransform();
		auto children = GetChildren();

		float offset = 0.0f;

		for (uintMem j = 0; j < i; ++j)
			offset += children[j].GetTransform().size.y;

		if (topToDownOrder)
			offset = -offset;

		if (i == flexibleNodeIndex)
		{
			float totalFixedHeight = 0.0f;
			for (uintMem j = 0; j < children.Count(); ++j)
				if (j != flexibleNodeIndex)
					totalFixedHeight += children[j].GetTransform().size.y;

			child.SetTransform(NodeTransform{
				.pos = Vec2f(0.0f, offset),
				.parentPivot = Vec2f(0.0f, topToDownOrder ? 1.0f : 0.0f),
				.pivot = Vec2f(0.0f, topToDownOrder ? 1.0f : 0.0f),
				.size = Vec2f(transform.size.x, std::max(transform.size.y - totalFixedHeight, 0.0f)),
				.scale = 1.0f,
				.rotation = 0.0f
				});
		}
		else
		{
			child.SetTransform(NodeTransform{
				.pos = Vec2f(0.0f, offset),
				.parentPivot = Vec2f(0.0f, topToDownOrder ? 1.0f : 0.0f),
				.pivot = Vec2f(0.0f, topToDownOrder ? 1.0f : 0.0f),
				.size = Vec2f(transform.size.x, children[i].GetTransform().size.y),
				.scale = 1.0f,
				.rotation = 0.0f
				});
		}
	}
	VerticalDivisonNode::VerticalDivisonNode(Node& parent, uintMem flexibleNodeIndex, bool leftToRightOrder)
		: DivisionNode(parent), flexibleNodeIndex(flexibleNodeIndex), leftToRightOrder(leftToRightOrder)
	{
	}
	void VerticalDivisonNode::FilterChildTransform(Node& child)
	{
		uintMem i = 0;
		for (auto& c : GetChildren())
			if (&c == &child)
				break;
			else
				++i;

		auto transform = GetTransform();
		auto children = GetChildren();

		float offset = 0.0f;
		for (uintMem j = 0; j < i; ++j)
			offset += children[j].GetTransform().size.x;

		if (!leftToRightOrder)
			offset = -offset;

		if (i == flexibleNodeIndex)
		{
			float totalFixedWidth = 0.0f;
			for (uintMem j = 0; j < children.Count(); ++j)
				if (j != flexibleNodeIndex)
					totalFixedWidth += children[j].GetTransform().size.x;

			child.SetTransform(NodeTransform{
				.pos = Vec2f(offset, 0.0f),
				.parentPivot = Vec2f(leftToRightOrder ? 0.0f : 1.0f, 0.0f),
				.pivot = Vec2f(leftToRightOrder ? 0.0f : 1.0f, 0.0f),
				.size = Vec2f(std::max(transform.size.x - totalFixedWidth, 0.0f), transform.size.y),
				.scale = 1.0f,
				.rotation = 0.0f
				});
		}
		else
		{
			child.SetTransform(NodeTransform{
				.pos = Vec2f(offset, 0.0f),
				.parentPivot = Vec2f(leftToRightOrder ? 0.0f : 1.0f, 0.0f),
				.pivot = Vec2f(leftToRightOrder ? 0.0f : 1.0f, 0.0f),
				.size = Vec2f(children[i].GetTransform().size.x, transform.size.y),
				.scale = 1.0f,
				.rotation = 0.0f
				});
		}
	}
}