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
		struct TransformUpdatedEvent
		{
			Node& node;
		};
		struct FinalTransformUpdatedEvent
		{
			Node& node;
		};
		struct EnabledStateChangedEvent
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
		EventDispatcher<EnabledStateChangedEvent> enabledStateChangedEventDispatcher;
		EventDispatcher<SurroundingNodeTreeChangedEvent> surroundingNodeTreeChangedEventDispatcher;

		Node();
		Node(Node& parent, const NodeTransform& transform);
		virtual ~Node();

		/*
			\returns Value greater than 0 if the node is hit and blocks further hits, less than 0 if it is hit
			and doesn't block further hits and 0 if it is not hit.
		*/
		virtual int HitTest(Vec2f screenPosition);

		void SetParent(Node* parent);
		/*
			Sets the transform and marks it as dirty
		*/
		void SetTransform(const NodeTransform& transform);
		/*
			Marks the node transform as dirty. Because of this, transformUpdatedEventDispatcher is guaranteed to
			be called before the next access to the node transform. This doesn't mark the final transform as
			dirty, because it will be if a new transform is set in the user event handler.

			This is useful if the transform depends on something external that is relatively computationally
			heavy (for example determining the size needed for text to fit). In this case the user can mark
			the transform as dirty when the external thing changes, and then set the transform in the event
			handler when the transform is actually needed. This way the transform is only calculated when
			needed.
		*/
		void MarkTransformDirty();
		/*
			Marks the node final transform as dirty. Because of this, finalTransformUpdatedEventDispatcher is
			guaranteed to be called before the next access to the node final transform.
		*/
		void MarkFinalTransformDirty();
		/*
			Same as GetTransform() but doesn't return anything.
		*/
		void CleanTransform();
		/*
			Same as GetFinalTransform() but doesn't return anything.
		*/
		void CleanFinalTransform();

		void SetEnabledFlag(bool newEnabledFlag);
		inline void Disable() { SetEnabledFlag(false); }
		inline void Enable() { SetEnabledFlag(true); }

		inline Node* GetParent() const { return parent; }
		inline Node* GetNextNodeInTree() const { return next; }
		inline Node* GetPreviousNodeInTree() const { return prev; }
		inline ArrayView<Node&> GetChildren() const { return children; }
		inline Screen* GetScreen() const { return screen; }
		/*
			\returns Returns if the node is enabled, accounting if the parent node is enabled or not. If the
			parent node is not enabled neither will be this one. Not same as GetNodeEnabledFlag().
		*/
		inline bool IsEnabled() const { return enabled; }
		/*
			\returns This returns if the enabled flag on this node is true or false, without being effected by
			the parent node enabled state. Not same as IsEnabled().
		*/
		inline bool GetNodeEnabledFlag() const { return enabledFlag; }
		inline bool IsTransformDirty() const { return transformChanged; }
		inline bool IsFinalTransformDirty() const { return finalTransformChanged; }

		NodeTreeView GetTree();
		/*
			If the transform is dirty calls the transformUpdatedEventDispatcher and cleans the transform.
			\returns The latest set transform
		*/
		NodeTransform GetTransform();
		/*
			If the final transform is dirty calls the finalTransformUpdatedEventDispatcher and cleans the
			final transform. If the transform (not final) is dirty, then it is cleaned.
			\returns The calculated final transform.
		*/
		NodeFinalTransform GetFinalTransform();
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

		bool finalTransformChanged : 1;
		bool transformChanged : 1;
		bool enabledStateChanged : 1; //Used to tell wether the event dispatcher should be notified after the enabled state has changed

		bool deferChanges : 1;

		bool containsDeferredEnabledFlag : 1;
		bool containsDeferredTransform : 1;
		bool containsDeferredParent : 1;

		bool deferredEnabledFlag : 1;
		NodeTransform deferredTransform;
		Node* deferredParent;

		//The parent transformDirty should be checked if it is true and updated accordingly
		static NodeFinalTransform CalculateFinalTransform(const NodeTransform& transform, const NodeFinalTransform& parentFinalTransform);

		void PropagateScreenChange(Screen* newScreen);
		void PropagateScreenChangeEvent(Screen* oldScreen);

		void PropagateEnabledState();
		void PropagateEnabledStateEvent();

		void StartDeferPeriod();
		void EndDeferPeriod();

		static bool AreNodesOnSameLevel(Node* a, Node* b);

		//Guaranteed to return a non-nullptr value
		static Node* GetNodeSubTreeTail(Node* node);
	};
}