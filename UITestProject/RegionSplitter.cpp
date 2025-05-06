#include "pch.h"
#include "RegionSplitter.h"

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

/*
NodeSizeConstraints::NodeSizeConstraints() : 
	maxSize(Measure(FLT_MAX, MeasureType::Pixel), Measure(FLT_MAX, MeasureType::Pixel)), 
	minSize(Measure(0, MeasureType::Pixel), Measure(0, MeasureType::Pixel)),
	aspectRatio(0)
{
}

NodeSizeConstraints::NodeSizeConstraints(const NodeSizeConstraints& other) :
	maxSize(other.maxSize), minSize(other.minSize), aspectRatio(other.aspectRatio)
{
}

void NodeSizeConstraints::SetWidthLowerBound(Measure minWidth)
{
	minSize.x = minWidth;

	if (aspectRatio != 0 && maxSize.x.value == FLT_MAX && minSize.x.value == 0)
	{
		minSize.y.value = 0;
		maxSize.y.value = FLT_MAX;
	}
}

void NodeSizeConstraints::SetWidthUpperBound(Measure maxWidth)
{
	maxSize.x = maxWidth;

	if (aspectRatio != 0 && maxSize.x.value == FLT_MAX && minSize.x.value == 0)
	{
		minSize.y.value = 0;
		maxSize.y.value = FLT_MAX;
	}
}

void NodeSizeConstraints::SetHeightLowerBound(Measure minHeight)
{
	minSize.y = minHeight;

	if (aspectRatio != 0 && maxSize.y.value == FLT_MAX && minSize.y.value == 0)
	{
		minSize.x.value = 0;
		maxSize.x.value = FLT_MAX;
	}
}

void NodeSizeConstraints::SetHeightUpperBound(Measure maxHeight)
{
	maxSize.y = maxHeight;

	if (aspectRatio != 0 && maxSize.y.value == FLT_MAX && minSize.y.value == 0)
	{
		minSize.x.value = 0;
		maxSize.x.value = FLT_MAX;
	}
}
void NodeSizeConstraints::SetSizeUpperBound(Vec2M maxSize)
{
	this->maxSize = maxSize;
	aspectRatio = 0;
}
void NodeSizeConstraints::SetSizeLowerBound(Vec2M minSize)
{
	this->minSize = minSize;
	aspectRatio = 0;
}
void NodeSizeConstraints::SetAspectRatioConstraint(float aspectRatio, bool respectHeightConstraint)
{
	if (respectHeightConstraint)
	{
		this->minSize.x.value = 0;
		this->maxSize.x.value = FLT_MAX;
	}
	else
	{
		this->minSize.y.value = 0;
		this->maxSize.y.value = FLT_MAX;
	}
	this->aspectRatio = aspectRatio;
}
void NodeSizeConstraints::UnsetWidthLowerBound()
{
	minSize.x = Measure(0, MeasureType::Pixel);
}
void NodeSizeConstraints::UnsetWidthUpperBound()
{
	maxSize.x = Measure(FLT_MAX, MeasureType::Pixel);
}
void NodeSizeConstraints::UnsetHeightLowerBound()
{
	minSize.y = Measure(0, MeasureType::Pixel);
}
void NodeSizeConstraints::UnsetHeightUpperBound()
{
	maxSize.y = Measure(FLT_MAX, MeasureType::Pixel);
}
void NodeSizeConstraints::UnsetSizeUpperBound()
{	
	maxSize = Vec2M(Measure(FLT_MAX, MeasureType::Pixel), Measure(FLT_MAX, MeasureType::Pixel));
}
void NodeSizeConstraints::UnsetSizeLowerBound()
{	
	minSize = Vec2M(0pixel, 0pixel);
}
void NodeSizeConstraints::UnsetAspectRatioConstraint()
{
	aspectRatio = 0;
}
Measure NodeSizeConstraints::GetWidthLowerBound() const
{
	return minSize.x;
}
Measure NodeSizeConstraints::GetWidthUpperBound() const
{
	return maxSize.x;
}
Measure NodeSizeConstraints::GetHeightLowerBound() const
{
	return minSize.y;
}
Measure NodeSizeConstraints::GetHeightUpperBound() const
{
	return minSize.x;
}
Vec2M NodeSizeConstraints::GetSizeUpperBound() const
{
	return maxSize;
}
Vec2M NodeSizeConstraints::GetSizeLowerBound() const
{
	return minSize;
}
float NodeSizeConstraints::GetAspectRatioConstraint() const
{
	return aspectRatio;
}
bool NodeSizeConstraints::IsWidthLowerBoundSet() const
{
	return minSize.x.value != 0;
}
bool NodeSizeConstraints::IsWidthUpperBoundSet() const
{
	return maxSize.x.value != FLT_MAX;
}
bool NodeSizeConstraints::IsHeightLowerBoundSet() const
{
	return minSize.y.value != 0;
}
bool NodeSizeConstraints::IsHeightUpperBoundSet() const
{
	return maxSize.y.value != FLT_MAX;
}
bool NodeSizeConstraints::IsAspectRatioConstraintSet() const
{
	return aspectRatio != 0;
}

bool NodeSizeConstraints::operator==(const NodeSizeConstraints& other) const
{
	return minSize == other.minSize && maxSize == other.maxSize && aspectRatio == other.aspectRatio;
}
bool NodeSizeConstraints::operator!=(const NodeSizeConstraints& other) const
{
	return minSize != other.minSize || maxSize != other.maxSize || aspectRatio != other.aspectRatio;
}
NodeSizeConstraints& NodeSizeConstraints::operator=(const NodeSizeConstraints& other)
{
	minSize = other.minSize;
	maxSize = other.maxSize;
	aspectRatio = other.aspectRatio;
	return *this;
}

ContainerNode::ContainerNode()
{
}

ContainerNode::~ContainerNode()
{
}

void ContainerNode::SetSizeConstraints(const NodeSizeConstraints& sizeConstraints)
{
	if (this->sizeConstraints != sizeConstraints)
	{
		MarkTransformDirty();
		this->sizeConstraints = sizeConstraints;
	}
}

const NodeSizeConstraints& ContainerNode::GetSizeConstraints()
{
	return sizeConstraints;
}

void ContainerNode::SetSpacing(const Spacing& spacing)
{
	if (this->spacing != spacing)
	{
		MarkTransformDirty();
		this->spacing = spacing;
	}
}

const Spacing& ContainerNode::GetSpacing() const
{
	return spacing;
}

Spacing::Spacing()
{
}

Spacing::Spacing(const Spacing& other)
	: top(other.top), bottom(other.bottom), left(other.left), right(other.right)
{
}
bool Spacing::operator==(const Spacing& other) const
{
	return top == other.top && bottom == other.bottom && left == other.left && right == other.right;
}
bool Spacing::operator!=(const Spacing& other) const
{
	return top != other.top || bottom != other.bottom || left != other.left || right != other.right;
}
Spacing& Spacing::operator=(const Spacing& other)
{
	top = other.top;
	bottom = other.bottom;
	right = other.right;
	left = other.left;

	return *this;
}
*/
/*
void HorizontalLayoutNode::UpdateContainerNodes()
{
	auto finalTransform = GetFinalTransform();

	MeasureContext measureContext{
		.referenceSize = finalTransform.size,
	};
	
	const PixelSpacing parentSpacing = layout.parentSpacing.ToPixelSpacing(measureContext);	
	const PixelSpacing childrenSpacing = layout.childrenSpacing.ToPixelSpacing(measureContext);
	const PixelSpacing maxSpacing = MaxSpacing(parentSpacing, childrenSpacing);
	const Vec2f alignPivot = ConvertAlignToPivot(layout.childrenAlign);
	
	Vec2f growWeightSum;
	Vec2f shrinkWeightSum;

	float firstLeftSpacing = 0.0f;
	float lastRightSpacing = 0.0f;
	float inbetweenSpacing = 0.0f;

	for (uintMem i = 0; i < containerNodes.Count(); ++i)
	{
		auto& child = containerNodes[i];

		const bool isFirst = i == 0;
		const bool isLast = i == containerNodes.Count() - 1;
		
		float specificLeftSpacing = isFirst ? maxSpacing.left : childrenSpacing.left;
		float specificRightSpacing = isLast ? maxSpacing.right : childrenSpacing.right;		

		if (ContainerNode* nodeContainer = dynamic_cast<ContainerNode*>(&child))
		{
			auto& sizeConstraints = nodeContainer->GetSizeConstraints();
			growWeightSum += sizeConstraints.GetGrowWeight();
			shrinkWeightSum += sizeConstraints.GetShrinkWeight();

			auto& nodeSpacing = nodeContainer->GetSpacing();
			specificLeftSpacing = std::max(specificLeftSpacing, nodeSpacing.left.ConvertToPixel(measureContext));
			specificRightSpacing = std::max(specificRightSpacing, nodeSpacing.right.ConvertToPixel(measureContext));
		}
		else
		{
			growWeightSum += Vec2f(1.0f);
			shrinkWeightSum += Vec2f(1.0f);
		}

		lastRightSpacing = specificRightSpacing;

		if (isFirst) 
			firstLeftSpacing = specificLeftSpacing;

		if (!isLast)
			inbetweenSpacing += std::max(lastRightSpacing, specificLeftSpacing);
	}	

	float availableParentWidth = finalTransform.size.x - firstLeftSpacing - inbetweenSpacing - lastRightSpacing;	

	if (availableParentWidth < 0.0f)
	{
		//TODO
	}
			
	float widthSum = 0.0f;
	float minWidthSum = 0.0f;
	float heightMax = 0.0f;
	
	for (uintMem i = 0; i < containerNodes.Count(); ++i)
	{
		auto& child = containerNodes[i];
		auto childFinalTransform = child.GetFinalTransform();				

		Vec2f size = childFinalTransform.size;
		Vec2f minSize = size;
				
		if (ContainerNode* nodeContainer = dynamic_cast<ContainerNode*>(&child))
		{				
			auto& nodeSpacing = nodeContainer->GetSpacing();
			float specificTopSpacing = std::max(maxSpacing.top, nodeSpacing.top.ConvertToPixel(measureContext));
			float specificBottomSpacing = std::max(maxSpacing.bottom, nodeSpacing.bottom.ConvertToPixel(measureContext));

			Vec2f availableParentSize {
				availableParentWidth,
				finalTransform.size.y - specificTopSpacing - specificBottomSpacing
			};

			auto& sizeConstraints = nodeContainer->GetSizeConstraints();
			size = sizeConstraints.ConstrainSize(availableParentSize, measureContext, shrinkWeightSum, growWeightSum);
			minSize = sizeConstraints.GetSizeLowerBound().ConvertToPixel(measureContext);
		}

		minWidthSum += size.x;
		widthSum += size.x;
		heightMax = std::max(heightMax, size.y);
	}

	if (widthSum < availableParentWidth)
	{
		if (minWidthSum > availableParentWidth)
		{
			//Unable to shrink enough
			//Just place them as if they were shrunk
		}		
	}
	else
	{
		//Grow if needed
		for (uintMem i = 0; i < containerNodes.Count(); ++i)
		{
			auto& child = containerNodes[i];
			auto childFinalTransform = child.GetFinalTransform();

			Vec2f size = childFinalTransform.size;

			if (ContainerNode* nodeContainer = dynamic_cast<ContainerNode*>(&child))
		}
	}

	widthSum += lastRightSpacing;

	if (widthSum < finalTransform.size.x)
	{
		float availableWidth = widthSum - firstLeftSpacing - lastRightSpacing;
		float offset = 0.0f;

		switch (layout.childrenAlign)
		{
		case Align::BottomLeft:
		case Align::Left:
		case Align::TopLeft:
			offset = firstLeftSpacing;
			break;
		case Align::Bottom:
		case Align::Center:
		case Align::Top:
			offset = finalTransform.size.x / 2 - availableWidth / 2;
			break;
		case Align::BottomRight:
		case Align::Right:
		case Align::TopRight:
			offset = finalTransform.size.x - availableWidth - lastRightSpacing;
			break;
		default:
			break;
		}

		for (uintMem i = 0; i < containerNodes.Count(); ++i)
		{
			auto& child = containerNodes[i];
			auto childFinalTransform = child.GetFinalTransform();

			Vec2f size = childFinalTransform.size;

			const bool isFirst = i == 0;
			const bool isLast = i == containerNodes.Count() - 1;

			PixelSpacing specificSpacing{
				.top = maxSpacing.top,
				.bottom = maxSpacing.bottom,
				.left = isFirst ? maxSpacing.left : childrenSpacing.left,
				.right = isLast ? maxSpacing.right : childrenSpacing.right,
			};

			float growWeight = 1.0f;

			if (ContainerNode* nodeContainer = dynamic_cast<ContainerNode*>(&child))							
				growWeight = nodeContainer->GetGrowWeight();			
			
			availableWidth * growWeight / growWeightSum;

			if (isFirst) firstLeftSpacing = specificSpacing.left;
			if (isLast) lastRightSpacing = specificSpacing.right;
		}
	}
}
*/

HorizontalWeightedFillLayout::HorizontalWeightedFillLayout()
	: layoutDirty(false), verticalAlign(VerticalAlign::Center)
{	
}
HorizontalWeightedFillLayout::HorizontalWeightedFillLayout(HorizontalWeightedFillLayout&& other)
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
	if (!nodesData.Empty())
	{
		nodesData.First().node.surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::FirstNodeSurroundingNodeTreeChangedEvent>(*this);

		if (auto parent = nodesData.First().node.GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::ParentTransformUpdatedEvent>(*this);
	}

	for (auto& nodeData : nodesData)
	{
		nodeData.node.transformUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::NodeTransformUpdatedEvent>(*this);
		nodeData.node.enabledStateUpdatedEventDispatcher.RemoveHandler<&HorizontalWeightedFillLayout::NodeEnabledStateUpdated>(*this);
	}
}
void HorizontalWeightedFillLayout::AddAllHandlers()
{
	for (auto& nodeData : nodesData)
	{
		nodeData.node.transformUpdatedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::NodeTransformUpdatedEvent>(*this);
		nodeData.node.enabledStateUpdatedEventDispatcher.AddHandler<&HorizontalWeightedFillLayout::NodeEnabledStateUpdated>(*this);
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
		nodeData.node.enabledStateUpdatedEventDispatcher.SwapHandlers<&HorizontalWeightedFillLayout::NodeEnabledStateUpdated>(other, *this);
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
		BLAZE_ENGINE_GRAPHICS_ERROR("Internal error. Gotten an event for a node that is not a child to the current node.");
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
void HorizontalWeightedFillLayout::NodeEnabledStateUpdated(const UI::Node::EnabledStateUpdatedEvent& event)
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

ChildWidthBinding::ChildWidthBinding()
	: child(nullptr)
{
}
ChildWidthBinding::ChildWidthBinding(UI::Node* child)
	: child(nullptr)
{
	SetChild(child);
}
ChildWidthBinding::~ChildWidthBinding()
{
	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildWidthBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildWidthBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildWidthBinding::ParentTransformChanged>(*this);
	}
}
void ChildWidthBinding::SetChild(UI::Node* newChild)
{
	if (child == newChild)
		return;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildWidthBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildWidthBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildWidthBinding::ParentTransformChanged>(*this);
	}

	child = newChild;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.AddHandler<&ChildWidthBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.AddHandler<&ChildWidthBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildWidthBinding::ParentTransformChanged>(*this);
	}
}
void ChildWidthBinding::ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event)
{
	if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
	{
		if (auto parent = event.otherNode)
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildWidthBinding::ParentTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildWidthBinding::ParentTransformChanged>(*this);
	}
}
void ChildWidthBinding::ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parentTransform = event.node.GetTransform();
	auto childTransform = child->GetTransform();
	childTransform.size.x = parentTransform.size.x * parentTransform.scale / childTransform.scale;
	child->SetTransform(childTransform);
}
void ChildWidthBinding::ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parent = child->GetParent();

	if (parent == nullptr)
		return;

	if (parent->IsTransformDirty())
		parent->CleanTransform();
	else
	{
		auto parentTransform = parent->GetTransform();
		auto childTransform = child->GetTransform();
		childTransform.size.x = parentTransform.size.x * parentTransform.scale / childTransform.scale;
		child->SetTransform(childTransform);
	}
}

ChildHeightBinding::ChildHeightBinding()
	: child(nullptr)
{
}
ChildHeightBinding::ChildHeightBinding(UI::Node* child)
	: child(nullptr)
{
	SetChild(child);
}
ChildHeightBinding::~ChildHeightBinding()
{
	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildHeightBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildHeightBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildHeightBinding::ParentTransformChanged>(*this);
	}
}
void ChildHeightBinding::SetChild(UI::Node* newChild)
{
	if (child == newChild)
		return;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildHeightBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildHeightBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildHeightBinding::ParentTransformChanged>(*this);
	}

	child = newChild;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.AddHandler<&ChildHeightBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.AddHandler<&ChildHeightBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildHeightBinding::ParentTransformChanged>(*this);
	}
}
void ChildHeightBinding::ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event)
{
	if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
	{
		if (auto parent = event.otherNode)
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildHeightBinding::ParentTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildHeightBinding::ParentTransformChanged>(*this);
	}
}
void ChildHeightBinding::ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parentTransform = event.node.GetTransform();
	auto childTransform = child->GetTransform();
	childTransform.size.y = parentTransform.size.y * parentTransform.scale / childTransform.scale;
	child->SetTransform(childTransform);
}
void ChildHeightBinding::ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parent = child->GetParent();

	if (parent == nullptr)
		return;

	if (parent->IsTransformDirty())
		parent->CleanTransform();
	else
	{
		auto parentTransform = parent->GetTransform();
		auto childTransform = child->GetTransform();
		childTransform.size.y = parentTransform.size.y * parentTransform.scale / childTransform.scale;
		child->SetTransform(childTransform);
	}
}

ChildSizeBinding::ChildSizeBinding()
	: child(nullptr)
{
}
ChildSizeBinding::ChildSizeBinding(UI::Node* child)
	: child(nullptr)
{
	SetChild(child);
}
ChildSizeBinding::~ChildSizeBinding()
{
	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildSizeBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildSizeBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildSizeBinding::ParentTransformChanged>(*this);
	}
}
void ChildSizeBinding::SetChild(UI::Node* newChild)
{
	if (child == newChild)
		return;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&ChildSizeBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.RemoveHandler<&ChildSizeBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildSizeBinding::ParentTransformChanged>(*this);
	}

	child = newChild;

	if (child != nullptr)
	{
		child->surroundingNodeTreeChangedEventDispatcher.AddHandler<&ChildSizeBinding::ChildSurroungingTreeChanged>(*this);
		child->transformUpdatedEventDispatcher.AddHandler<&ChildSizeBinding::ChildTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildSizeBinding::ParentTransformChanged>(*this);
	}
}
void ChildSizeBinding::Apply(UI::Node& child)
{
	child.dataMap.map.Insert<ChildSizeBinding>("ChildSizeBinding", &child);
}
void ChildSizeBinding::ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event)
{
	if (event.type == UI::Node::SurroundingNodeTreeChangedEvent::Type::ParentChanged)
	{
		if (auto parent = event.otherNode)
			parent->transformUpdatedEventDispatcher.RemoveHandler<&ChildSizeBinding::ParentTransformChanged>(*this);
		if (auto parent = child->GetParent())
			parent->transformUpdatedEventDispatcher.AddHandler<&ChildSizeBinding::ParentTransformChanged>(*this);
	}
}
void ChildSizeBinding::ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parentTransform = event.node.GetTransform();
	auto childTransform = child->GetTransform();
	childTransform.size = parentTransform.size * parentTransform.scale / childTransform.scale;
	child->SetTransform(childTransform);
}
void ChildSizeBinding::ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event)
{
	auto parent = child->GetParent();

	if (parent == nullptr)
		return;

	if (parent->IsTransformDirty())
		parent->CleanTransform();
	else
	{
		auto parentTransform = parent->GetTransform();
		auto childTransform = child->GetTransform();
		childTransform.size = parentTransform.size * parentTransform.scale / childTransform.scale;
		child->SetTransform(childTransform);
	}
}
