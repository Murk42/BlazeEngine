#pragma once

namespace Blaze::UI
{
	class Node;

	class BLAZE_API NodeTreeIterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = Node;

		NodeTreeIterator();
		NodeTreeIterator(const NodeTreeIterator& other);
		//Constructs a iterator pointing to the first node in the tree in which <node> is found
		NodeTreeIterator(Node* node, bool ahead, bool behind);

		Node* Ptr() const;
		bool IsNull() const;

		Node& operator*() const;
		Node* operator->() const;

		NodeTreeIterator& operator++();
		NodeTreeIterator operator++(int);
		NodeTreeIterator& operator--();
		NodeTreeIterator operator--(int);

		bool operator==(const NodeTreeIterator& other) const;
		bool operator!=(const NodeTreeIterator& other) const;

		operator bool() const;

		NodeTreeIterator& operator=(const NodeTreeIterator& other);
	private:
		Node* ptr;
		bool ahead : 1;
		bool behind : 1;
	};
}