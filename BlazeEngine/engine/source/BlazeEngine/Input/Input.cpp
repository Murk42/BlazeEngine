#include "BlazeEngine/Input/Input.h"
#include "source/BlazeEngine/Input/ClientEventStack.h"

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