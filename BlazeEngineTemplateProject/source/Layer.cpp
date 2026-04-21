#include "pch.h"
#include "Layer.h"

void Layer::Update()
{
}

void Layer::Render(const Graphics::RenderContext& renderContext)
{
}

Input::EventProcessedState Layer::OnEvent(const Input::GenericInputEvent& event, bool isProcessed)
{
	return Input::EventProcessedState::NotProcessed;
}
