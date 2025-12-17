#include "pch.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI
{
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
}