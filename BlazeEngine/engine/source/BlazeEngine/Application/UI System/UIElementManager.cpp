#include "BlazeEngine/Application/UI System/UIElementManager.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

namespace Blaze::UI
{
	UIElementParsingData UIBaseElementManager::GetElementParsingData()
	{
		UIElementParsingData data;

		data.AddMember<UIElement, Vec2f>("pos", &UIElement::SetPos, &UIElement::GetPos);
		data.AddMember<UIElement, Vec2f>("size", &UIElement::SetSize, &UIElement::GetSize);
		data.AddMember<UIElement, float>("depth", &UIElement::SetDepth, &UIElement::GetDepth);
		data.AddMember<UIElement, bool>("clickable", &UIElement::SetClickableFlag, &UIElement::IsClickable);
		data.AddMember<UIElement, bool>("active", &UIElement::SetActiveFlag, &UIElement::IsActive);
		data.AddMember<UIElement, Align>("localAlignment", &UIElement::SetLocalAlignment, &UIElement::GetLocalAlignment);
		data.AddMember<UIElement, Align>("anchorAlignment", &UIElement::SetAnchorAlignment, &UIElement::GetAnchorAlignment);
		data.AddMember<UIElement, String>("anchor",
		    [](UIScene& scene, UIElement& el, const String& name) {
		        el.SetAnchor(scene.GetElement(name).first);
		    },
		    nullptr
		    );
		data.AddMember<UIElement, String>("sizeChanged",
		    [](UIScene& scene, UIElement& el, const String& name) {
		        el.sizeChanged += scene.GetEventFunction(name);
		    }, nullptr);

		return data;
	}
}