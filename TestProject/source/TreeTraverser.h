#pragma once
#include <concepts>

template<typename I, typename T>
concept TreeTraverserInterfrace = requires (const T & value, uintMem index) {
	{ I::Count(value) } -> std::same_as<uintMem>;
	{ I::GetChild(value, index) } -> std::same_as<T>;
};

template<typename T, TreeTraverserInterfrace<T> I>
class TreeTraverser
{
	struct Iterator
	{
		T value;
		uint childIndex;
	};

	Iterator stackStaticMemory[128];
	Iterator* stackDynamicMemory;

	uintMem stackMemorySize;

	Iterator* stack;
	Iterator* it;
public:
	TreeTraverser()
		: stackDynamicMemory(nullptr), stackMemorySize(0), stack(nullptr), it(nullptr)
	{
	}
	~TreeTraverser()
	{
		if (stack != stackStaticMemory)
			delete[] stack;
	}
	void Setup(T root)
	{
		if (stack != stackStaticMemory)
			delete[] stack;

		stackDynamicMemory = nullptr;
		stack = stackStaticMemory;
		stackMemorySize = _countof(stackStaticMemory);
		stack = stackStaticMemory;
		it = stack;

		*it = { root, 0 };
	}
	void Advance()
	{
		if (it == nullptr)
			return;

		while (it->childIndex == I::Count(it->value))
		{
			if (it == stack)
			{
				it = nullptr;
				return;
			}

			--it;
		}

		Iterator* old = it;
		++it;

		if (it - stack == stackMemorySize)
		{
			Iterator* old = stack;
			stack = new Iterator[stackMemorySize + 128];
			memcpy(stack, old, sizeof(Iterator) * stackMemorySize);
			it = stack + stackMemorySize;

			if (old != stackStaticMemory)
				delete[] old;

			stackMemorySize += 128;
		}

		*it = { I::GetChild(old->value, old->childIndex), 0 };

		++old->childIndex;
	}
	T GetCurrent()
	{
		if (it == nullptr)
			return T();

		return it->value;
	}
	bool Finished()
	{
		return it == nullptr;
	}
};