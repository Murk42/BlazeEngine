#pragma once
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Graphics/RenderableNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"
#include <functional>

namespace Blaze::UI::Nodes
{
	struct SliderStyle
	{
		ColorRGBAf highlightTint = { 0.0f, 0.0f, 0.0f, 0.4f };
		ColorRGBAf pressedTint = { 0.0f, 0.0f, 0.0f, 0.6f };

		PanelStyle handleStyle = {
			.fillColor = { 0.0f, 0.0f, 0.0f, 1.0f },
			.borderColor = { 0.9f, 0.9f, 0.9f, 1.0f },
			.borderWidth = 1,
			.cornerRadius = 0,
		};

		PanelStyle trackStyle = {
			.fillColor = { 0.0f, 0.0f, 0.0f, 0.2f },
			.borderColor = { 0.6f, 0.6f, 0.6f, 1.0f },
			.borderWidth = 1,
			.cornerRadius = 0,
		};
	};

	class BLAZE_API Slider : public ButtonBase, public RenderableNode
	{
	public:
		struct ValueChangedEvent
		{
			Slider& slider;
		};
		EventDispatcher<ValueChangedEvent> valueChangedEventDispatcher;

		Slider();
		Slider(Node& parent, const NodeTransform& transform = { }, float value = 0, Vec2f handleSize = { 30, 12 }, float trackThickness = 1.0f, const SliderStyle& style = {});
		~Slider();

		void SetStyle(const SliderStyle& style);
		SliderStyle GetStyle() const;

		void SetValueChangedCallback(std::function<void(float value)> valueChangedCallback);

		void SetValue(float value);
		void SetHandleSize(Vec2f handleSize);
		void SetTrackThickness(float thickness);

		inline float GetValue() const { return value; }

		bool PreRender(const Graphics::RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		PanelRenderUnit handleRenderUnit;
		PanelRenderUnit trackRenderUnit;
		bool renderUnitDirty;

		SliderStyle style;		

		float value;

		Vec2f handleSize;
		float trackThickness;

		float draggingMouseOffset;

		std::function<void(float value)> valueChangedCallback;

		Vec2f CalculateHandlePos() const;
		bool HitTestHandle(Vec2f mouseScreenPos, Vec2f* handleRelativePos = nullptr);

		void UpdateColor();

		void UpdateHandleRenderUnit();
		void UpdateTrackRenderUnit();

		void SetValueForHandleRelativePos(Vec2f handleRelativePos);

		void IsDownChanged(bool byMouse, Vec2f pos) override;
		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		void MouseHitStatusChangedEvent(const UIMouseHitStatusChangedEvent& event);
		void MouseMotionEvent(const UIMouseMotionEvent& event);
	};
}