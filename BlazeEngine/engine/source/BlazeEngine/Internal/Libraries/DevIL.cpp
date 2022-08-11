#include "source/BlazeEngine/Internal/Libraries/DevIL.h"
#include "BlazeEngine/Utilities/Time.h"

#include "IL/il.h"
#include "IL/ilu.h"

namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::DevILInitInfo InitializeDevIL()
	{
		Startup::BlazeLibrariesInitInfo::DevILInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

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

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateDevIL()
	{
		ilShutDown();
	}
}