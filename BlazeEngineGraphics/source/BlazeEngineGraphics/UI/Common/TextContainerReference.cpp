#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/TextContainerReference.h"

namespace Blaze::UI
{
	TextContainerReference::TextContainerReference(TextContainerBase& container)
		: container(&container)
	{
		container.textUpdatedEventDispatcher.AddHandler<&TextContainerReference::TextUpdatedEvent>(*this);
	}
	TextContainerReference::~TextContainerReference()
	{
		container->textUpdatedEventDispatcher.RemoveHandler<&TextContainerReference::TextUpdatedEvent>(*this);
	}
	void TextContainerReference::SetContainer(TextContainerBase& newContainer)
	{
		if (container == &newContainer)
			return;

		container->textUpdatedEventDispatcher.RemoveHandler<&TextContainerReference::TextUpdatedEvent>(*this);

		auto oldContainer = container;
		container = nullptr;

		if (oldContainer->CharacterCount() != 0)
			textUpdatedEventDispatcher.Call({ .type = TextUpdatedEvent::Type::Erased, .begin = 0, .count = oldContainer->CharacterCount() });

		container = &newContainer;

		textUpdatedEventDispatcher.Call({ .type = TextUpdatedEvent::Type::Inserted, .begin = 0, .count = container->CharacterCount() });

		container->textUpdatedEventDispatcher.AddHandler<&TextContainerReference::TextUpdatedEvent>(*this);
	}
	void TextContainerReference::TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event)
	{
		textUpdatedEventDispatcher.Call(event);
	}
}