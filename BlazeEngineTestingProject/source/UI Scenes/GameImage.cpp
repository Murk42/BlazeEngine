#include "Globals.h"
#include "ElementPropertiesMenu.h"

namespace GameImage
{
	UI::UIElement* gameImage;
	UI::Panel* gameImagePanel;
	UI::Button* gameImageButton;
	UI::TexturedPanel* selectedElementPanel;
		
	UI::UIElement* selectedEl;

	class GameImageEventHandler :
		public EventHandler<Event::MouseMotion>
	{
	public:
		~GameImageEventHandler()
		{
			EventManager::RemoveEventHandler<Event::MouseMotion>(*this);
		}

		void OnEvent(Event::MouseMotion event) override
		{			
			if (Input::GetKeyState(Key::G).down && selectedEl != nullptr)
				selectedEl->SetPos(selectedEl->GetPos() + (Vec2f)event.delta);
		}
	};
	GameImageEventHandler eventHandler;

	void Setup()
	{		
		EventManager::AddEventHandler<Event::MouseMotion>(eventHandler);

		const char* layer = "GameImage";
		
		globals->uiManager.CreateLayer(layer);

		CREATE_ELEMENT(gameImage, layer);
		CREATE_ELEMENT(gameImageButton, layer);
		CREATE_ELEMENT(selectedElementPanel, layer);

		gameImage->UIElement::SetProperties({
			.name = (String)"gameImage", .pos = Vec2f(0, -50), .size = Vec2f(300, 300), .localAlign = Align::Top, .anchorAlign = Align::Top
			});
		gameImage->sizeChanged += [&]()
		{
			gameImageButton->SetSize(gameImage->GetSize());
			globals->gameUIManager.SetViewport((Vec2i)gameImage->GetViewportPos(), (Vec2i)gameImage->GetSize());
		};
		
		gameImageButton->UIElement::SetProperties({
			.anchor = gameImage, .solid = false,
			});
		gameImageButton->pressed += []() {
			UI::UIElement* el = globals->gameUIManager.GetBlockingElement();
			selectedEl = el;
			ElementPropertiesMenu::SelectElement(el);
			if (selectedEl == nullptr)
				selectedElementPanel->SetActiveFlag(false);
			else
			{
				selectedElementPanel->SetActiveFlag(true);
				selectedElementPanel->SetSize(selectedEl->GetSize());
			}
		};

		auto selectedElementTexture = globals->resourceStorage.CreateResource<Graphics::Core::Texture2D>().value;
		selectedElementTexture->Load("assets/selectedElement.png");
		selectedElementTexture->SetSettings({ });

		selectedElementPanel->UIElement::SetProperties({
			.pos = Vec2f(100), .size = Vec2f(300, 300), .localAlign = Align::BottomLeft, .anchorAlign = Align::BottomLeft, .active = false
			});
		selectedElementPanel->SetProperties({
			.texture = selectedElementTexture,
			.mask = (ColorRGBAf)0x00000000,
			.sourceRect = Rectf(0, 0, 64, 64),
			.textureSpacing = UI::TexturedPanelSpacing({ 32, 32, 32, 32}),
			.spacing = UI::TexturedPanelSpacing({ 32, 32, 32, 32}),
			});
	}

	void Update()
	{
		if (selectedEl != nullptr)
		{
			Vec2f elSize = selectedEl->GetSize() + Vec2f(30);
			selectedElementPanel->SetSize(elSize);
			selectedElementPanel->SetPos(selectedEl->GetViewportPos() + (Vec2f)globals->gameUIManager.GetViewport().pos - Vec2f(15));
		}
	}
}