#pragma once
#include <vector>

namespace Blaze
{
	class BLAZE_API Result
	{
	public:
		Result();
		Result(const Result&) = delete;
		Result(Result&&) noexcept;
		
		Result(const Log& log, bool failed = true);		

		~Result();		

		void AddLogs(const std::vector<Log>& logs);
		void AddLog(const Log& log);

		void SetFailed(bool failed);
		std::vector<Log> GetLogs() const { return logs; }
		void Clear();

		String FormatString() const;

		operator bool() const { return failed; }

		Result& operator=(const Result&) = delete;
		Result& operator=(Result&&) noexcept;
		
		Result operator+(Result&&);
		Result operator+(Result&);

		Result& operator+=(Result&&);
		Result& operator+=(Result&);
	private:
		bool failed;		
		std::vector<Log> logs;
	};

#define BLAZE_DEBUG_RESULT(source, message)		::Blaze::Result(::Blaze::Log(::Blaze::LogType::Debug	, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message))
#define BLAZE_INFO_RESULT(source, message)		::Blaze::Result(::Blaze::Log(::Blaze::LogType::Info		, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message))
#define BLAZE_WARNING_RESULT(source, message)	::Blaze::Result(::Blaze::Log(::Blaze::LogType::Warning	, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message))
#define BLAZE_ERROR_RESULT(source, message)		::Blaze::Result(::Blaze::Log(::Blaze::LogType::Error	, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message))
#define BLAZE_FATAL_RESULT(source, message)		::Blaze::Result(::Blaze::Log(::Blaze::LogType::Fatal	, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, source, message))
}