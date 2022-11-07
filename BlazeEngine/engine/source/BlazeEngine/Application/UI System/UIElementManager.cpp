#include "BlazeEngine/Application/UI System/UIElementManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Application/UI System/UIScene.h"
#include "BlazeEngine/Application/UI System/UIElement.h"

namespace Blaze::UI
{
	UIDebugRenderData UIBaseElementManager::GetDebugRenderData(UIElement* ptr)
	{
		return { { { Rectf(ptr->GetViewportPos(), ptr->GetSize()), 1, false, ColorRGBAf() }}};
	}
	void UIBaseElementManager::Serialize(UIElement* el, BinaryOutputStream& stream)
	{
		String name = el->GetName();

		stream.Write(name.Size());
		for (auto ch : name) stream.Write(ch);

		stream.WriteString(el->GetLayerName());
		stream.WriteVector(el->GetPos());
		stream.WriteVector(el->GetSize());
		stream.Write<uint8>((uint8)el->GetAnchorAlignment());
		stream.Write<uint8>((uint8)el->GetLocalAlignment());
		stream.Write(el->IsActive());
		stream.Write(el->IsSolid());
		stream.Write(el->GetDepth());

		if (el->GetAnchor() != 0)
		{
			name = el->GetAnchor()->GetName();

			stream.Write(name.Size());
			for (auto ch : name) stream.Write(ch);
		}
		else
			stream.Write<size_t>(0);
	}
	void UIBaseElementManager::Deserialize(UIElement* el, BinaryInputStream& stream)
	{				
		Vec2f pos;
		Vec2f size;
		uint8 anchorAlign;
		uint8 localAlign;
		uint8 active;
		uint8 clickable;
		float depth;
		String anchor;		
		
		stream.ReadVector(pos);
		stream.ReadVector(size);
		stream.Read(anchorAlign);
		stream.Read(localAlign);
		stream.Read(active);
		stream.Read(clickable);
		stream.Read(depth);
		stream.ReadString(anchor);

		auto* scene = el->GetScene();

		el->SetPos(pos);
		el->SetSize(size);
		el->SetAnchorAlignment((Align)anchorAlign);
		el->SetLocalAlignment((Align)localAlign);
		el->SetActiveFlag(active);
		el->SetSolidFlag(clickable);
		el->SetDepth(depth);
		el->SetAnchor(scene->GetElementBase(anchor));

		uint eventCount;
		stream.Read(eventCount);
		for (int i = 0; i < eventCount; ++i)
		{
			String name;
			stream.ReadString(name);
			el->sizeChanged += scene->GetEventFunction(name);
		}
	}		
}