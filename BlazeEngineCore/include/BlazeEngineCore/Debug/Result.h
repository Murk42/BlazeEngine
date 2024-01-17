#pragma once

namespace Blaze
{
	class BLAZE_CORE_API Result
	{
	public:			
		Result();
		Result(const Result&);
		Result(Result&&) noexcept;
		Result(Debug::Log log);
		Result(std::initializer_list<Debug::Log> logs);

		~Result();

		void AddLogs(std::initializer_list<Debug::Log> logs);
		void AddLog(Debug::Log log);

		StringUTF8 ToString() const;
		StringUTF8 ToStringVerbose() const;

		void Clear();
		void ClearSilent();

		bool IsEmpty() const;
		Debug::LogType HighestLogType() const { return logType; }

		void SupressFatalLogs();

		inline const Debug::Log* GetLogs() const { return logs; }

		operator bool() const;		

		Result& JoinResults(Result& result) noexcept;
		Result& JoinResults(Result&& result) noexcept;		

		Result& operator+=(Result&) noexcept;
		Result& operator+=(Result&&) noexcept;
		Result& operator+=(Debug::Log);

		Result& operator=(const Result&);
		Result& operator=(Result&&) noexcept;
	private:				
		Debug::Log* logs;		
		uint logCount;

		Debug::LogType logType;
		bool supressFatalsLogs;

		//Wont free previous contents
		void CopyUnsafe(const Result& other);
		//Wont check if logs is nullptr
		void AppendUnsafe(Debug::Log* logs, uint count);

	};

	BLAZE_CORE_API Result operator+(Result& a, Result&& b);	
	BLAZE_CORE_API Result operator+(Result&& a, Result& b);
	BLAZE_CORE_API Result operator+(Result&& a, Result&& b);

#define BLAZE_DEBUG_RESULT(source, message)	    ::Blaze::Result(BLAZE_DEBUG_LOG(source, message))
#define BLAZE_INFO_RESULT(source, message)	    ::Blaze::Result(BLAZE_INFO_LOG(source, message))
#define BLAZE_WARNING_RESULT(source, message)   ::Blaze::Result(BLAZE_WARNING_LOG(source, message))
#define BLAZE_ERROR_RESULT(source, message)     ::Blaze::Result(BLAZE_ERROR_LOG(source, message))
#define BLAZE_FATAL_RESULT(source, message)     ::Blaze::Result(BLAZE_FATAL_LOG(source, message))

#define CHECK_RESULT(value) if (::Blaze::Result result = value) return result;
}