#include "pch.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
	NodeFinalTransform CalculateFinalTransform(const NodeTransform& transform, const NodeFinalTransform& parentFinalTransform)
	{
		NodeFinalTransform newFinalTransform;

		newFinalTransform.scale = parentFinalTransform.scale * transform.scale;
		newFinalTransform.size = transform.size * parentFinalTransform.scale;
		newFinalTransform.rotation = transform.rotation + parentFinalTransform.rotation;

		Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
		Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

		Vec2f parentRight = { Math::Cos(parentFinalTransform.rotation), Math::Sin(parentFinalTransform.rotation) };
		Vec2f parentUp = { -parentRight.y, parentRight.x };

		Vec2f offset = -newFinalTransform.size * transform.pivot;
		Vec2f parentRelativePos =
			transform.parentPivot * parentFinalTransform.size + transform.pos * parentFinalTransform.scale +
			parentRelativeRight * offset.x + parentRelativeUp * offset.y;

		newFinalTransform.position =
			parentFinalTransform.position +
			parentRight * parentRelativePos.x + parentUp * parentRelativePos.y;

		newFinalTransform.right = { Math::Cos(newFinalTransform.rotation), Math::Sin(newFinalTransform.rotation) };

		if (std::abs(newFinalTransform.right.x) == 1 || std::abs(newFinalTransform.right.y) == 1)
		{
			newFinalTransform.position.x = Math::Ceil(newFinalTransform.position.x);
			newFinalTransform.position.y = Math::Ceil(newFinalTransform.position.y);
		}

		return newFinalTransform;
	}

	Screen::Screen()
		: Node()
	{
		SetTransform({
			.pos = Vec2f(0.0f, 0.0f),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(0.0f, 0.0f),
			.scale = 1.0f,
			.rotation = 0.0f,
			});
	}
	Screen::~Screen()
	{
		dataMap.map.Clear();

		destructionEventDispatcher.Call({ *this });
	}
	Node::HitStatus Screen::HitTest(Vec2f screenPos)
	{
		return HitStatus::NotHit;
	}
	void Screen::Update()
	{
		bool parentDirty = false;
		Node* grandparent = nullptr;

		auto CleanNode = [&](Node& node, const NodeFinalTransform& parentFinalTransform)
			{
				if (parentDirty)
				{
					if (node.parent == grandparent)
						parentDirty = false;
					else
						node.MarkTransformDirty();
				}

				if (node.transformDirty)
				{
					node.SetFinalTransform(CalculateFinalTransform(node.transform, parentFinalTransform));

					node.transformDirty = false;
					node.transformUpdatedEventDispatcher.Call({ node });

					if (!parentDirty)
					{
						parentDirty = true;
						grandparent = node.parent;
					}

				}

				if (node.finalTransformDirty)
				{
					node.finalTransformDirty = false;
					node.finalTransformUpdatedEventDispatcher.Call({ node });
				}
			};

		
		CleanNode(*this, { });

		for (auto node = GetNextNodeInTree(); node != nullptr; node = node->GetNextNodeInTree())
			CleanNode(*node, node->parent->finalTransform);
	}
}