#pragma once
#include "BlazeEngine/UI/Common/TextInputBase.h"
#include <functional>

namespace Blaze::UI::Nodes
{
	class FloatInput : public TextInputBase
	{
	public:
		FloatInput();
		FloatInput(Node& parent, const UI::NodeTransform& transform);
		FloatInput(Node& parent, const UI::NodeTransform& transform, const TextStyle& textStyle, float value);

		float GetValue() const { return value; }

		void SetValueEnteredCallback(std::function<void(float)> callback);
	private:
		std::function<void(float)> callback;
		float value;

		void TextEntered() override;
	};
}