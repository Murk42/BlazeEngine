#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"

namespace Blaze
{	
	namespace Time
	{		
		double BLAZE_API GetWorldTime();
		double BLAZE_API GetRunTime();
		double BLAZE_API GetDeltaTime();

		/// 
		/// Should be called every frame.
		/// Updated the value that Time::GetDeltaTime returns.
		/// 
		void BLAZE_API Update();

	}
}