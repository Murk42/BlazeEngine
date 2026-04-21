#include "pch.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
	NodeFinalTransform CalculateFinalTransform(const NodeTransform& transform, float finalScale, const NodeFinalTransform& parentFinalTransform)
	{
		NodeFinalTransform newFinalTransform;
		
		newFinalTransform.size = transform.size * finalScale;
		newFinalTransform.rotation = transform.rotation + parentFinalTransform.rotation;

		Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
		Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

		Vec2f parentRight = { Math::Cos(parentFinalTransform.rotation), Math::Sin(parentFinalTransform.rotation) };
		Vec2f parentUp = { -parentRight.y, parentRight.x };

		Vec2f offset = -newFinalTransform.size * transform.pivot;
		Vec2f parentRelativePos =
			transform.parentPivot * parentFinalTransform.size + transform.position * finalScale +
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
			.position = Vec2f(0.0f, 0.0f),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(0.0f, 0.0f),
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
		auto CleanNode = [&](Node& node, float parentFinalScale, const NodeFinalTransform& parentFinalTransform, bool dirctCopyTransform)
			{
				float newFinalScale = node.scale * parentFinalScale;
				if (node.finalScale != newFinalScale)
				{
					node.finalScale = newFinalScale;
					node.finalScaleUpdatedEventDispatcher.Call({ node });
				}

				if (node.transformDirty)
				{	
					node.UpdateTransform();
					node.transformDirty = false;
				}

				if (node.finalTransformDirty)
				{
					if (dirctCopyTransform)
					{
						finalTransform = NodeFinalTransform{
							.position = transform.position,
							.size = transform.size,
							.rotation = 0.0f,
							.right = Vec2f(1, 0),
						};
					}
					else
					{
						node.finalTransform = CalculateFinalTransform(node.transform, node.finalScale, parentFinalTransform);
					}
					
					for (auto& child : node.children)
						child.MarkFinalTransformDirty();

					node.finalTransformDirty = false;
					node.finalTransformUpdatedEventDispatcher.Call({ node });
				}
			};
				
		
		CleanNode(*this, 1, { }, true);

		for (auto node = GetNextNodeInTree(); node != nullptr; node = node->GetNextNodeInTree())
			CleanNode(*node, node->parent->finalScale, node->parent->finalTransform, false);
	}
}