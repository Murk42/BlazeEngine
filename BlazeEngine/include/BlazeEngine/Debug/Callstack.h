#pragma once

namespace Blaze::Debug
{
	class BLAZE_API StackFrame
	{
	public:
		StackFrame();
		StackFrame(const StackFrame&) = default;

		inline bool IsEmpty() const { return address == nullptr; }

		inline Path FilePath() const { return file; }
		inline uintMem FileLine() const { return line; }
		inline const void* Address() const { return address; }
		inline String LocationName() const { return locationName; }

		bool operator==(const StackFrame& other) const;
		bool operator!=(const StackFrame& other) const;

		StackFrame& operator=(const StackFrame&) = default;		
	private:
		Path file;
		uintMem line;
		const void* address;
		String locationName;	

		friend class Callstack;
	};
	
	class BLAZE_API Callstack
	{
	public:
		Callstack(uint skip = 1, uint max_depth = UINT_MAX);
		Callstack(const Callstack&);
		Callstack(Callstack&&) noexcept;
		~Callstack();

		void Clear();

		uintMem Count() const;

		const StackFrame* begin() const;
		const StackFrame* end() const;

		const StackFrame& operator[](int i) const;

		bool operator==(const Callstack& other) const;
		bool operator!=(const Callstack& other) const;

		Callstack& operator=(const Callstack&);
		Callstack& operator=(Callstack&&) noexcept;
	private:
		StackFrame* frames;
		uintMem frameCount;
		uint skips;

		//Wont free previous contents
		void CopyUnsafe(const Callstack&);
	};
}