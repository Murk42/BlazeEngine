#include "pch.h"
#include "BlazeEngine/Core/Debug/Callstack.h"
#include "BlazeEngine/Core/Memory/MemoryManager.h"
#include "BlazeEngine/Core/String/StringView.h"

#define BOOST_STACKTRACE_USE_WINDBG_CACHED
#include <boost/stacktrace/stacktrace.hpp>
#undef min

namespace Blaze::Debug
{
	StackFrame::StackFrame()
		: fileLine(0), address(nullptr)
	{
		filePath[0] = '\0';
		locationName[0] = '\0';
	}
	StackFrame::StackFrame(StringView filePath, uintMem fileLine, const void* address, StringView locationName)
		: fileLine(fileLine), address(address)
	{
		{
			uintMem countToCopy = std::min(_countof(this->filePath) - 1, filePath.Count());
			memcpy(this->filePath, filePath.Ptr() + (filePath.Count() - countToCopy), countToCopy);
			this->filePath[countToCopy] = '\0';
		}

		{
			uintMem countToCopy = std::min(_countof(this->locationName) - 1, locationName.Count());
			memcpy(this->locationName, locationName.Ptr(), countToCopy);
			this->locationName[countToCopy] = '\0';
		}
	}
	StringView StackFrame::GetFilePath() const
	{
		uintMem size = 0;
		for (; size < _countof(filePath) - 1; ++size)
			if (filePath[size] == '\0')
				break;

		return StringView(filePath, size);
	}
	StringView StackFrame::GetLocationName() const
	{
		uintMem size = 0;
		for (; size < _countof(locationName) - 1; ++size)
			if (locationName[size] == '\0')
				break;

		return StringView(locationName, size);
	}
	bool StackFrame::operator==(const StackFrame& other) const
	{
		return address == other.address && fileLine == other.fileLine;
	}
	bool StackFrame::operator!=(const StackFrame& other) const
	{
		return address != other.address || fileLine != other.fileLine;
	}

	template<typename T>
	struct StacktraceAllocator
	{
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using size_type = uintMem;
		using difference_type = intMem;

		StacktraceAllocator() {}
		template<typename T2>
		StacktraceAllocator(const StacktraceAllocator<T2>&) { }

		[[nodiscard]] T* allocate(uintMem n)
		{
			return static_cast<T*>(Memory::Allocate(sizeof(T) * n));
		}
		void deallocate(T* p, uintMem n) noexcept
		{
			Memory::Free(p);
		}
	};


	Callstack::Callstack()
		: frames(nullptr), frameCount(0)
	{
	}
	Callstack::Callstack(uint skip, uint max_depth)
		: frames(nullptr), frameCount(0)
	{
		try
		{
			using Stacktrace = boost::stacktrace::basic_stacktrace<StacktraceAllocator<boost::stacktrace::frame>>;

			auto stacktrace = Stacktrace(skip + 2, max_depth);

			frames = (StackFrame*)Memory::Allocate(sizeof(StackFrame) * stacktrace.size());

			for (uint i = 0; i < stacktrace.size(); ++i)
			{
				auto sourceFile = stacktrace[i].source_file();
				auto name = stacktrace[i].name();

				std::construct_at(&frames[i], StackFrame(
					StringView(sourceFile.data(), sourceFile.size()),
					stacktrace[i].source_line(),
					stacktrace[i].address(),
					StringView(name.data(), name.size())
				));
			}

			frameCount = stacktrace.size();
		}
		catch (std::exception exception)
		{
			Logger::ProcessLog(Log(LogType::Error, "Blaze Engine", Format("Failed to capture callstack. Exception thrown: {}", exception.what()), { }));
			frameCount = 0;
			frames = nullptr;
		}
		catch (...)
		{
			Logger::ProcessLog(Log(LogType::Error, "Blaze Engine", "Failed to capture callstack. Unknown exception thrown", { }));
			frameCount = 0;
			frames = nullptr;
		}
	}

	Callstack::Callstack(const Callstack& other)
	{
		CopyUnsafe(other);
	}

	Callstack::Callstack(Callstack&& other) noexcept
		: frames(other.frames), frameCount(other.frameCount)
	{
		other.frames = nullptr;
		other.frameCount = 0;
	}
	Callstack::~Callstack()
	{
		Clear();
	}
	void Callstack::Clear()
	{
		std::destroy_n(&frames, frameCount);
		Memory::Free(frames);
		frames = nullptr;
		frameCount = 0;
	}
	uintMem Callstack::Count() const
	{
		return frameCount;
	}
	const StackFrame* Callstack::FirstStackFrame() const
	{
		return frames;
	}
	const StackFrame& Callstack::operator[](int i) const
	{
		if (i > frameCount)
			BLAZE_LOG_FATAL("Index out of bounds");

		return frames[i];
	}
	bool Callstack::operator==(const Callstack& other) const
	{
		if (frameCount != other.frameCount)
			return false;

		for (uint i = 0; i < frameCount; ++i)
			if (frames[i] != other.frames[i])
				return false;

		return true;
	}
	bool Callstack::operator!=(const Callstack& other) const
	{
		return !operator==(other);
	}
	Callstack& Callstack::operator=(const Callstack& other)
	{
		Clear();
		CopyUnsafe(other);
		return *this;
	}
	Callstack& Callstack::operator=(Callstack&& other) noexcept
	{
		frames = other.frames;
		frameCount = other.frameCount;
		other.frames = nullptr;
		other.frameCount = 0;

		return *this;
	}
	void Callstack::CopyUnsafe(const Callstack& other)
	{
		frames = (StackFrame*)Memory::Allocate(sizeof(StackFrame) * other.frameCount);
		frameCount = other.frameCount;

		for (uint i = 0; i < frameCount; ++i)
			std::construct_at(&frames[i], other.frames[i]);
	}
	const StackFrame* begin(const Callstack& callstack)
	{
		return callstack.FirstStackFrame();
	}
	const StackFrame* end(const Callstack& callstack)
	{
		return callstack.FirstStackFrame() + callstack.Count();
	}
}