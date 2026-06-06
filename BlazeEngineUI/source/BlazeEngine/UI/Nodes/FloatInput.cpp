#include "pch.h"
#include "BlazeEngine/UI/Nodes/FloatInput.h"

namespace Blaze::UI::Nodes
{
	FloatInput::FloatInput()
		: TextInputBase(), value(0.0f)
	{
	}
	FloatInput::FloatInput(Node& parent, const UI::NodeTransform& transform)
		: TextInputBase(parent, transform), value(0.0f)
	{
		SetInputedText(u8String::Parse(value, FloatStringParseFormat::Fixed));
	}
	FloatInput::FloatInput(Node& parent, const UI::NodeTransform& transform, const TextStyle& textStyle, float value)
		: TextInputBase(parent, transform, textStyle), value(value)
	{
		SetInputedText(u8String::Parse(value, FloatStringParseFormat::Fixed));
	}
	void FloatInput::SetValueEnteredCallback(std::function<void(float)> callback)
	{
		this->callback = std::move(callback);
	}
	void FloatInput::TextEntered()
	{
		u8String inputedText = GetInputedText();
		u8StringView remainingString;

		float newValue = 0.0f;
		if (!inputedText.ConvertToDecimal(newValue, FloatStringConvertFormat::General, &remainingString) || !remainingString.Empty())
		{
			SetInputedText(u8String::Parse(value, FloatStringParseFormat::Fixed));
			return;
		}
		 
		if (newValue == value)
			return;

		value = newValue;

		SetInputedText(u8String::Parse(value, FloatStringParseFormat::Fixed));

		if (callback)
			callback(value);
	}
}