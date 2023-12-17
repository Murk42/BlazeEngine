#include "pch.h"
#include "sail/sail.h"
#include "SailClasses.h"

namespace Blaze
{		
	/*
	 * Reads from the underlying I/O object into the specified buffer. In contrast to strict_read(),
	 * doesn't fail when the actual number of bytes read is smaller than requested.
	 * Assigns the number of bytes actually read to the 'read_size' argument.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_tolerant_read(void* stream, void* buf, std::size_t size_to_read, std::size_t* read_size)
	{
		*read_size = (*(ReadStream*)stream).Read(buf, size_to_read);
		return SAIL_OK;
	}

	/*
	 * Reads from the underlying I/O object into the specified buffer. In contrast to tolerant_read(),
	 * fails when the actual number of bytes read is smaller than requested.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_strict_read(void* stream, void* buf, std::size_t size_to_read)
	{
		uintMem read_size = (*(ReadStream*)stream).Read(buf, size_to_read);
		if (read_size != size_to_read)
			return SAIL_ERROR_READ_IO;
		return SAIL_OK;
	}	

	/*
	 * Sets the I/O position in the underlying I/O object.
	 *
	 * Possible 'whence' values: SEEK_SET, SEEK_CUR, or SEEK_END declared in <cstdio>.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_seek(void* stream, long offset, int whence)
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
	}

	/*
	 * Assigns the current I/O position in the underlying I/O object.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_tell(void* stream, std::size_t* offset)
	{
		*offset = (*(ReadStream*)stream).GetPosition();
		return SAIL_OK;
	}

	/*
	 * Flushes buffers of the underlying I/O object. Has no effect if the underlying I/O object
	 * is opened for reading.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_flush(void* stream)
	{
		return SAIL_OK;
	}

	/*
	 * Closes the underlying I/O object.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_close(void* stream)
	{
		return SAIL_OK;
	}

	/*
	 * Assigns true to the specified result if the underlying I/O object reached the end-of-file indicator.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t read_io_eof(void* stream, bool* result)
	{
		*result = (*(ReadStream*)stream).GetPosition() == (*(ReadStream*)stream).GetSize();
		return SAIL_OK;
	}

	/*
	 * Finds and returns a first codec info object that can theoretically read the underlying
	 * I/O stream into a valid image.
	 *
	 * Returns an invalid codec info object if no suitable codec was found.
	 */
	static sail_codec_info* read_io_codec_info(void* stream)
	{
		return nullptr;
	}

	void SetupSailReadIO(sail_io* io, ReadStream& stream)
	{
		io->stream = &stream;
		io->features = SAIL_IO_FEATURE_SEEKABLE;
		io->tolerant_read = read_io_tolerant_read;
		io->strict_read = read_io_strict_read;
		io->tolerant_write = nullptr;
		io->strict_write = nullptr;
		io->seek = read_io_seek;
		io->tell = read_io_tell;
		io->flush = read_io_flush;
		io->close = read_io_close;
		io->eof = read_io_eof;
	}

	/*
	 * Writes the specified buffer to the underlying I/O object. In contrast to strict_write(),
	 * doesn't fail when the actual number of bytes written is smaller than requested.
	 * Assigns the number of bytes actually written to the 'written_size' argument.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_tolerant_write(void* stream, const void* buf, std::size_t size_to_write, std::size_t* written_size)
	{
		*written_size = (*(WriteStream*)stream).Write(buf, size_to_write);
		return SAIL_OK;
	}

	/*
	 * Writes the specified buffer to the underlying I/O object. In contrast to tolerant_write(),
	 * fails when the actual number of bytes written is smaller than requested.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_strict_write(void* stream, const void* buf, std::size_t size_to_write)
	{
		uintMem written_size = (*(WriteStream*)stream).Write(buf, size_to_write);
		if (written_size != size_to_write)
			return SAIL_ERROR_WRITE_IO;
		return SAIL_OK;
	}

	/*
	 * Sets the I/O position in the underlying I/O object.
	 *
	 * Possible 'whence' values: SEEK_SET, SEEK_CUR, or SEEK_END declared in <cstdio>.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_seek(void* stream, long offset, int whence)
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
	}

	/*
	 * Assigns the current I/O position in the underlying I/O object.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_tell(void* stream, std::size_t* offset)
	{
		*offset = (*(WriteStream*)stream).GetPosition();
		return SAIL_OK;
	}

	/*
	 * Flushes buffers of the underlying I/O object. Has no effect if the underlying I/O object
	 * is opened for reading.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_flush(void* stream)
	{
		return SAIL_OK;
	}

	/*
	 * Closes the underlying I/O object.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_close(void* stream)
	{
		return SAIL_OK;
	}

	/*
	 * Assigns true to the specified result if the underlying I/O object reached the end-of-file indicator.
	 *
	 * Returns SAIL_OK on success.
	 */
	static sail_status_t write_io_eof(void* stream, bool* result)
	{
		*result = (*(WriteStream*)stream).GetPosition() == (*(WriteStream*)stream).GetSize();
		return SAIL_OK;
	}

	/*
	 * Finds and returns a first codec info object that can theoretically read the underlying
	 * I/O stream into a valid image.
	 *
	 * Returns an invalid codec info object if no suitable codec was found.
	 */
	static sail_codec_info* write_io_codec_info(void* stream)
	{
		return nullptr;
	}

	void SetupSailWriteIO(sail_io* io, WriteStream& stream)
	{
		io->stream = &stream;
		io->features = SAIL_IO_FEATURE_SEEKABLE;
		io->tolerant_read = nullptr;
		io->strict_read = nullptr;
		io->tolerant_write = write_io_tolerant_write;
		io->strict_write = write_io_strict_write;
		io->seek = write_io_seek;
		io->tell = write_io_tell;
		io->flush = write_io_flush;
		io->close = write_io_close;
		io->eof = write_io_eof;
	}
	const char* GetSailStatusString(sail_status_t status)
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
}