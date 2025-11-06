#pragma once
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI::Layout
{
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
		return Measure(static_cast<float>(value), MeasureType::Percent);
	}
	constexpr Measure operator""_pc(unsigned long long value)
	{
		return Measure(static_cast<float>(value), MeasureType::Percent);
	}
	constexpr Measure operator""_px(long double value)
	{
		return Measure(static_cast<float>(value), MeasureType::Pixel);
	}
	constexpr Measure operator""_px(unsigned long long value)
	{
		return Measure(static_cast<float>(value), MeasureType::Pixel);
	}
	constexpr Measure operator""_mm(long double value)
	{
		return Measure(static_cast<float>(value), MeasureType::Millimeters);
	}
	constexpr Measure operator""_mm(unsigned long long value)
	{
		return Measure(static_cast<float>(value), MeasureType::Millimeters);
	}
	constexpr Measure operator""_in(long double value)
	{
		return Measure(static_cast<float>(value), MeasureType::Inch);
	}
	constexpr Measure operator""_in(unsigned long long value)
	{
		return Measure(static_cast<float>(value), MeasureType::Inch);
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
}

namespace Blaze
{
	using Vec2M = Vec2<UI::Layout::Measure>;

	template<>
	struct Vector<UI::Layout::Measure, 2>
	{
		UI::Layout::HorizontalMeasure x;
		UI::Layout::VerticalMeasure y;

		constexpr Vector();
		constexpr Vector(const Vec2<UI::Layout::Measure>& v) : x(v.x), y(v.y) {}
		constexpr Vector(const UI::Layout::HorizontalMeasure& x, const UI::Layout::VerticalMeasure& y) : x(x), y(y) {}

		constexpr Vec2f Convert(UI::Layout::MeasureType newType, const UI::Layout::MeasureContext& context) const { return Vec2f(x.Convert(newType, context), y.Convert(newType, context)); }
		constexpr Vec2f ConvertToPixel(const UI::Layout::MeasureContext& context) const { return Vec2f(x.ConvertToPixel(context), y.ConvertToPixel(context)); }
		constexpr Vec2f ConvertToPercent(const UI::Layout::MeasureContext& context) const { return Vec2f(x.ConvertToPercent(context), y.ConvertToPercent(context)); }

		constexpr static Vec2M FromPixels(Vec2f value) { return Vec2M(UI::Layout::Measure(value.x, UI::Layout::MeasureType::Pixel), UI::Layout::Measure(value.y, UI::Layout::MeasureType::Pixel)); }
		constexpr static Vec2M FromPercent(Vec2f value) { return Vec2M(UI::Layout::Measure(value.x, UI::Layout::MeasureType::Percent), UI::Layout::Measure(value.y, UI::Layout::MeasureType::Percent)); }

		constexpr bool operator== (const  Vec2<UI::Layout::Measure>& v) const { return x == v.x && y == v.y; }
		constexpr bool operator!= (const  Vec2<UI::Layout::Measure>& v) const { return x != v.x || y != v.y; }
	};
}

namespace Blaze::UI::Layout
{
	class HorizontalWeightedFillLayout
	{
	public:
		struct NodeSetupData
		{
			UI::Node& node;
			float weight;
		};

		HorizontalWeightedFillLayout();
		HorizontalWeightedFillLayout(HorizontalWeightedFillLayout&& other) noexcept;
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
		void ChildEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event);
		void FirstNodeSurroundingNodeTreeChangedEvent(const UI::Node::SurroundingNodeTreeChangedEvent& event);
	};

	//HorizontalArrayLayout

	class VerticalArrayLayoutNode : public UI::Nodes::EmptyNode
	{
	public:
		VerticalArrayLayoutNode(UI::Node& parent, float distance = 5.0f, HorizontalAlign horizontalAlign = HorizontalAlign::Center);
		~VerticalArrayLayoutNode();

		void SetNodesHorizontalAlign(HorizontalAlign horizontalAlign);
		void SetNodesDistance(float distance);

		HorizontalAlign GetNodesHorizontalAlign() const;
		float GetNodesDistance() const;
	private:
		struct NodeData
		{
			float offset;
		};

		HorizontalAlign horizontalAlign;
		float distance;
		Array<NodeData> nodesData;
		bool layoutDirty;

		void RecalculateLayout();
		void MarkLayoutDirty();

		void SurroungingTreeChanged(const SurroundingNodeTreeChangedEvent& event);
		void TransformUpdated(const TransformUpdatedEvent& event);
		void ChildTransformUpdated(const TransformUpdatedEvent& event);
		void ChildEnabledStateUpdated(const EnabledStateChangedEvent& event);
	};

	class ParentTransformBinding
	{
	public:
		ParentTransformBinding();
		ParentTransformBinding(UI::Node* child);
		virtual ~ParentTransformBinding();

		void SetChild(UI::Node* child);

		inline UI::Node* GetChild() const { return child; }

	protected:
		virtual void ApplyTransformToChild() = 0;
	private:
		UI::Node* child;

		void ChildSurroungingTreeChanged(const UI::Node::SurroundingNodeTreeChangedEvent& event);
		void ParentTransformChanged(const UI::Node::TransformUpdatedEvent& event);
		void ChildTransformChanged(const UI::Node::TransformUpdatedEvent& event);
	};

	class ParentWidthBinding : public ParentTransformBinding
	{
	public:
		ParentWidthBinding() = default;
		ParentWidthBinding(UI::Node* child);
	private:
		void ApplyTransformToChild() override;
	};

	class ParentHeightBinding : public ParentTransformBinding
	{
	public:
		ParentHeightBinding() = default;
		ParentHeightBinding(UI::Node* child);
	private:
		void ApplyTransformToChild() override;
	};

	class ParentSizeBinding : public ParentTransformBinding
	{
	public:
		ParentSizeBinding() = default;
		ParentSizeBinding(UI::Node* child);
	private:
		void ApplyTransformToChild() override;
	};

	class DivisionNode : public Nodes::EmptyNode
	{
	public:
		DivisionNode();
		DivisionNode(Node& parent);
		virtual ~DivisionNode();
	protected:
		virtual Array<NodeTransform> CalculateChildTransforms() = 0;
	private:
		Array<NodeTransform> transforms;
		bool transformsDirty;

		void SurroungingTreeChanged(const SurroundingNodeTreeChangedEvent& event);
		void TransformChanged(const TransformUpdatedEvent& event);
		void ChildTransformChanged(const TransformUpdatedEvent& event);
	};

	class HorizontalDivisonNode : public DivisionNode
	{
	public:
		HorizontalDivisonNode(float firstNodeHeight);
		HorizontalDivisonNode(Node& parent, float firstNodeHeight);
	private:
		float firstNodeHeight;
		Array<NodeTransform> CalculateChildTransforms() override;
	};
}