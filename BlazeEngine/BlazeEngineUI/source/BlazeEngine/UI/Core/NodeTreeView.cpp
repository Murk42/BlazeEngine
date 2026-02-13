#include "pch.h"
#include "BlazeEngine/UI/Core/NodeTreeView.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	NodeTreeView::NodeTreeView()
		: root(nullptr)
	{
	}
	NodeTreeView::NodeTreeView(Node* root)
		: root(root)
	{
	}
	NodeTreeView::NodeTreeView(Node& root)
		: root(&root)
	{
	}
	NodeTreeView::NodeTreeView(const NodeTreeView& other)
		: root(other.root)
	{		
	}
	NodeTreeIterator NodeTreeView::AheadIterator() const
	{
		return NodeTreeIterator(root, true, false);
	}
	NodeTreeIterator NodeTreeView::FirstIterator() const
	{
		return NodeTreeIterator(root, false, false);
	}
	NodeTreeIterator NodeTreeView::LastIterator() const
	{
		auto it = root;
		while (!it->GetChildren().Empty())
			it = &it->GetChildren().Last();		
		return NodeTreeIterator(it, false, false);
	}
	NodeTreeIterator NodeTreeView::BehindIterator() const
	{
		auto it = root;
		while (!it->GetChildren().Empty())
			it = &it->GetChildren().Last();
		return NodeTreeIterator(it, false, true);
	}
	NodeTreeView& NodeTreeView::operator=(const NodeTreeView& other)
	{
		root = other.root;
		return *this;
	}
	NodeTreeIterator begin(NodeTreeView& view) 
	{
		return view.FirstIterator();
	}
	NodeTreeIterator end(NodeTreeView& view) 
	{
		return view.BehindIterator();
	}
}