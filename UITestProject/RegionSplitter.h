#pragma once

struct MeasureContext
{
	Vec2f referenceSize;	
};

enum class MeasureType
{
	Percent, Pixel, Millimeters, Inch
};
struct Measure
{
	float value;
	MeasureType type;

	constexpr Measure();
	constexpr Measure(float value, MeasureType type);
	constexpr Measure(const Measure& other);

	constexpr bool operator==(const Measure& other) const;
	constexpr bool operator!=(const Measure& other) const;

	constexpr Measure& operator=(const Measure& other);
};

constexpr Measure operator""_pc(long double value)
{
	return Measure(value, MeasureType::Percent);
}
constexpr Measure operator""_pc(unsigned long long value)
{
	return Measure(value, MeasureType::Percent);
}
constexpr Measure operator""_px(long double value)
{
	return Measure(value, MeasureType::Pixel);
}
constexpr Measure operator""_px(unsigned long long value)
{
	return Measure(value, MeasureType::Pixel);
}
constexpr Measure operator""_mm(long double value)
{
	return Measure(value, MeasureType::Millimeters);
}
constexpr Measure operator""_mm(unsigned long long value)
{
	return Measure(value, MeasureType::Millimeters);
}
constexpr Measure operator""_in(long double value)
{
	return Measure(value, MeasureType::Inch);
}
constexpr Measure operator""_in(unsigned long long value)
{
	return Measure(value, MeasureType::Inch);
}

struct HorizontalMeasure : Measure
{
	constexpr HorizontalMeasure();
	constexpr HorizontalMeasure(const HorizontalMeasure&);
	constexpr HorizontalMeasure(const Measure&);

	constexpr float Convert(MeasureType newType, const MeasureContext& context) const;
	constexpr float ConvertToPixel(const MeasureContext& context) const;
	constexpr float ConvertToPercent(const MeasureContext& context) const;
	constexpr float ConvertToMillimeter(const MeasureContext& context) const;
	constexpr float ConvertToInch(const MeasureContext& context) const;

	constexpr HorizontalMeasure& operator=(const Measure& other);
	constexpr HorizontalMeasure& operator=(const HorizontalMeasure& other);
};
struct VerticalMeasure : Measure
{
	constexpr VerticalMeasure();
	constexpr VerticalMeasure(const VerticalMeasure&);
	constexpr VerticalMeasure(const Measure&);

	constexpr float Convert(MeasureType newType, const MeasureContext& context) const;
	constexpr float ConvertToPixel(const MeasureContext& context) const;
	constexpr float ConvertToPercent(const MeasureContext& context) const;
	constexpr float ConvertToMillimeter(const MeasureContext& context) const;
	constexpr float ConvertToInch(const MeasureContext& context) const;

	constexpr VerticalMeasure& operator=(const Measure& other);
	constexpr VerticalMeasure& operator=(const VerticalMeasure& other);
};

using Vec2M = Vec2<Measure>;

template<>
struct Vector<Measure, 2>
{
	HorizontalMeasure x;
	VerticalMeasure y;

	constexpr Vector();
	constexpr Vector(const Vec2<Measure>& v) : x(v.x), y(v.y) { }
	constexpr Vector(const HorizontalMeasure& x, const VerticalMeasure& y) : x(x), y(y) { }

	constexpr Vec2f Convert(MeasureType newType, const MeasureContext& context) const { return Vec2f(x.Convert(newType, context), y.Convert(newType, context)); }
	constexpr Vec2f ConvertToPixel(const MeasureContext& context) const { return Vec2f(x.ConvertToPixel(context), y.ConvertToPixel(context)); }
	constexpr Vec2f ConvertToPercent(const MeasureContext& context) const { return Vec2f(x.ConvertToPercent(context), y.ConvertToPercent(context)); }

	constexpr static Vec2M FromPixels(Vec2f value) { return Vec2M(Measure(value.x, MeasureType::Pixel), Measure(value.y, MeasureType::Pixel)); }
	constexpr static Vec2M FromPercent(Vec2f value) { return Vec2M(Measure(value.x, MeasureType::Percent), Measure(value.y, MeasureType::Percent)); }

	constexpr bool operator== (const  Vec2<Measure>& v) const { return x == v.x && y == v.y; }
	constexpr bool operator!= (const  Vec2<Measure>& v) const { return x != v.x || y != v.y; }
};

/*
struct PixelSpacing
{
	float top;
	float bottom;
	float left;
	float right;
};
struct Spacing
{
	VerticalMeasure top;
	VerticalMeasure bottom;
	HorizontalMeasure left;
	HorizontalMeasure right;

	Spacing();
	Spacing(const Spacing& other);

	PixelSpacing ToPixelSpacing(const MeasureContext& measureContext) const;

	bool operator==(const Spacing& other) const;
	bool operator!=(const Spacing& other) const;

	Spacing& operator=(const Spacing& other);
};

class NodeSizeConstraints
{
public:
	NodeSizeConstraints();
	NodeSizeConstraints(const NodeSizeConstraints& other);

	//minWidth cannot be set to auto, in that case the bound is unset
	void SetWidthLowerBound(Measure minWidth);
	void SetWidthUpperBound(Measure maxWidth);
	void SetHeightLowerBound(Measure minHeight);
	void SetHeightUpperBound(Measure maxHeight);

	void SetSizeUpperBound(Vec2<Measure> maxSize);
	void SetSizeLowerBound(Vec2<Measure> minSize);
		
	void SetAspectRatioConstraint(float aspectRatio, bool respectHeightConstraint);

	void UnsetWidthLowerBound();
	void UnsetWidthUpperBound();
	void UnsetHeightLowerBound();
	void UnsetHeightUpperBound();

	void UnsetSizeUpperBound();
	void UnsetSizeLowerBound();

	void UnsetAspectRatioConstraint();

	Measure GetWidthLowerBound() const;
	Measure GetWidthUpperBound() const;
	Measure GetHeightLowerBound() const;
	Measure GetHeightUpperBound() const;

	Vec2<Measure> GetSizeUpperBound() const;
	Vec2<Measure> GetSizeLowerBound() const;

	Vec2M GetDesiredSize() const;
	Vec2f GetGrowWeight() const;
	Vec2f GetShrinkWeight() const;

	float GetAspectRatioConstraint() const;

	bool IsWidthLowerBoundSet() const;
	bool IsWidthUpperBoundSet() const;
	bool IsHeightLowerBoundSet() const;
	bool IsHeightUpperBoundSet() const;

	bool IsAspectRatioConstraintSet() const;

	bool FitInto(Vec2f bounds, Vec2f& out) const;
	
	Vec2f ConstrainSize(Vec2f bounds, const MeasureContext& measureContext, Vec2f shrinkWeightSum, Vec2f growWeightSum) const;

	bool operator==(const NodeSizeConstraints& other) const;
	bool operator!=(const NodeSizeConstraints& other) const;

	NodeSizeConstraints& operator=(const NodeSizeConstraints&);
private:	
	Vec2<Measure> maxSize, minSize;
	float aspectRatio;	
	Vec2M desiredSize;
	Vec2f growWeight, shrinkWeight;
};

class ContainerNode : public UI::Node
{
public:
	ContainerNode();	
	~ContainerNode();

	void SetSizeConstraints(const NodeSizeConstraints& sizeConstraints);
	void SetSpacing(const Spacing& spacing);
	void SetGrowWeight(float growWeight);
	void SetShrinkWeight(float shrinkWeight);

	inline const NodeSizeConstraints& GetSizeConstraints() { return sizeConstraints; }
	inline const Spacing& GetSpacing() const { return spacing; }	
private:
	NodeSizeConstraints sizeConstraints;
	Spacing spacing;		
};

*/
enum class VerticalAlign
{
	Top, Center, Bottom
};
enum class HorizontalAlign
{
	Left, Center, Right
};

class HorizontalWeightedFillLayout
{
public:	
	struct NodeSetupData
	{
		UI::Node& node;
		float weight;
	};

	HorizontalWeightedFillLayout();
	HorizontalWeightedFillLayout(HorizontalWeightedFillLayout&& other);
	HorizontalWeightedFillLayout(ArrayView<NodeSetupData> nodes, VerticalAlign verticalAlign = VerticalAlign::Center);
	~HorizontalWeightedFillLayout();
	
	void SetNodes(ArrayView<NodeSetupData> nodes);
	void SetLayoutChildrenVerticalAlign(VerticalAlign verticalAlign);		

	uintMem GetNodeCount() const;
	UI::Node& GetNode(uintMem index) const;
	float GetNodeWeight(uintMem index) const;

	HorizontalWeightedFillLayout& operator=(HorizontalWeightedFillLayout&& other) noexcept;

	static void Apply(UI::Node& parent, ArrayView<NodeSetupData> nodes, VerticalAlign verticalAlign = VerticalAlign::Center);
private:	
	struct NodeData
	{
		UI::Node& node;
		float weight;
		float offset;
		float width;
	};
	
	VerticalAlign verticalAlign;	
	Array<NodeData> nodesData;	
	bool layoutDirty;

	void RemoveAllHandlers();
	void AddAllHandlers();
	void SwapHandlers(HorizontalWeightedFillLayout& other);
	
	void RecalculateLayout();
	void MarkLayoutDirty();		

	void ParentTransformUpdatedEvent(const UI::Node::TransformUpdatedEvent& event);
	void NodeTransformUpdatedEvent(const UI::Node::TransformUpdatedEvent& event);
	void NodeEnabledStateUpdated(const UI::Node::EnabledStateUpdatedEvent& event);
	void FirstNodeSurroundingNodeTreeChangedEvent(const UI::Node::SurroundingNodeTreeChangedEvent& event);	
};

class ChildWidthBinding
{
public:
	ChildWidthBinding();
	ChildWidthBinding(UI::Node* child);
	~ChildWidthBinding();

	void SetChild(UI::Node* child);

	inline UI::Node* GetChild() const { return child; }
private:
	UI::Node* child;

	void ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event);
	void ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event);
	void ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event);
};

class ChildHeightBinding
{
public:
	ChildHeightBinding();
	ChildHeightBinding(UI::Node* child);
	~ChildHeightBinding();

	void SetChild(UI::Node* child);

	inline UI::Node* GetChild() const { return child; }
private:
	UI::Node* child;

	void ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event);
	void ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event);
	void ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event);
};

class ChildSizeBinding
{
public:
	ChildSizeBinding();
	ChildSizeBinding(UI::Node* child);
	~ChildSizeBinding();

	void SetChild(UI::Node* child);

	inline UI::Node* GetChild() const { return child; }

	static void Apply(UI::Node& child);
private:
	UI::Node* child;

	void ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event);
	void ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event);
	void ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event);
};