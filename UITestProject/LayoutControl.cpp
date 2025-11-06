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
	}
}
static float ConvertVerticalAlignToPivot(VerticalAlign align)
{
	switch (align)
	{
	case VerticalAlign::Bottom:    return 0.0f;
	case VerticalAlign::Center:    return 0.5f;
	case VerticalAlign::Top:       return 1.0f;
	}
}
static float ConvertHorizontalAlignToPivot(HorizontalAlign align)
{
	switch (align)
	{
	case HorizontalAlign::Left:        return 0.0f;
	case HorizontalAlign::Center:      return 0.5f;
	case HorizontalAlign::Right:       return 1.0f;
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

	VerticalArrayLayoutNode::VerticalArrayLayoutNode(UI::Node& parent, float distance, HorizontalAlign horizontalAlign)
		: layoutDirty(false), horizontalAlign(HorizontalAlign::Center), distance(5)
	{
		SetParent(&parent);
		surroundingNodeTreeChangedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::TransformUpdated>(*this);

		SetNodesHorizontalAlign(horizontalAlign);
		SetNodesDistance(distance);
	}
	VerticalArrayLayoutNode::~VerticalArrayLayoutNode()
	{
		surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::TransformUpdated>(*this);
	}
	void VerticalArrayLayoutNode::SetNodesHorizontalAlign(HorizontalAlign newHorizontalAlign)
	{
		if (horizontalAlign != newHorizontalAlign)
		{
			horizontalAlign = newHorizontalAlign;
			MarkLayoutDirty();
		}
	}
	void VerticalArrayLayoutNode::SetNodesDistance(float newDistance)
	{
		if (distance != newDistance)
		{
			distance = newDistance;
			MarkLayoutDirty();
		}
	}
	HorizontalAlign VerticalArrayLayoutNode::GetNodesHorizontalAlign() const
	{
		return horizontalAlign;
	}
	float VerticalArrayLayoutNode::GetNodesDistance() const
	{
		return distance;
	}
	void VerticalArrayLayoutNode::RecalculateLayout()
	{
		layoutDirty = false;

		nodesData.Resize(GetChildren().Count());

		float offset = 0;
		uintMem i = 0;
		for (auto& child : GetChildren())
		{
			if (child.IsEnabled())
			{
				nodesData[i].offset = offset;
				offset += child.GetTransform().size.y + distance;
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
			event.node.transformUpdatedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::ChildTransformUpdated>(*this);
			event.node.enabledStateChangedEventDispatcher.AddHandler<&VerticalArrayLayoutNode::ChildEnabledStateUpdated>(*this);
			layoutDirty = true;
		}
		else if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildRemoved)
		{
			event.node.transformUpdatedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::ChildTransformUpdated>(*this);
			event.node.enabledStateChangedEventDispatcher.RemoveHandler<&VerticalArrayLayoutNode::ChildEnabledStateUpdated>(*this);
			layoutDirty = true;
		}
	}
	void VerticalArrayLayoutNode::TransformUpdated(const TransformUpdatedEvent& event)
	{
		MarkLayoutDirty();
	}
	void VerticalArrayLayoutNode::ChildTransformUpdated(const UI::Node::TransformUpdatedEvent& event)
	{
		uintMem i = 0;
		for (auto& child : GetChildren())
			if (&child == &event.node)
				break;
			else
				++i;

		if (layoutDirty)
			RecalculateLayout();

		float horizontalPivot = ConvertHorizontalAlignToPivot(horizontalAlign);
		auto nodeTransform = event.node.GetTransform();

		nodeTransform.pos = Vec2f(0, nodesData[i].offset);
		nodeTransform.parentPivot = Vec2f(horizontalPivot, 0);
		nodeTransform.pivot = Vec2f(horizontalPivot, 0);
		nodeTransform.scale = 1.0f;
		nodeTransform.rotation = 0.0f;

		event.node.SetTransform(nodeTransform);
	}
	void VerticalArrayLayoutNode::ChildEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event)
	{
		MarkLayoutDirty();
	}

	ParentTransformBinding::ParentTransformBinding()
		: child(nullptr)
	{
	}
	ParentTransformBinding::ParentTransformBinding(UI::Node* child)
		: ParentTransformBinding()
	{
 		SetChild(child);
	}
	ParentTransformBinding::~ParentTransformBinding()
	{
		if (child != nullptr)
		{
			child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildSurroungingTreeChanged>(*this);
			child->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildTransformChanged>(*this);
			if (auto parent = child->GetParent())
				parent->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
		}
	}
	void ParentTransformBinding::SetChild(UI::Node* newChild)
	{
		if (child == newChild)
			return;

		if (child != nullptr)
		{
			child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildSurroungingTreeChanged>(*this);
			child->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ChildTransformChanged>(*this);
			if (auto parent = child->GetParent())
				parent->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
		}

		child = newChild;

		if (child != nullptr)
		{
			child->surroundingNodeTreeChangedEventDispatcher.AddHandler<&ParentTransformBinding::ChildSurroungingTreeChanged>(*this);
			child->transformUpdatedEventDispatcher.AddHandler<&ParentTransformBinding::ChildTransformChanged>(*this);
			if (auto parent = child->GetParent())
				parent->transformUpdatedEventDispatcher.AddHandler<&ParentTransformBinding::ParentTransformChanged>(*this);

			child->MarkTransformDirty();
		}
	}
	void ParentTransformBinding::ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
		{
			if (auto parent = event.otherNode)
				parent->transformUpdatedEventDispatcher.RemoveHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
			if (auto parent = child->GetParent())
				parent->transformUpdatedEventDispatcher.AddHandler<&ParentTransformBinding::ParentTransformChanged>(*this);
		}
	}
	void ParentTransformBinding::ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event)
	{
		ApplyTransformToChild();
	}
	void ParentTransformBinding::ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event)
	{
		if (child->GetParent() != nullptr)
			ApplyTransformToChild();
	}

	ParentWidthBinding::ParentWidthBinding(UI::Node* child)
		: ParentTransformBinding(child)
	{
	}
	void ParentWidthBinding::ApplyTransformToChild()
	{
		auto childTransform = GetChild()->GetTransform();
		childTransform.size.x = GetChild()->GetParent()->GetTransform().size.x;
		GetChild()->SetTransform(childTransform);
	}

	ParentHeightBinding::ParentHeightBinding(UI::Node* child)
		: ParentTransformBinding(child)
	{
	}
	void ParentHeightBinding::ApplyTransformToChild()
	{
		auto childTransform = GetChild()->GetTransform();
		childTransform.size.x = GetChild()->GetParent()->GetTransform().size.x;
		GetChild()->SetTransform(childTransform);
	}

	ParentSizeBinding::ParentSizeBinding(UI::Node* child)
		: ParentTransformBinding(child)
	{
	}
	void ParentSizeBinding::ApplyTransformToChild()
	{
		auto childTransform = GetChild()->GetTransform();
		childTransform.size = GetChild()->GetParent()->GetTransform().size;
		GetChild()->SetTransform(childTransform);
	}


	DivisionNode::DivisionNode()
		: transformsDirty(true)
	{
		surroundingNodeTreeChangedEventDispatcher.AddHandler<&DivisionNode::SurroungingTreeChanged>(*this);
		transformUpdatedEventDispatcher.AddHandler<&DivisionNode::TransformChanged>(*this);
	}
	DivisionNode::DivisionNode(Node& parent)
		: DivisionNode()
	{
		SetParent(&parent);
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
			event.node.transformUpdatedEventDispatcher.AddHandler<&DivisionNode::ChildTransformChanged>(*this);
			event.node.MarkTransformDirty();
		}
		else if (event.type == SurroundingNodeTreeChangedEvent::Type::ChildRemoved)
		{
			event.node.transformUpdatedEventDispatcher.RemoveHandler<&DivisionNode::ChildTransformChanged>(*this);
		}
	}
	void DivisionNode::TransformChanged(const TransformUpdatedEvent& event)
	{
		uintMem i = 0;
		for (auto& child : GetChildren())
			child.MarkTransformDirty();

		transformsDirty = true;
	}
	void DivisionNode::ChildTransformChanged(const TransformUpdatedEvent& event)
	{
		if (transformsDirty)
		{
			transforms = CalculateChildTransforms();
			transformsDirty = false;
		}

		uintMem i = 0;
		for (auto& child : GetChildren())
			if (&child == &event.node)
				break;
			else
				++i;

		if (i >= transforms.Count())
			return;

		event.node.SetTransform(transforms[i]);
	}

	HorizontalDivisonNode::HorizontalDivisonNode(float firstNodeHeight)
		: firstNodeHeight(firstNodeHeight)
	{
	}
	HorizontalDivisonNode::HorizontalDivisonNode(Node& parent, float firstNodeHeight)
		: DivisionNode(parent), firstNodeHeight(firstNodeHeight)
	{
	}
	Array<NodeTransform> HorizontalDivisonNode::CalculateChildTransforms()
	{
		auto transform = GetTransform();

		float secondNodeHeight = transform.size.y - firstNodeHeight;

		Array<NodeTransform> transforms{ 2 };
		transforms[0] = NodeTransform{
			.pos = Vec2f(0.0f, secondNodeHeight),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(transform.size.x, firstNodeHeight),
			.scale = 1.0f,
			.rotation = 0.0f
		};
		transforms[1] = NodeTransform{
			.pos = Vec2f(0.0f, 0.0f),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(transform.size.x, secondNodeHeight),
			.scale = 1.0f,
			.rotation = 0.0f
		};

		return transforms;
	}
}