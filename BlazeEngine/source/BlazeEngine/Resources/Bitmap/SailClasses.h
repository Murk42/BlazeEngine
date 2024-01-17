#pragma once

namespace Blaze
{
	void SetupSailReadIO(sail_io* io, ReadStream& stream);
	void SetupSailWriteIO(sail_io* io, WriteStream& stream);	

	StringView GetSailStatusString(sail_status_t status);

#define SAIL_CHECK(value, message) if (sail_status_t status = (value)) return BLAZE_ERROR_RESULT("SAIL image library", message + StringView(" Returned value:") + GetSailStatusString(status));
}