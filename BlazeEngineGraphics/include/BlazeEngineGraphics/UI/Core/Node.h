#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Core/NodeDataMap.h"
#include "BlazeEngineGraphics/UI/Core/NodeTransform.h"
#include "BlazeEngineGraphics/UI/Core/NodeFinalTransform.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeView.h"

namespace Blaze::UI
{				
	class Screen;

	class BLAZE_GRAPHICS_API Node
	{
	public:	
		struct TransformUpdatedEvent
		{
			Node& node;						
		};
		struct FinalTransformUpdatedEvent
		{
			Node& node;			
		};		
		struct EnabledStateUpdatedEvent
		{
			Node& node;			
		};
		struct SurroundingNodeTreeChangedEvent
		{
			enum class Type
			{				
				ChildRemoved,				
				ChildAdded,				
				ParentChanged,				
				ScreenChanged
			};

			Type type;
			Node& node;
			//Depends on the even type:
			//ChildRemoved - the removed child
			//ChildAdded - the added child
			//ParentChanged - the old parent
			//ScreenChanged - the old screen
			Node* otherNode;
		};
		NodeDataMap dataMap;
		
		EventDispatcher<TransformUpdatedEvent> transformUpdatedEventDispatcher;
		EventDispatcher<FinalTransformUpdatedEvent> finalTransformUpdatedEventDispatcher;
		EventDispatcher<EnabledStateUpdatedEvent> enabledStateUpdatedEventDispatcher;
		EventDispatcher<SurroundingNodeTreeChangedEvent> surroundingNodeTreeChangedEventDispatcher;

		Node();
		virtual ~Node();		

		virtual int HitTest(Vec2f screenPosition);		

		void SetParent(Node* parent);
		//Sets the transform and marks it as dirty
		void SetTransform(const NodeTransform& transform);
		//Marks the node transform as dirty. Because of this, transformUpdatedEventDispatcher is guaranteed to
		// be called before the next access to the node transform. This doesn't mark the final transform as 
		// dirty, because it will be if a new transform is set, which will be by the user.
		void MarkTransformDirty();
		//Marks the node final transform as dirty. Because of this, finalTransformUpdatedEventDispatcher is 
		// guaranteed to be called before the next access to the node final transform.
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
		
		//inline ArrayView<Node&> GetChildren() const { return children; }

		inline Node* GetParent() const { return parent; }
		inline Node* GetNextNodeInTree() const { return next; }
		inline Node* GetPreviousNodeInTree() const { return prev; }

		inline ArrayView<Node&> GetChildren() const { return children; }

		inline Screen* GetScreen() const { return screen; }
		//Not same as GetNodeEnabledFlag(). This returns if the node is enabled, accounting if the parent node is enabled or not. If the parent noode
		//is not enabled neither will be this one.
		inline bool IsEnabled() const { return parentEnabled; }
		//Not same as IsEnabled(). This returns if the enabled flag on this node is true or false, without being effected by the parents nodes state
		inline bool GetNodeEnabledFlag() const { return enabled; }
		inline bool IsTransformDirty() const { return transformDirty; }
		inline bool IsFinalTransformDirty() const { return finalTransformDirty; }
		
		NodeTreeView GetTree();
		NodeTransform GetTransform();
		NodeFinalTransform GetFinalTransform();		
	private:				
		Screen* screen;
		Node* parent;		
		Node* next;
		Node* prev;		

		Array<Node&> children;

		bool finalTransformDirty : 1;		
		bool transformDirty : 1;
		bool parentEnabled : 1;
		bool enabled : 1;
		bool insideTransformUpdatedEventHandler : 1;
		bool insideFinalTransformUpdatedEventHandler : 1;

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

		static bool AreNodesOnSameLevel(Node* a, Node* b);

		//Guaranteed to return a non-nullptr value
		static Node* GetNodeSubTreeTail(Node* node);
	
		friend class Screen;
	};
}