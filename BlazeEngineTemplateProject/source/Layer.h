#pragma once

class Layer : public AppLayer
{
public:
	void Update() override;
	void Render() override;

	bool OnEvent(const Input::GenericInputEvent& event, bool isProcessed) override;
};