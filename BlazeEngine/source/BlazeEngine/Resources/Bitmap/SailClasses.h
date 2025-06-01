#pragma once
#include "BlazeEngineCore/DataStructures/StringView.h"

namespace Blaze
{	
	class ReadStream;
	class WriteStream;

	void SetupSailReadIO(sail_io* io, ReadStream& stream);
	void SetupSailWriteIO(sail_io* io, WriteStream& stream);	

	StringView GetSailStatusString(sail_status_t status);

#define SAIL_CHECK(value, message, ret) if (sail_status_t status = (value)) { Debug::Logger::LogError("SAIL image library", message + StringView(" Returned value:") + GetSailStatusString(status)); return ret; }
}