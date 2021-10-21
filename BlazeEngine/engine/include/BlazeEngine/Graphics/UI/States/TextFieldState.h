#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/UI/States/State.h"
#include "BlazeEngine/DataStructures/String.h"

namespace Blaze::UI
{
	class Element;
	class BLAZE_API TextFieldState : public State
	{
	public:
		Element* parent;
		bool hovered;
		bool down;		
		String string;
		std::function<void()> StateChangedEvent;
		std::function<void()> StringChangedEvent;
		std::function<void()> StringFinishedEvent;		

		TextFieldState();

		friend class TextFieldStateManager;
	};
}