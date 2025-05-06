#pragma once
#include "BlazeEngineGraphics/UI/Core/NodeTreeIterator.h"

namespace Blaze::UI
{	
	//This class is used to iterate over the node tree
	class BLAZE_GRAPHICS_API NodeTreeView
	{
	public:
		NodeTreeView();
		NodeTreeView(const NodeTreeView&);
		//Constructs a view pointing to the first node in the tree in which <node> is found
		NodeTreeView(Node* node);
		NodeTreeView(Node& node);

		inline Node* GetRootNode() const { return root; }

		NodeTreeIterator AheadIterator() const;
		NodeTreeIterator FirstIterator() const;
		NodeTreeIterator LastIterator() const;
		NodeTreeIterator BehindIterator() const;

		NodeTreeView& operator=(const NodeTreeView& other);
	private:
		Node* root;

	};

	NodeTreeIterator begin(NodeTreeView& view);
	NodeTreeIterator end(NodeTreeView& view);
}