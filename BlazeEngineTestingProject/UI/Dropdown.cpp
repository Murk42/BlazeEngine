#include "Dropdown.h"

namespace Blaze::UI
{
	void Dropdown::UpdateState()
	{		
		float x = 0 + 16 + 0 + text.GetSize().x + 5;
		float y = 0 + 16 + 0;

		button.SetSize({ 16 + 0 + text.GetSize().x, 16});

		if (open && list != nullptr)
		{
			y += list->GetSize().y + 5;
			x = std::max(list->GetSize().x + 15, x);

			image.texture = openTexture;
			image.sourceRect = openSource;
		}
		else
		{
			image.texture = closedTexture;
			image.sourceRect = closedSource;
		}

		UIElement::SetSize(Vec2f(x, y));
	}
	void Dropdown::AttachedToManager()
	{
		auto* manager = GetManager();
		uint layer = GetLayer();
		manager->AddElement(&text, layer);
		manager->AddElement(&button, layer);
		manager->AddElement(&image, layer);
	}
	Dropdown::Dropdown()
		: open(false), openTexture(nullptr), closedTexture(nullptr), list(nullptr)
	{
		image.SetAnchor(this);
		image.SetRect({ 0, -5, 16, 16 });
		image.SetAnchorAlignment(Align::TopLeft);
		image.SetLocalAlignment(Align::TopLeft);
		button.SetAnchor(&image);
		button.SetAnchorAlignment(Align::Left);
		button.SetLocalAlignment(Align::Left);
		button.SetSize(image.GetSize());

		text.SetAnchor(&image);
		text.SetLocalAlignment(Align::Left);
		text.SetAnchorAlignment(Align::Right);
		text.SetPos({ 0, 0 });
		text.fontSize = 18;		
		text.sizeChanged += [&]() { UpdateState(); };
		
		button.pressed += [&]() {
			open = !open;
			if (list != nullptr)
				list->SetActiveFlag(open);
			UpdateState();
		};

		UpdateState();
	}
	void Dropdown::SetFontResolution(FontResolution* res)
	{
		text.SetFontResolution(res);
	}
	void Dropdown::SetLabel(StringViewUTF8 name)
	{
		text.SetText(name);
	}
	void Dropdown::SetImages(Graphics::Core::Texture2D* open, Rectf src1, Graphics::Core::Texture2D* closed, Rectf src2)
	{
		openTexture = open;
		openSource = src1;
		closedTexture = closed;
		closedSource = src2;
		UpdateState();
	}	

	void Dropdown::SetSize(Vec2f size)
	{
	}

	void Dropdown::SetRect(Rectf rect)
	{
		SetPos(rect.pos);
	}

	void Dropdown::SetDepth(float depth)
	{
		text.SetDepth(depth);
		image.SetDepth(depth);
		if (list)
			list->SetDepth(depth);

		UIElement::SetDepth(depth);
	}

	void Dropdown::SetClickableFlag(bool clickable)
	{
		text.SetClickableFlag(clickable);
		image.SetClickableFlag(clickable);
		if (list)
			list->SetClickableFlag(clickable);

		UIElement::SetClickableFlag(clickable);
	}

	void Dropdown::SetList(List* list)
	{
		this->list = list;
		list->SetAnchor(this);
		list->SetAnchorAlignment(Align::TopLeft);
		list->SetLocalAlignment(Align::TopLeft);
		list->SetPos({ 15, -(5 + 16 + 5) });
		list->SetActiveFlag(open);
		list->sizeChanged += [&]() { if (open) UpdateState(); };
	}
	UIElementParsingData DropdownManager::GetElementParsingData()
	{
		UIElementParsingData data = UIBaseElementManager::GetElementParsingData();
		
		data.AddMember<Dropdown, String>("openTexture",
			[](UIScene& scene, Dropdown& el, const String& name) {
				el.openTexture = scene.GetResourceManager()->GetResource<Graphics::Core::Texture2D>(name);
			}, nullptr);
		data.AddMember<Dropdown, String>("closedTexture",
			[](UIScene& scene, Dropdown& el, const String& name) {
				el.closedTexture = scene.GetResourceManager()->GetResource<Graphics::Core::Texture2D>(name);
			}, nullptr);
		data.AddMember<Dropdown, Rectf>("openTextureRect", &Dropdown::openSource);
		data.AddMember<Dropdown, Rectf>("closedTextureRect", &Dropdown::closedSource);
		data.AddMember<Dropdown, StringUTF8>("text", &Dropdown::SetLabel, &Dropdown::GetLabel);
		data.AddMember<Dropdown, String>("font",
			[](UIScene& scene, Dropdown& dropdown, const String& name) {
				dropdown.SetFontResolution(scene.GetResourceManager()->GetResource<FontResolution>(name));
			}, nullptr);
		
		data.AddMember<Dropdown, String>("list",
			[](UIScene& scene, Dropdown& dropdown, const String& name) {
				auto el = scene.GetElement(name);

				if (el.first == nullptr)
					Result(Log(LogType::Warning, BLAZE_FILE_NAME, "Dropdown::list setter", BLAZE_FILE_LINE,
						"Blaze Engine", "Invalid element name."));
				else if (el.second != scene.GetManager().GetElementTypeRegistry().GetElementTypeIndex("List"))
					Result(Log(LogType::Warning, BLAZE_FILE_NAME, "Dropdown::list setter", BLAZE_FILE_LINE,
						"Blaze Engine", "Invalid element type. Expected a list"));
				else
					dropdown.SetList((UI::List*)el.first);
			}, nullptr);

		return data;
	}

}