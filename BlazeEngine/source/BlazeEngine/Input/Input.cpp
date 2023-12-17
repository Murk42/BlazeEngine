#include "pch.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Input/ClientEventStack.h"

namespace Blaze
{
	namespace Input
	{							
		EventStack<2048> clientEventStack;

		InputEventSystem inputEventSystem;		
		
		InputEventSystem& GetInputEventSystem()
		{
			return inputEventSystem;
		}

		void Update()
		{			
			clientEventStack.ProcessAndClear();
		}		
	}
}