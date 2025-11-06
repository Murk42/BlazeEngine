#pragma once
#include "BlazeEngine/Core/Time/Timing.h"
#include "sail/sail.h"

class ReadStream;
class WriteStream;

namespace Blaze
{
	void sail_SetupReadIO(sail_io* io, ReadStream& stream);
	void sail_SetupWriteIO(sail_io* io, WriteStream& stream);

	StringView sail_GetStatusString(sail_status_t status);

	void sail_EnableLogs(bool enable);

	TimingResult InitializeSail();
	void TerminateSail();

#define SAIL_CHECK_RET(value, ret, message, ...) if (sail_status_t status = (value)) { Debug::Logger::LogError(u8"SAIL image library", Format<char8_t>(message + StringView(". Returned value:") + sail_GetStatusString(status), __VA_ARGS__)); return ret; }
#define SAIL_CHECK(value, message, ...) if (sail_status_t status = (value)) { Debug::Logger::LogError(u8"SAIL image library", Format<char8_t>(message + StringView(". Returned value: ") + sail_GetStatusString(status), __VA_ARGS__)); return; }
#define SAIL_NO_LOGS(value) ([&](){ sail_EnableLogs(false); auto status = (value); sail_EnableLogs(true); return status; }())
}