#include "pch.h"
#include "BlazeEngineCore/Debug/Callstack.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"

#define BOOST_STACKTRACE_USE_WINDBG_CACHED
#include <boost/stacktrace/stacktrace.hpp>	
#undef min

namespace Blaze::Debug
{
	StackFrame::StackFrame()
		: line(0), address(nullptr)
	{
	}

	bool StackFrame::operator==(const StackFrame& other) const
	{
		return line == other.line && file == other.file;
	}

	bool StackFrame::operator!=(const StackFrame& other) const
	{
		return line != other.line || file != other.file;
	}
	
	Callstack::Callstack(uint skip, uint max_depth)
		: skips(skip)
	{		
		auto stacktrace = boost::stacktrace::stacktrace(skip + 2, max_depth);
		
		frames = (StackFrame*)Memory::Allocate(sizeof(StackFrame) * stacktrace.size());

		for (uint i = 0; i < stacktrace.size(); ++i)
		{
			std::construct_at(&frames[i]);

			auto s = stacktrace[i].source_file();
			frames[i].file = StringViewUTF8(s.data(), s.size());
			frames[i].line = stacktrace[i].source_line();
			frames[i].address = stacktrace[i].address();
			s = stacktrace[i].name();
			frames[i].locationName = StringView(s.data(), s.size());
		}
		frameCount = stacktrace.size();
	}

	Callstack::Callstack(const Callstack& other)
	{
		CopyUnsafe(other);
	}

	Callstack::Callstack(Callstack&& other) noexcept
		: frames(other.frames), frameCount(other.frameCount), skips(other.skips)
	{
		other.frames = nullptr;
		other.frameCount = 0;
		other.skips = 0;
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
		skips = 0;
	}

	uintMem Callstack::Count() const
	{ 
		if (frameCount < skips)
			return 0;

		return frameCount - skips;
	}

	const StackFrame* Callstack::begin() const 
	{ 
		if (frameCount < skips)
			return nullptr;

		return frames + skips; 
	}

	const StackFrame* Callstack::end() const 
	{ 
		if (frameCount < skips)
			return nullptr;

		return frames + frameCount; 
	}
	const StackFrame& Callstack::operator[](int i) const 
	{ 
		uint realIndex = (uint)(i + (int)skips);
		return frames[realIndex]; 
	}
	bool Callstack::operator==(const Callstack& other) const
	{
		uintMem minSize = std::min(frameCount, other.frameCount);

		for (uint i = 0; i < minSize; ++i)
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
		skips = other.skips;
		other.frames = nullptr;
		other.frameCount = 0;
		other.skips = 0;

		return *this;
	}

	void Callstack::CopyUnsafe(const Callstack& other)
	{
		frames = (StackFrame*)Memory::Allocate(sizeof(StackFrame) * other.frameCount);
		frameCount = other.frameCount;

		for (uint i = 0; i < frameCount; ++i)
			std::construct_at(&frames[i], other.frames[i]);

		skips = other.skips;
	}

}