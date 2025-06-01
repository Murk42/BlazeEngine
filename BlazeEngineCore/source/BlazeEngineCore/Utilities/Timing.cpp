#include "pch.h"
#include "BlazeEngineCore/Utilities/Timing.h"
#include "BlazeEngineCore/Utilities/TimeIntervalImpl.h"

namespace Blaze
{	
	Timing::Timing()
		: timePoint(TimePoint::GetCurrentWorldTime()), name()
	{
	}
	Timing::Timing(String name)
		: timePoint(TimePoint::GetCurrentWorldTime()), name(std::move(name))
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
		info.time = TimePoint::GetCurrentWorldTime() - timePoint;
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
		nodes.Insert(info.name, std::move(info));
	}
}