#pragma once

class Layer : public AppGraphicsLayer
{
public:
	void Update() override;
	void Render(const Graphics::RenderContext& renderContext) override;

	Input::EventProcessedState OnEvent(const Input::GenericInputEvent& event, bool isProcessed) override;
};