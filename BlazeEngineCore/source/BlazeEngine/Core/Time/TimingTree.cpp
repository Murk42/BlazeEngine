#include "pch.h"
#include "BlazeEngine/Core/Time/TimingTree.h"
#include "BlazeEngine/Core/Time/TimeInterval.h"

namespace Blaze
{
	TimingTree::TimingTree()
		: parentIndex(SIZE_MAX), level(0)
	{	
	}
	TimingTree::~TimingTree()
	{
		if (parentIndex != SIZE_MAX)
			BLAZE_LOG_WARNING("Timing tree node not ended");
	}
	void TimingTree::Start(u8String name)
	{
		nodes.AddBack(Node(parentIndex, level, std::move(name), ProgramTimePoint()));
		parentIndex = nodes.Count() - 1;
		++level;
	}
	float TimingTree::End()
	{
		--level;

		if (parentIndex == SIZE_MAX)
		{
			BLAZE_LOG_ERROR("End() called ona timing tree without a corresponding Start()");
			return 0.0f;
		}

		auto& node = nodes[parentIndex];
		parentIndex = node.parentIndex;

		float time = static_cast<float>((ProgramTimePoint() - node.startingPoint).GetSeconds());
		node.time = time;

		return time;
	}
	TimingTree::Node::Node(uintMem parentIndex, uintMem level, u8String name, ProgramTimePoint startingPoint)
		: parentIndex(parentIndex), level(level), name(std::move(name)), startingPoint(std::move(startingPoint)), time(0)
	{
	}
}