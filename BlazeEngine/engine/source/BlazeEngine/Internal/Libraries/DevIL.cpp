#include "source/BlazeEngine/Internal/Libraries/DevIL.h"
#include "BlazeEngine/DataStructures/String.h"

#include "IL/il.h"
#include "IL/ilu.h"

namespace Blaze
{
	void InitializeDevIL()
	{
		ILenum error;

		ilInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw
			"Failed to initialize the DevIL library!\n"
			"DevIL error code: " + String::Convert(error);

		iluInit();

		error = ilGetError();
		if (error != IL_NO_ERROR)
			throw
			"Failed to initialize the DevIL library!\n"
			"DevIL error code: " + String::Convert(error);
	}
	void TerminateDevIL()
	{
		ilShutDown();
	}
}