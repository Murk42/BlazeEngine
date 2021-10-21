#include "BlazeEngine/Console/LogList.h"
#include "BlazeEngine/Utilities/Time.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze
{
	namespace Console
	{
		LogList::LogList(const Vec2i& pos, const Vec2i& size)
			: frame(pos, size)
		{

		}
		LogList::LogList(LogList&& logList) noexcept
			: frame(std::move(logList.frame)), logs(std::move(logList.logs))
		{

		}

		LogList::~LogList()
		{
		}

		void LogList::AddLog(const Log& log)
		{
			bool found = false;
			for (int i = 0; i < logs.size(); ++i)
				if (logs[i].log == log)
				{
					logs[i].time = Time::GetWorldTime();
					++logs[i].count;
					found = true;

					logs.emplace_back(logs[i]);
					logs.erase(logs.begin() + i);					
					break;
				}

			if (!found)
			{
				if (logs.size() == frame.GetSize().y)
					logs.erase(logs.begin());
				auto& lc = logs.emplace_back();
				lc.log = log;
				lc.count = 1;
				lc.time = Time::GetWorldTime();
			}
		}

		void LogList::Refresh()
		{
			frame.Clear();									

			for (int i = 0; i != logs.size(); ++i)
			{							
				const char* prefix = nullptr;
				switch (logs[i].log.type)
				{
				case LogType::Error: prefix = F_RED; break;
				case LogType::Warning: prefix = F_YELLOW; break;
				default: prefix = F_WHITE;
				}

				int t = int(logs[i].time);
				int s = t % 60;
				int m = (t / 60) % 60;
				int h = (t / 3600) % 24;
				String message;
				if (logs[i].count == 1)
					message = prefix + String(format_string, "[%d:%d:%d] ", h, m, s) + logs[i].log.source + ": " + logs[i].log.message;
				else
					message = prefix + String(format_string, "(%d)[%d:%d:%d] ", logs[i].count, h, m, s) + logs[i].log.source + ": " + logs[i].log.message;
							
				frame.Write(message, Vec2i(0, i));				
			}

			frame.Refresh();
		}

		LogList& LogList::operator=(LogList&& list) noexcept
		{
			frame = std::move(list.frame);
			logs = std::move(list.logs);
			return *this;
		}
	}
}