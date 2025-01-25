#pragma once

namespace Blaze::UI
{				
	class Node;
	class Screen;

	struct NodeTransform
	{
		Vec2f pos = Vec2f(0.0f, 0.0f);
		Vec2f parentPivot = Vec2f(0.5f, 0.5f);
		Vec2f pivot = Vec2f(0.5f, 0.5f);
		Vec2f size = Vec2f(0.0f, 0.0f);
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
		String GetTypeName() const
		{ 
			auto it = map.Find("type"); 
			if (!it.IsNull()) 
				if (const String* ptr = it.GetValue<String>()) 
					return *ptr; 
			return String(); 
		}
	};

	class Node
	{
	public:	
		struct TransformUpdatedEvent
		{
			Node& node;			
		};
		struct FinalTransformUpdatedEvent
		{
			Node& node;
			NodeFinalTransform finalTransform;
		};		
		struct EnabledStateUpdatedEvent
		{
			Node& node;			
		};
		struct SurroundingNodeTreeChangedEvent
		{
			enum class Type
			{
				//Other node is the removed child
				ChildRemoved,
				//Other node is the added child
				ChildAdded,
				//Other node is the old parent
				ParentChanged,
				//Other node is the new parent
				ScreenChanged
			};

			Type type;
			Node& node;
			Node* otherNode;
		};
		NodeDataMap dataMap;
		
		EventDispatcher<TransformUpdatedEvent> transformUpdatedEventDispatcher;
		EventDispatcher<FinalTransformUpdatedEvent> finalTransformUpdatedEventDispatcher;
		EventDispatcher<EnabledStateUpdatedEvent> enabledStateUpdatedEventDispatcher;
		EventDispatcher<SurroundingNodeTreeChangedEvent> surroundingNodeTreeChangedEventDispatcher;

		Node();
		virtual ~Node();		

		virtual bool HitTest(Vec2f screenPosition);		

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

		void Disable();
		void Enable();
		
		inline ArrayView<Node&> GetChildren() const { return children; }
		inline Node* GetParent() const { return parent; }
		inline Screen* GetScreen() const { return screen; }
		//Not same as GetNodeEnabledFlag(). This returns if the node is enabled, accounting if the parent node is enabled or not. If the parent noode
		//is not enabled neither will be this one.
		inline bool IsEnabled() const { return parentEnabled; }
		//Not same as IsEnabled(). This returns if the enabled flag on this node is true or false, without being effected by the parents nodes state
		inline bool GetNodeEnabledFlag() const { return enabled; }
		NodeTransform GetTransform();
		NodeFinalTransform GetFinalTransform();		
	private:				
		Screen* screen;
		Node* parent;				
		Array<Node&> children;
		bool finalTransformDirty : 1;		
		bool transformDirty : 1;
		bool parentEnabled : 1;
		bool enabled : 1;

		NodeTransform transform;		
		NodeFinalTransform finalTransform;		

		void CalculateFinalTransformUpwards();
		//The parent transformDirty should be checked if it is true and updated accordingly
		void CalculateFinalTransform();
		
		void PropagateScreenChange(Screen* newScreen);
		void PropagateScreenChangeEvent(Screen* oldScreen);	
		
		//Should be called if the parent changes or the enabled flag changes
		void UpdateParentEnabledState();
		//This functions expects that the node has a parent.
		void PropagateEnabledState();
	
		friend class Screen;
	};
}