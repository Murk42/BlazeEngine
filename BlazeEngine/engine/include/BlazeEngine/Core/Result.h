#pragma once

namespace Blaze
{
	class BLAZE_API Result
	{
	public:
		struct StackFrame
		{
			Path filePath;
			uint line = 0;
			String functionName;
		};
		struct StackFrameData
		{
			String report;
			StackFrame frame;
			Array<Log> logs;
			Array<StackFrameData> children;
		};

		Result();
		Result(const Result&) = delete;
		Result(Result&&) noexcept;
		Result(Log log);				

		~Result();

		void AddLogs(const std::vector<Log>& logs);
		void AddLog(Log log);

		String ToString() const;				

		void Clear();

		bool IsEmpty() const;

		operator bool() const;

		Result& JoinResults(Result& result) noexcept;
		Result& JoinResults(Result&& result) noexcept;

		Result& AddStackFrame(StackFrame frame, String report) &;		
		Result& AddStackFrame(StackFrame frame) &;
		Result AddStackFrame(StackFrame frame, String report) &&;
		Result AddStackFrame(StackFrame frame) &&;

		Result& operator+=(Result&) noexcept;
		Result& operator+=(Result&&) noexcept;

		Result& operator=(const Result&) = delete;
		Result& operator=(Result&&) noexcept;

		Result& operator+=(Log);
	private:
		Array<Log> logs;
		Array<StackFrameData> children;		
	};

#define BLAZE_DEBUG_RESULT(source, message)		std::move(::Blaze::Result(BLAZE_DEBUG_LOG(source, message)).AddStackFrame(BLAZE_STACK_FRAME))
#define BLAZE_INFO_RESULT(source, message)		std::move(::Blaze::Result(BLAZE_INFO_LOG(source, message)).AddStackFrame(BLAZE_STACK_FRAME))
#define BLAZE_WARNING_RESULT(source, message)	std::move(::Blaze::Result(BLAZE_WARNING_LOG(source, message)).AddStackFrame(BLAZE_STACK_FRAME))
#define BLAZE_ERROR_RESULT(source, message)		std::move(::Blaze::Result(BLAZE_ERROR_LOG(source, message)).AddStackFrame(BLAZE_STACK_FRAME))
#define BLAZE_FATAL_RESULT(source, message)		std::move(::Blaze::Result(BLAZE_FATAL_LOG(source, message)).AddStackFrame(BLAZE_STACK_FRAME))

#define BLAZE_STACK_FRAME (::Blaze::Result::StackFrame({ BLAZE_FILE_NAME, BLAZE_FILE_LINE, BLAZE_FUNCTION_NAME }))
#define ADD_STACK_FRAME(result) result.AddStackFrame(BLAZE_STACK_FRAME)
#define ADD_STACK_FRAME_REPORTED(result, report) std::move(result.AddStackFrame(BLAZE_STACK_FRAME, report))

#define CHECK_RESULT(result) if (::Blaze::Result macro_result = std::move(result)) { return std::move(macro_result.AddStackFrame(BLAZE_STACK_FRAME)); }
#define CHECK_RESULT_RETURN(result, returnValue) if (::Blaze::Result r = std::move(result)) return returnValue;
#define CHECK_RESULT_REPORTED(result, report) if (::Blaze::Result r = std::move(result)) return ::Blaze::Result(r, BLAZE_STACK_FRAME, report);
}