#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Time/TimePoint.h"

namespace Blaze
{
	class BLAZE_API TimingTree
	{
	public:
		struct Node
		{
		public:
			uintMem level;
			u8String name;
			ProgramTimePoint startingPoint;
			float time; //In seconds

			Node(Node&& other) noexcept = default;
			Node(uintMem parentIndex, uintMem level, u8String name, ProgramTimePoint startingPoint);

			Node& operator=(Node&& other) noexcept = default;
		private:
			uintMem parentIndex;

			friend class TimingTree;
		};

		TimingTree();
		TimingTree(TimingTree&& other) noexcept = default;
		~TimingTree();

		void Start(u8String name);
		float End();

		inline ArrayView<Node> GetNodes() const { return nodes; }

		TimingTree& operator=(TimingTree&& other) noexcept = default;
	private:
		Array<Node> nodes;
		uintMem parentIndex;
		uintMem level;
	};
}