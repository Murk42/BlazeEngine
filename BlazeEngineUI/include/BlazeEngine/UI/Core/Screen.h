#pragma once
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	class BLAZE_API Screen :
		public Node
	{
	public:
		struct ScreenTreeChangedEvent
		{
			enum class Type
			{
				NodeAdded,
				NodeRemoved,
				NodeMoved
			};

			Type type;
			Node& node;
			Node* oldParent;
		};
		struct DestructionEvent
		{
			Screen& screen;
		};
		
		EventDispatcher<DestructionEvent> destructionEventDispatcher;
		EventDispatcher<ScreenTreeChangedEvent> treeChangedEventDispatcher;		

		Screen();
		virtual ~Screen();

		HitStatus HitTest(Vec2f screenPos) override;

		void Update();
	};
}