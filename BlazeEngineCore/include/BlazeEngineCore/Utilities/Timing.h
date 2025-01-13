#pragma once
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/Utilities/TimePoint.h"

namespace Blaze
{
	struct TimingResult
	{
		Map<String, TimingResult> nodes;
		String name;
		TimeInterval time;
	};

	class BLAZE_CORE_API Timing
	{
	public:
		Timing();
		Timing(String name);
		Timing(const Timing&) = default;
		Timing(Timing&&) noexcept;
		~Timing();

		String GetName() const { return name; }		

		void AddNode(TimingResult info);

		TimingResult GetTimingResult() const;

		Timing& operator=(const Timing&) = default;
		Timing& operator=(Timing&&) noexcept;		

		operator TimingResult() const;
	private:
		Map<String, TimingResult> nodes;

		String name;				
		TimePoint timePoint;
	};	
}