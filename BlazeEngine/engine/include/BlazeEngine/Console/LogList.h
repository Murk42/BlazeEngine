#pragma once
#include "BlazeEngine/Console/Frame.h"
#include "BlazeEngine/Core/Logger.h"

namespace Blaze
{
	namespace Console
	{
		class BLAZE_API LogList
		{
			struct LogCache
			{
				Log log;
				double time = 0;
				uint count = 0;
			};

			std::vector<LogCache> logs;

			Frame frame;
		public:
			LogList(const Vec2i& pos, const Vec2i& size);
			LogList(LogList&&) noexcept;
			~LogList();

			void AddLog(const Log& log);
			void Refresh();

			LogList& operator=(LogList&&) noexcept;
		};
	}
}