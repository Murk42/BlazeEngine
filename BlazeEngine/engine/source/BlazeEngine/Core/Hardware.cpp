#include "BlazeEngine/Core/Hardware.h"

#include "GL/glew.h"
#include "SDL/SDL.h"

namespace Blaze
{
	namespace Hardware
	{
		namespace CPU
		{
			int NumberOfCores()
			{				
				return SDL_GetCPUCount();
			}
		}
		namespace GPU
		{
			int MaxGeometryShaderUniforms()
			{				
				int num;
				glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &num);
				return num;
			}
		}
	}
}