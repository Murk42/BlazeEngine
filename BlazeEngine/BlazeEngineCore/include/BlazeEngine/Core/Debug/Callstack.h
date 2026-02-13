#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/File/Path.h"

namespace Blaze::Debug
{
	class BLAZE_API StackFrame
	{
	public:
		StackFrame();
		StackFrame(const StackFrame&) = default;
		StackFrame(StringView filePath, uintMem fileLine, const void* address, StringView locationName);

		StringView GetFilePath() const;
		inline uintMem GetFileLine() const { return fileLine; }
		inline const void* Address() const { return address; }
		StringView GetLocationName() const;

		bool operator==(const StackFrame& other) const;
		bool operator!=(const StackFrame& other) const;

		StackFrame& operator=(const StackFrame&) = default;
	private:
		char filePath[128];
		uintMem fileLine;
		const void* address;
		char locationName[64];
	};

	class BLAZE_API Callstack
	{
	public:
		Callstack();
		Callstack(uint skip, uint max_depth = UINT_MAX);
		Callstack(const Callstack&);
		Callstack(Callstack&&) noexcept;
		~Callstack();

		void Clear();
		inline bool Empty() const { return frameCount == 0; }

		uintMem Count() const;

		const StackFrame* FirstStackFrame() const;

		const StackFrame& operator[](int i) const;

		bool operator==(const Callstack& other) const;
		bool operator!=(const Callstack& other) const;

		Callstack& operator=(const Callstack&);
		Callstack& operator=(Callstack&&) noexcept;
	private:
		StackFrame* frames;
		uintMem frameCount;

		//Wont free previous contents
		void CopyUnsafe(const Callstack&);
	};

	const StackFrame* begin(const Callstack& callstack);
	const StackFrame* end(const Callstack& callstack);
}