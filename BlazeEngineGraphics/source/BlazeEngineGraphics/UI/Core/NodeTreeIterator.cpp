#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeIterator.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI
{
	NodeTreeIterator::NodeTreeIterator()
		: ptr(nullptr)
	{
	}
	NodeTreeIterator::NodeTreeIterator(const NodeTreeIterator& other)
		: ptr(other.ptr)
	{
	}
	NodeTreeIterator::NodeTreeIterator(Node* ptr)
		: ptr(ptr)
	{
	}
	Node* NodeTreeIterator::Ptr() const
	{
		return ptr;
	}
	bool NodeTreeIterator::IsNull() const
	{
		return ptr == nullptr;
	}
	Node& NodeTreeIterator::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_GRAPHICS_FATAL("Dereferencing a null iterator");
#endif

		return *ptr;
	}
	Node* NodeTreeIterator::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_GRAPHICS_FATAL("Dereferencing a null iterator");
#endif

		return ptr;
	}
	NodeTreeIterator& NodeTreeIterator::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_GRAPHICS_FATAL("Incrementing a null iterator");
#endif	

		ptr = ptr->GetNextNodeInTree();

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
		if (ptr == nullptr)
			BLAZE_ENGINE_GRAPHICS_FATAL("Decrementing a null iterator");
#endif	

		ptr = ptr->GetPreviousNodeInTree();

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
		return ptr == other.ptr;
	}
	bool NodeTreeIterator::operator!=(const NodeTreeIterator& other) const
	{
		return ptr != other.ptr;
	}
	NodeTreeIterator::operator bool() const
	{
		return ptr != nullptr;
	}
	NodeTreeIterator NodeTreeIterator::operator=(const NodeTreeIterator& other)
	{
		ptr = other.ptr;

		return *this;
	}	
}