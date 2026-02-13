#pragma once
#include "BlazeEngine/Core/File/Path.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"

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

		ResourceManager& resourceManager;
		EventDispatcher<DestructionEvent> destructionEventDispatcher;
		EventDispatcher<ScreenTreeChangedEvent> treeChangedEventDispatcher;

		Screen(ResourceManager& resourceManager);
		virtual ~Screen();

		HitStatus HitTest(Vec2f screenPos) override;

		void Update();
	};
}