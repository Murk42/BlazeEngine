#pragma once

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

		bool operator==(const NodeTransform& other) const = default;		
		bool operator!=(const NodeTransform& other) const = default;		
	};	

	struct NodeFinalTransform
	{
		Vec2f position;
		Vec2f size;
		float scale = 1.0f;
		float rotation = 0.0f;

		Vec2f TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition);
	};

	struct NodeDataMap
	{
	public:
		VirtualMap<String> map;
		
		void SetTypeName(const String& name) { map.Insert<String, true>("type", name); }
		String GetTypeName() 
		{ 
			auto it = map.Find("type"); 
			if (!it.IsNull()) 
				if (String* ptr = it.GetValue<String>()) 
					return *ptr; 
			return String(); 
		}
	};

	class Node
	{
	public:	
		struct TransformUpdatedEvent
		{
			Node* node;			
		};
		struct FinalTransformUpdatedEvent
		{
			Node* node;
			NodeFinalTransform finalTransform;
		};		
		NodeDataMap dataMap;
		
		mutable EventDispatcher<TransformUpdatedEvent> transformUpdatedEventDispatcher;		
		mutable EventDispatcher<FinalTransformUpdatedEvent> finalTransformUpdatedEventDispatcher;						

		Node();
		virtual ~Node();		

		void SetParent(Node* parent);
		//Sets the transform and marks it as dirty
		void SetTransform(const NodeTransform& transform);
		//Marks the node transform as dirty. Because of this, transformUpdatedEventDispatcher is guaranteed to
		// be called before the next access to the node transform. This doesn't mark the final transform as 
		// dirty, because it will be if a new transform is set, which will be by the user.
		void MarkTransformDirty();
		//Marks the node final transform as dirty. Because of this, finalTransformUpdatedEventDispatcher is 
		// guaranteed to be called before the next access to the final node transform.
		void MarkFinalTransformDirty();
		//Calls transformUpdatedEventDispatcher if the transform is dirty if not, does nothing. Same as 
		// GetTransform() but doesn't return anything.		
		void CleanTransform();
		//Calls finalTransformUpdatedEventDispatcher if the final transform is dirty, if not does nothing. 
		// If the final transform is dirty CleanTransform() will be called beforehand. If the final transform
		// isn't dirty and the transform is, the final transform wont be cleaned! Same as GetFinalTransform() but 
		// doesn't return anything. 
		void CleanFinalTransform();				

		inline uint32 GetTransformState() const { return transformState; }				
		inline ArrayView<Node*> GetChildren() const { return children; }
		inline Node* GetParent() const { return parent; }
		inline Screen* GetScreen() const { return screen; }
		NodeTransform GetTransform();
		NodeFinalTransform GetFinalTransform();		
	private:				
		Screen* screen;
		Node* parent;				
		Array<Node*> children;
		mutable bool finalTransformDirty : 1;
		mutable bool destroyed : 1;		
		mutable bool transformDirty : 1;

		mutable NodeTransform transform;		
		mutable NodeFinalTransform finalTransform;
		mutable uint32 transformState;


		void CalculateFinalTransformUpwards();
		//This function should be used only if it is known that all parent transforms aren't dirty
		void CalculateFinalTransformDownwards();
		//The parent transformDirty should be checked if it is true and updated accordingly
		void CalculateFinalTransform();
	
		friend class Screen;
	};
}