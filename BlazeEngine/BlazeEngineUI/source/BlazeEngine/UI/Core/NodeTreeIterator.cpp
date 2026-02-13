#include "pch.h"
#include "BlazeEngine/UI/Core/NodeTreeIterator.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	NodeTreeIterator::NodeTreeIterator()
		: ptr(nullptr)
	{
	}
	NodeTreeIterator::NodeTreeIterator(const NodeTreeIterator& other)
		: ptr(other.ptr), ahead(other.ahead), behind(other.behind)
	{
	}
	NodeTreeIterator::NodeTreeIterator(Node* ptr, bool ahead, bool behind)
		: ptr(ptr), ahead(ahead), behind(behind)
	{
	}
	Node* NodeTreeIterator::Ptr() const
	{
		return ahead || behind ? nullptr : ptr;
	}
	bool NodeTreeIterator::IsNull() const
	{
		return ptr == nullptr || ahead || behind;
	}
	Node& NodeTreeIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr || ahead || behind)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return *ptr;
	}
	Node* NodeTreeIterator::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr || ahead || behind)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return ptr;
	}
	NodeTreeIterator& NodeTreeIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr || behind)
			BLAZE_LOG_FATAL("Incrementing a invalid iterator");
#endif	
		if (ahead)
			ahead = false;
		else
		{
			auto newPtr = ptr->GetNextNodeInTree();

			if (newPtr == nullptr)
				behind = true;
			else
				ptr = newPtr;
		}

		return *this;
	}
	NodeTreeIterator NodeTreeIterator::operator++(int)
	{
		NodeTreeIterator old = *this;

		operator++();

		return old;
	}
	NodeTreeIterator& NodeTreeIterator::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr || ahead)
			BLAZE_LOG_FATAL("Decrementing a invalid iterator");
#endif	

		if (behind)
			behind = false;
		else
		{
			auto newPtr = ptr->GetPreviousNodeInTree();

			if (newPtr == nullptr)
				ahead = true;
			else
				ptr = newPtr;
		}

		return *this;
	}
	NodeTreeIterator NodeTreeIterator::operator--(int)
	{
		NodeTreeIterator old = *this;

		operator--();

		return old;
	}
	bool NodeTreeIterator::operator==(const NodeTreeIterator& other) const
	{
		return ptr == other.ptr && ahead == other.ahead && behind == other.behind;
	}
	bool NodeTreeIterator::operator!=(const NodeTreeIterator& other) const
	{
		return ptr != other.ptr || ahead != other.ahead || behind != other.behind;
	}
	NodeTreeIterator::operator bool() const
	{
		return !IsNull();
	}
	NodeTreeIterator& NodeTreeIterator::operator=(const NodeTreeIterator& other)
	{
		ptr = other.ptr;
		ahead = other.ahead;
		behind = other.behind;

		return *this;
	}	
}