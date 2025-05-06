#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeView.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

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
		return NodeTreeIterator();
	}
	NodeTreeIterator NodeTreeView::FirstIterator() const
	{
		return NodeTreeIterator(root);
	}
	NodeTreeIterator NodeTreeView::LastIterator() const
	{
		auto it = root;
		while (!it->GetChildren().Empty())
			it = &it->GetChildren().Last();		
		return NodeTreeIterator(it);
	}
	NodeTreeIterator NodeTreeView::BehindIterator() const
	{
		return NodeTreeIterator();
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