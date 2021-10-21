#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"

namespace Blaze
{
	namespace Hardware
	{
		namespace CPU
		{
			BLAZE_API int NumberOfCores();
		}

		namespace GPU
		{
			BLAZE_API int MaxGeometryShaderUniforms();
		}
	}
}