#pragma once
#include "BlazeEngineCore/Time/Timing.h"
#include "sail/sail.h"

class ReadStream;
class WriteStream;
struct sail_io;

namespace Blaze
{
	void SetupSailReadIO(sail_io* io, ReadStream& stream);
	void SetupSailWriteIO(sail_io* io, WriteStream& stream);

	StringView GetSailStatusString(sail_status_t status);

	void EnableSailLogs(bool enable);

	TimingResult InitializeSail();
	void TerminateSail();

#define SAIL_CHECK_RET(value, ret, message, ...) if (sail_status_t status = (value)) { Debug::Logger::LogError(u8"SAIL image library", Format<char8_t>(message + StringView(". Returned value:") + GetSailStatusString(status), __VA_ARGS__)); return ret; }
#define SAIL_CHECK(value, message, ...) if (sail_status_t status = (value)) { Debug::Logger::LogError(u8"SAIL image library", Format<char8_t>(message + StringView(". Returned value: ") + GetSailStatusString(status), __VA_ARGS__)); return; }
#define SAIL_NO_LOGS(value) ([&](){ EnableSailLogs(false); auto status = (value); EnableSailLogs(true); return status; }())
}