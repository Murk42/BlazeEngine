#pragma once
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/Console/Console.h"
#include <string.h>
#include <utility>

namespace Blaze
{
	Result::Result()	
	{		
	}	
	Result::Result(Result&& result) noexcept
		: logs(std::move(result.logs)), children(std::move(result.children))
	{

	}
	Result::Result(Log log)				
	{		
		logs.Add(std::move(log));
	}	
	Result::~Result()
	{		
		if (!IsEmpty())
			Logger::ProcessResult(std::move(*this));
	}
	void Result::AddLogs(const std::vector<Log>& logs)
	{
		for (auto& log : logs)
			AddLog(log);
	}
	void Result::AddLog(Log log)
	{
		logs.Add(std::move(log));		
	}

	String StackFrameToString(Result::StackFrameData* data, uint indent)
	{		
		String out;

		String prefix(indent * 3);
		memset(prefix.Ptr(), ' ', indent * 3);

		out = prefix.SubString(0, indent * 3 - 3) + data->frame.filePath.FileName() + ", line " + StringParsing::Convert(data->frame.line) + " (" + data->frame.functionName + ")\n";

		if (data->logs.Count() > 0 || data->report.Size() != 0)
		{
			out += "\n";

			for (auto& log : data->logs)
			{
				String logString = log.ToString();
				out += prefix + logString + "\n";
			}

			out += prefix + data->report + "\n";

			out += "\n";
		}

		for (auto& child : data->children)
			out += StackFrameToString(&child, indent + 1);

		return out;
	}

	String Result::ToString() const
	{
		String out;
		
		if (IsEmpty())
		{
			out = "Empty result";
		}
		else
		{
			out += "-------------------------------------------------\n";

			for (auto& child : children)
				out += StackFrameToString(&child, 1);

			if (logs.Count())
			{
				for (auto& log : logs)
				{
					String logString = log.ToString();
					out += logString + "\n";
				}
			}
			out += "-------------------------------------------------\n";
		}

		return out;
	}

	void Result::Clear()
	{
		children.Clear();
		logs.Clear();
	}

	bool Result::IsEmpty() const
	{
		return children.Count() == 0 && logs.Count() == 0;
	}

	Result::operator bool() const
	{
		return !IsEmpty();
	}

	Result& Result::JoinResults(Result& result) noexcept
	{
		return JoinResults(std::move(result));		
	}

	Result& Result::JoinResults(Result&& result) noexcept
	{
		if (result.IsEmpty())
			return *this;

		logs.Append(std::move(result.logs));
		children.Append(std::move(result.children));

		return *this;		
	}

	Result& Result::AddStackFrame(StackFrame frame, String report) &
	{
		auto old = std::move(children);
		auto& frameData = children.Add();
		frameData.frame = std::move(frame);
		frameData.report = std::move(report);
		frameData.logs = std::move(logs);
		frameData.children = std::move(old);

		return *this;
	}

	Result& Result::AddStackFrame(StackFrame frame) &
	{
		if (!IsEmpty())
		{
			auto old = std::move(children);
			auto& frameData = children.Add();
			frameData.frame = std::move(frame);
			frameData.report = "";
			frameData.logs = std::move(logs);
			frameData.children = std::move(old);
		}

		return *this;
	}

	Result Result::AddStackFrame(StackFrame frame, String report)&&
	{
		return std::move(AddStackFrame(std::move(frame), std::move(report)));
	}
	Result Result::AddStackFrame(StackFrame frame) &&
	{
		return std::move(AddStackFrame(std::move(frame)));
	}

	Result& Result::operator+=(Result& result) noexcept
	{
		return JoinResults(result);
	}
	Result& Result::operator+=(Result&& result) noexcept
	{
		return JoinResults(result);
	}
	
	Result& Result::operator=(Result&& result) noexcept	
	{		
		if (this == &result)
			return *this;

		if (!IsEmpty())
			Logger::ProcessResult(std::move(*this)); 
		logs = std::move(result.logs);
		children = std::move(result.children);		

		return *this;
	}			
	
	Result& Result::operator+=(Log b)
	{		
		AddLog(std::move(b));

		return *this;
	}

}