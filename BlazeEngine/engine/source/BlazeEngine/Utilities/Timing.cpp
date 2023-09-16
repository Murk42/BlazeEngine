#include "BlazeEngine/Utilities/Timing.h"

namespace Blaze
{
	const TimingResult& GetInitTimeInfo()
	{
		return engineData->blazeInitTimings;
	}
	Timing::Timing()
		: timePoint(TimePoint::GetWorldTime()), name()
	{
	}
	Timing::Timing(String name)
		: timePoint(TimePoint::GetWorldTime()), name(std::move(name))
	{
	}
	Timing::Timing(Timing&& other) noexcept
		: timePoint(std::move(other.timePoint)), name(std::move(other.name)), nodes(std::move(other.nodes))
	{
	}
	Timing::~Timing()
	{
	}
	TimingResult Timing::GetTimingResult() const
	{
		TimingResult info;
		info.nodes = nodes;
		info.name = name;
		info.time = TimePoint::GetWorldTime() - timePoint;
		return info;
	}
	Timing& Timing::operator=(Timing&& other) noexcept
	{
		nodes = std::move(other.nodes);
		name = std::move(other.name);		
		timePoint = std::move(other.timePoint);
		return *this;
	}
	Timing::operator TimingResult() const
	{
		return GetTimingResult();
	}
	void Timing::AddNode(TimingResult info)
	{		
		nodes.try_emplace(info.name, std::move(info));
	}
}