#pragma once
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include "BlazeEngine/UI/Core/NodeDataMap.h"
#include "BlazeEngine/UI/Core/NodeTransform.h"
#include "BlazeEngine/UI/Core/NodeFinalTransform.h"
#include "BlazeEngine/UI/Core/NodeTreeView.h"

namespace Blaze::UI
{
	class Screen;

	class BLAZE_API Node
	{
	public:
		enum class HitStatus : uint8
		{
			NotHit = 0,
			HitBlocking = 1,
			HitNotBlocking = 2,
		};
		struct TransformUpdatedEvent
		{
			Node& node;
		};
		struct FinalTransformUpdatedEvent
		{
			Node& node;
		};
		struct NodeEnabledStateChangedEvent
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
		struct HitTestInvalidatedEvent
		{
			Node& node;
		};
		NodeDataMap dataMap;

		EventDispatcher<TransformUpdatedEvent> transformUpdatedEventDispatcher;
		EventDispatcher<FinalTransformUpdatedEvent> finalTransformUpdatedEventDispatcher;
		EventDispatcher<NodeEnabledStateChangedEvent> enabledStateChangedEventDispatcher;
		EventDispatcher<SurroundingNodeTreeChangedEvent> surroundingNodeTreeChangedEventDispatcher;
		EventDispatcher<HitTestInvalidatedEvent> hitTestInvalidatedEventDispatcher;

		Node();
		Node(Node& parent, const NodeTransform& transform);
		virtual ~Node();
		
		/*
			\returns Hit test value indicating if the node was hit at the given position in screen node coordinates.
		*/
		virtual HitStatus HitTest(Vec2f screenNodePosition);
		void InvalidateHitTest();

		Node* GetParent() const;
		void SetParent(Node* parent);

		void SetTransform(const NodeTransform& transform);
		const NodeTransform& GetTransform() const;
		void SetFinalTransform(const NodeFinalTransform& newFinalTransform);
		const NodeFinalTransform& GetFinalTransform() const;
		/*
			Sets the transform and marks it as dirty
		*/
		void MarkTransformDirty();
		bool GetTransformDirtyFlag() const;
		void MarkFinalTransformDirty();
		bool GetFinalTransformDirtyFlag() const;

		void Disable();
		void Enable();
		void SetEnabledFlag(bool newEnabledFlag);
		/*
			\returns If the node is enabled returns true, otherwise false. For a node to be enabled all of it's ancestors
			must be enabled and its enabled flag must be set, otherwise it is disabled. Not same as GetNodeEnabledFlag().
		*/
		bool IsEnabled() const;
		/*
			\returns If the node enabled flag is set returns true, otherwise false. The nodes enabled state depends on 
			the enabled flag and all of it's ancestors enabled states.
		*/
		bool GetNodeEnabledFlag() const;

		NodeTreeView GetTree();
		Node* GetNextNodeInTree() const;
		Node* GetPreviousNodeInTree() const;
		ArrayView<Node&> GetChildren() const;
		Screen* GetScreen() const;
	private:
		Screen* screen;

		Node* parent;
		Node* next;
		Node* prev;
		Array<Node&> children;

		NodeTransform transform;
		NodeFinalTransform finalTransform;

		bool enabled : 1; //Stores weather this node is enabled, accounting for parent nodes
		bool enabledFlag : 1; //Stores weather this nodes enabled flag is on or off

		bool transformDirty : 1;
		bool finalTransformDirty : 1;


		void PropagateScreenChange(Screen* newScreen);
		void PropagateScreenChangeEvent(Screen* oldScreen);

		void PropagateEnabledState();
		void PropagateEnabledStateEvent();

		void MarkFinalTransformDirtyDownwards();

		static bool AreNodesOnSameLevel(Node* a, Node* b);

		//Guaranteed to return a non-nullptr value
		static Node* GetNodeSubTreeTail(Node* node);

		friend class Screen;
	};

	inline Node* Node::GetParent() const { return parent; }
	inline const NodeTransform& Node::GetTransform() const { return transform; }
	inline const NodeFinalTransform& Node::GetFinalTransform() const { return finalTransform; }
	inline bool Node::GetTransformDirtyFlag() const { return transformDirty; }
	inline bool Node::GetFinalTransformDirtyFlag() const { return finalTransformDirty; }
	inline void Node::Disable() { SetEnabledFlag(false); }
	inline void Node::Enable() { SetEnabledFlag(true); }
	inline bool Node::IsEnabled() const { return enabled; }
	inline bool Node::GetNodeEnabledFlag() const { return enabledFlag; }
	inline Node* Node::GetNextNodeInTree() const { return next; }
	inline Node* Node::GetPreviousNodeInTree() const { return prev; }
	inline ArrayView<Node&> Node::GetChildren() const { return children; }
	inline Screen* Node::GetScreen() const { return screen; }
}