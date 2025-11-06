#pragma once
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API Slider : public ButtonBase, public RenderNode
	{
	public:
		struct Style
		{
			ColorRGBAf highlightTint = { 0.0f, 0.0f, 0.0f, 0.4f };
			ColorRGBAf pressedTint = { 0.0f, 0.0f, 0.0f, 0.6f };

			ColorRGBAf handleFillColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			ColorRGBAf handleBorderColor = { 0.9f, 0.9f, 0.9f, 1.0f };
			float handleCornerRadius = 0;
			float handleBorderWidth = 1;

			ColorRGBAf trackFillColor = { 0.0f, 0.0f, 0.0f, 0.2f };
			ColorRGBAf trackBorderColor = { 0.6f, 0.6f, 0.6f, 1.0f };
			float trackCornerRadius = 0;
			float trackBorderWidth = 1;
		};
		struct ValueChangedEvent
		{
			Slider& slider;
		};
		EventDispatcher<ValueChangedEvent> valueChangedEventDispatcher;

		Slider();
		~Slider();

		int HitTest(Vec2f screenPosition) override;

		void SetStyle(const Style& style);
		void SetStepCount(uint32 stepCount);
		void SetValue(float value);
		void SetHandleSize(Vec2f handleSize);
		void SetTrackThickness(float thickness);

		inline float GetStepCount() const { return value; }
		inline float GetValue() const { return value; }

		void PreRender(const UIRenderContext& renderContext) override;
		UIRenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		PanelRenderUnit handleRenderUnit;
		PanelRenderUnit trackRenderUnit;
		bool renderUnitDirty;

		ColorRGBAf highlightTint;
		ColorRGBAf pressedTint;
		ColorRGBAf handleFillColor;
		ColorRGBAf handleBorderColor;

		bool highlighted;

		uint32 stepCount;
		float value;

		Vec2f handleSize;
		float trackThickness;

		float pressedOffset;

		void UpdateColor();

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		void MouseHitStatusChangedEvent(const MouseHitStatusChangedEvent& event);
		void MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event);
		void PressedStateChangedEvent(const PressedStateChangedEvent& event);
		void MouseMotionEvent(const MouseMotionEvent& event);
	};
}