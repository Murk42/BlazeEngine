#include "pch.h"
#include "BlazeEngine/External/sail/sail.h"
#include "BlazeEngine/Core/Common/Stream.h"
#include "BlazeEngine/Core/Debug/Logger.h"

#include "sail/sail.h"

namespace Blaze
{
	bool sailLogsEnabled = true;
	static void SailLogger(SailLogLevel level, const char* file, int line, const char* format, va_list args)
	{
		if (!sailLogsEnabled)
			return;

		if (level == SAIL_LOG_LEVEL_INFO || level == SAIL_LOG_LEVEL_DEBUG)
			return;

		// Copy args for _vscprintf
		va_list argsCopy1;
		va_copy(argsCopy1, args);
		int size = _vscprintf(format, argsCopy1);
		va_end(argsCopy1);

		String message;

		if (size <= 0)
			BLAZE_LOG_ERROR("Failed to format sail log");
		else
		{
			message.Resize(size, ' ');
			// Copy args for vsprintf_s
			va_list argsCopy2;
			va_copy(argsCopy2, args);
			if (vsprintf_s(message.Ptr(), size + 1, format, argsCopy2) <= 0)
				BLAZE_LOG_ERROR("Failed to format sail log");
			va_end(argsCopy2);
		}

		StringView levelString;
		Debug::LogType logType;

		switch (level)
		{
		case SAIL_LOG_LEVEL_SILENCE: logType = Debug::LogType::Info;   levelString = "SILENCE"; break;
		case SAIL_LOG_LEVEL_ERROR:   logType = Debug::LogType::Error;  levelString = "ERROR"; break;
		case SAIL_LOG_LEVEL_WARNING: logType = Debug::LogType::Warning;levelString = "WARNING"; break;
		case SAIL_LOG_LEVEL_INFO:    logType = Debug::LogType::Info;   levelString = "INFO"; break;
		case SAIL_LOG_LEVEL_MESSAGE: logType = Debug::LogType::Info;   levelString = "MESSAGE"; break;
		case SAIL_LOG_LEVEL_DEBUG:   logType = Debug::LogType::Debug;  levelString = "DEBUG"; break;
		case SAIL_LOG_LEVEL_TRACE:   logType = Debug::LogType::Debug;  levelString = "TRACE"; break;
		default: levelString = "UNKNOWN"; break;
		}

		Debug::Log log{ logType, "sail", Format(u8"sail ({}, line {}) level {}: \"{}\"", Path(StringView(file, strlen(file))).FileName(), line, levelString, message)};

		Debug::Logger::ProcessLog(log);
	}

	void sail_SetupReadIO(sail_io* io, ReadStream& stream)
	{
		io->stream = &stream;
		io->features = SAIL_IO_FEATURE_SEEKABLE;
		io->tolerant_read = [](void* stream, void* buf, std::size_t size_to_read, std::size_t* read_size)
			{
				*read_size = (*(ReadStream*)stream).Read(buf, size_to_read);
				return SAIL_OK;
			};
		io->strict_read = [](void* stream, void* buf, std::size_t size_to_read)
			{
				uintMem read_size = (*(ReadStream*)stream).Read(buf, size_to_read);
				if (read_size != size_to_read)
					return SAIL_ERROR_READ_IO;
				return SAIL_OK;
			};
		io->tolerant_write = [](void* stream, const void* buf, std::size_t size_to_write, std::size_t* written_size)
			{
				return SAIL_ERROR_NOT_IMPLEMENTED;
			};
		io->strict_write = [](void* stream, const void* buf, std::size_t size_to_write)
			{
				return SAIL_ERROR_NOT_IMPLEMENTED;
			};
		io->seek = [](void* stream, long offset, int whence)
		{
			switch (whence)
			{
			case SEEK_SET:
				(*(ReadStream*)stream).SetPosition(offset);
				break;
			case SEEK_CUR:
				(*(ReadStream*)stream).MovePosition(offset);
				break;
			case SEEK_END:
				(*(ReadStream*)stream).SetPositionFromEnd(offset);
				break;
			}

			return SAIL_OK;
		};
		io->tell = [](void* stream, std::size_t * offset)
		{
			*offset = (*(ReadStream*)stream).GetPosition();
			return SAIL_OK;
		};
		io->flush = [](void* stream)
			{
				return SAIL_OK;
			};
		io->close = [](void* stream)
			{
				return SAIL_OK;
			};
		io->eof = [](void* stream, bool* result)
			{
				*result = (*(ReadStream*)stream).GetPosition() == (*(ReadStream*)stream).GetSize();
				return SAIL_OK;
			};
	}

	void sail_SetupWriteIO(sail_io* io, WriteStream& stream)
	{
		io->stream = &stream;
		io->features = SAIL_IO_FEATURE_SEEKABLE;
		io->tolerant_read = [](void* stream, void* buf, std::size_t size_to_read, std::size_t* read_size)
			{
				return SAIL_ERROR_NOT_IMPLEMENTED;
			};
		io->strict_read = [](void* stream, void* buf, std::size_t size_to_read)
			{
				return SAIL_ERROR_NOT_IMPLEMENTED;
			};
		io->tolerant_write = [](void* stream, const void* buf, std::size_t size_to_write, std::size_t* written_size)
			{
				*written_size = (*(WriteStream*)stream).Write(buf, size_to_write);
				return SAIL_OK;
			};
		io->strict_write = [](void* stream, const void* buf, std::size_t size_to_write)
			{
				uintMem written_size = (*(WriteStream*)stream).Write(buf, size_to_write);
				if (written_size != size_to_write)
					return SAIL_ERROR_WRITE_IO;
				return SAIL_OK;
			};
		io->seek = [](void* stream, long offset, int whence)
			{
				switch (whence)
				{
				case SEEK_SET:
					(*(WriteStream*)stream).SetPosition(offset);
					break;
				case SEEK_CUR:
					(*(WriteStream*)stream).MovePosition(offset);
					break;
				case SEEK_END:
					(*(WriteStream*)stream).SetPositionFromEnd(offset);
					break;
				}

				return SAIL_OK;
			};
		io->tell = [](void* stream, std::size_t* offset)
			{
				*offset = (*(WriteStream*)stream).GetPosition();
				return SAIL_OK;
			};
		io->flush = [](void* stream)
			{
				return SAIL_OK;
			};
		io->close = [](void* stream)
			{
				return SAIL_OK;
			};
		io->eof = [](void* stream, bool* result)
			{
				*result = (*(WriteStream*)stream).GetPosition() == (*(WriteStream*)stream).GetSize();
				return SAIL_OK;
			};
	}

	StringView sail_GetStatusString(sail_status_t status)
	{
		switch (status)
		{
		case SAIL_OK: return "SAIL_OK";
		case SAIL_ERROR_NULL_PTR: return "SAIL_ERROR_NULL_PT";
		case SAIL_ERROR_MEMORY_ALLOCATION: return "SAIL_ERROR_MEMORY_ALLOCATION";
		case SAIL_ERROR_OPEN_FILE: return "SAIL_ERROR_OPEN_FILE";
		case SAIL_ERROR_READ_FILE: return "SAIL_ERROR_READ_FILE";
		case SAIL_ERROR_SEEK_FILE: return "SAIL_ERROR_SEEK_FILE";
		case SAIL_ERROR_CLOSE_FILE: return "SAIL_ERROR_CLOSE_FILE";
		case SAIL_ERROR_LIST_DIR: return "SAIL_ERROR_LIST_DIR";
		case SAIL_ERROR_PARSE_FILE: return "SAIL_ERROR_PARSE_FILE";
		case SAIL_ERROR_INVALID_ARGUMENT: return "SAIL_ERROR_INVALID_ARGUMENT";
		case SAIL_ERROR_READ_IO: return "SAIL_ERROR_READ_IO";
		case SAIL_ERROR_WRITE_IO: return "SAIL_ERROR_WRITE_IO";
		case SAIL_ERROR_FLUSH_IO: return "SAIL_ERROR_FLUSH_IO";
		case SAIL_ERROR_SEEK_IO: return "SAIL_ERROR_SEEK_IO";
		case SAIL_ERROR_TELL_IO: return "SAIL_ERROR_TELL_IO";
		case SAIL_ERROR_CLOSE_IO: return "SAIL_ERROR_CLOSE_IO";
		case SAIL_ERROR_EOF: return "SAIL_ERROR_EOF";
		case SAIL_ERROR_NOT_IMPLEMENTED: return "SAIL_ERROR_NOT_IMPLEMENTED";
		case SAIL_ERROR_UNSUPPORTED_SEEK_WHENCE: return "SAIL_ERROR_UNSUPPORTED_SEEK_WHENCE";
		case SAIL_ERROR_EMPTY_STRING: return "SAIL_ERROR_EMPTY_STRING";
		case SAIL_ERROR_INVALID_VARIANT: return "SAIL_ERROR_INVALID_VARIANT";
		case SAIL_ERROR_INVALID_IO: return "SAIL_ERROR_INVALID_IO";
		case SAIL_ERROR_INCORRECT_IMAGE_DIMENSIONS: return "SAIL_ERROR_INCORRECT_IMAGE_DIMENSIONS";
		case SAIL_ERROR_UNSUPPORTED_PIXEL_FORMAT: return "SAIL_ERROR_UNSUPPORTED_PIXEL_FORMAT";
		case SAIL_ERROR_INVALID_PIXEL_FORMAT: return "SAIL_ERROR_INVALID_PIXEL_FORMAT";
		case SAIL_ERROR_UNSUPPORTED_COMPRESSION: return "SAIL_ERROR_UNSUPPORTED_COMPRESSION";
		case SAIL_ERROR_UNSUPPORTED_META_DATA: return "SAIL_ERROR_UNSUPPORTED_META_DATA";
		case SAIL_ERROR_UNDERLYING_CODEC: return "SAIL_ERROR_UNDERLYING_CODEC";
		case SAIL_ERROR_NO_MORE_FRAMES: return "SAIL_ERROR_NO_MORE_FRAMES";
		case SAIL_ERROR_INTERLACING_UNSUPPORTED: return "SAIL_ERROR_INTERLACING_UNSUPPORTED";
		case SAIL_ERROR_INCORRECT_BYTES_PER_LINE: return "SAIL_ERROR_INCORRECT_BYTES_PER_LINE";
		case SAIL_ERROR_UNSUPPORTED_IMAGE_PROPERTY: return "SAIL_ERROR_UNSUPPORTED_IMAGE_PROPERTY";
		case SAIL_ERROR_UNSUPPORTED_BIT_DEPTH: return "SAIL_ERROR_UNSUPPORTED_BIT_DEPTH";
		case SAIL_ERROR_MISSING_PALETTE: return "SAIL_ERROR_MISSING_PALETTE";
		case SAIL_ERROR_UNSUPPORTED_FORMAT: return "SAIL_ERROR_UNSUPPORTED_FORMAT";
		case SAIL_ERROR_BROKEN_IMAGE: return "SAIL_ERROR_BROKEN_IMAGE			";
		case SAIL_ERROR_CODEC_LOAD: return "SAIL_ERROR_CODEC_LOAD";
		case SAIL_ERROR_CODEC_NOT_FOUND: return "SAIL_ERROR_CODEC_NOT_FOUND";
		case SAIL_ERROR_UNSUPPORTED_CODEC_LAYOUT: return "SAIL_ERROR_UNSUPPORTED_CODEC_LAYOUT";
		case SAIL_ERROR_CODEC_SYMBOL_RESOLVE: return "SAIL_ERROR_CODEC_SYMBOL_RESOLVE";
		case SAIL_ERROR_INCOMPLETE_CODEC_INFO: return "SAIL_ERROR_INCOMPLETE_CODEC_INFO";
		case SAIL_ERROR_UNSUPPORTED_CODEC_FEATURE: return "SAIL_ERROR_UNSUPPORTED_CODEC_FEATURE";
		case SAIL_ERROR_UNSUPPORTED_CODEC_PRIORITY: return "SAIL_ERROR_UNSUPPORTED_CODEC_PRIORITY";
		case SAIL_ERROR_ENV_UPDATE: return "SAIL_ERROR_ENV_UPDATE";
		case SAIL_ERROR_CONTEXT_UNINITIALIZED: return "SAIL_ERROR_CONTEXT_UNINITIALIZED";
		case SAIL_ERROR_GET_DLL_PATH: return "SAIL_ERROR_GET_DLL_PATH";
		case SAIL_ERROR_CONFLICTING_OPERATION: return "SAIL_ERROR_CONFLICTING_OPERATION";
		default: return "INVALID_STATUS";
		}
	}

	void sail_EnableLogs(bool enable)
	{
		sailLogsEnabled = enable;
	}

	TimingResult InitializeSail()
	{
		Timing timing{ "sail" };

		sail_set_logger(SailLogger);
		sail_init_with_flags(SAIL_FLAG_PRELOAD_CODECS);

		Array<StringView> imageFormats;

		auto node = sail_codec_bundle_list();
		while (node != nullptr)
		{
			auto info = node->codec_bundle->codec_info;
			imageFormats.AddBack(StringView(info->name, strlen(info->name)));

			node = node->next;
		}

		BLAZE_LOG_INFO("<color=green>Successfully<color/> initialized sail " SAIL_VERSION_STRING " ({.1f}ms). Available image formats : {}", timing.GetTimingResult().time.GetSeconds() * 1000.0, StringView(", ").Join(imageFormats));

		return timing.GetTimingResult();
	}

	void TerminateSail()
	{
	}
}