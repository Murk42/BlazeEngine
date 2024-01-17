#pragma once
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::UI
{				
	class Node;
	class Screen;

	struct NodeTransform
	{
		Vec2f pos;
		Vec2f parentPivot;
		Vec2f pivot;
		Vec2f size;
		float scale = 1.0f;
		float rotation = 0.0f;
	};

	struct NodeTransformUpdatedEvent
	{
		Node* node;
	};
	struct NodePreTransformUpdatedEvent
	{
		Node* node;
	};	

	class Node
	{
	public:	
		VirtualMap<String> otherData;
		Graphics::RenderStream* renderStream;
		mutable EventDispatcher<NodePreTransformUpdatedEvent> preTransformUpdateEventDispatcher;
		mutable EventDispatcher<NodeTransformUpdatedEvent> transformUpdatedEventDispatcher;						

		Node(Node* parent, const NodeTransform& transform);
		virtual ~Node();		

		void SetTransform(const NodeTransform& transform);
		//Marks the nodes transform as dirty, it will be updated only when 
		//a final position, size, scale or rotation is being asked for or
		//the screen updates it
		void UpdateTransform();		
		//Forces the update, doesn't just mark it as dirty, but if it isn't
		//dirty this  function does nothing
		void ForceUpdateTransform();
		inline uint32 GetTransformState() { return transformState; }
				
		ArrayView<Node*> GetChildren() const { return children; }
		Node* GetParent() const { return parent; }
		Screen* GetScreen() const { return screen; }
		inline NodeTransform GetTransform() const { return transform; }		

		Vec2f GetFinalPosition() const;
		Vec2f GetFinalSize() const;
		float GetFinalScale() const;
		float GetFinalRotation() const;
	private:				
		Screen* screen;
		Node* parent;				
		Array<Node*> children;
		mutable uint8 state;

		mutable NodeTransform transform;		
		mutable uint32 transformState;
		mutable float finalScale;
		mutable Vec2f finalSize;
		mutable Vec2f finalPosition;
		mutable float finalRotation;		

		void MakeTransformDirty() const;

		void CalculateTransformUpwards() const;
		//This function should be used only if it is known that all parent transforms aren't dirty
		void CalculateTransformDownwards() const;
		void CalculateTransform() const;
	
		friend class Screen;
	};				
}