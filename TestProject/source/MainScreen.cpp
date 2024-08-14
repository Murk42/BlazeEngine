#include "pch.h"
#include "MainScreen.h"

void MainScreen::Update()
{
	{
		auto transform = explanationText.GetTransform();
		transform.size.x = 60 + (Math::Sin(TimePoint::GetRunTime() * 2) + 1) / 2 * 400;
		explanationText.SetTransform(transform);
	}
	{
		auto transform = panel.GetTransform();
		transform.rotation = TimePoint::GetRunTime();
		panel.SetTransform(transform);
	}
	
}

MainScreen::MainScreen(Resource::ResourceManager& manager) :
	panel(this, { }),
	title(&panel, { }),
	label(&panel, { }), inputBox(&label, { }),
	button(&label, { }),
	explanationText(this, { })
{
	explanationText.SetFont(manager.GetFont("default"));
	explanationText.SetFontHeight(16);	
	explanationText.SetText("This is a UI demo. Enjoy :D. You can write \"rotation = 0.707\" or \"scale = 0.5\" ;)\nsecond row test");	
	explanationText.SetLayoutOptions(UI::Text::HorizontalLayout::Wrap, UI::Text::VerticalLayout::ChangeSize, UI::Text::LineAlign::Left, true);
	explanationText.SetTransform({
		.pos = { 5, -5 },
		.parentPivot = { 0.0f, 1.0f },
		.pivot = { 0.0f, 1.0f},		
		.size = { 300.0f, 30.0f},
		.scale = 1.0f,
		.rotation = 0.0f,
		});

	panel.fillColor = ColorRGBAf(0, 28.0f / 255, 17.0f / 255);
	panel.borderColor = ColorRGBAf(5.0f / 255, 8.0 / 255, 13.0f / 255);
	panel.borderWidth = 2.0f;
	panel.cornerRadius = 10.0f;
	panel.SetTransform({
		.pos = { 0, 0 },
		.parentPivot = { 0.5f, 0.5f },
		.pivot = { 0.5f, 0.5f},
		.size = { 250, 135},
		.scale = 1.0f,
		.rotation = 0.0f,
		});

	title.SetTransform({
		.pos = { 0, -10.0f },
		.parentPivot = { 0.5f, 1.0f },
		.pivot = { 0.5f, 1.0f },
		.size = { 180, 180},
		.scale = 1.0f,
		.rotation = 0,
		});
	title.SetFont(manager.GetFont("default"));
	title.SetFontHeight(20);	
	title.SetText("Console");	

	label.SetTransform({
		.pos = { 10, -60 },
		.parentPivot = { 0.0f, 1.0f },
		.pivot = { 0.0f, 1.0f},
		.size = { 80, 20 },
		.scale = 1.0f,
		.rotation = 0
		});
	label.SetText("Text:");		
	label.SetFontHeight(16);
	label.SetFont(manager.GetFont("default"));

	inputBox.SetTextAlign(UI::InputBox::TextAlign::Left);
	inputBox.GetTextNode().SetFont(manager.GetFont("default"));	
	inputBox.SetNormalColor(0x33333333);
	inputBox.SetHighlightedColor(0x50505033);
	inputBox.SetPressedColor(0x20202033);
	inputBox.SetTransform({
		.pos = { 5, -1 },
		.parentPivot = { 1.0f, 0.5f },
		.pivot = { 0.0f, 0.5f},
		.size = { 180, 15 },
		.scale = 1.0f,
		.rotation = 0
		});
	inputBox.SetEmptyText("Enter name");

	button.GetTextNode().SetFont(manager.GetFont("default"));
	button.GetTextNode().SetFontHeight(12);
	button.GetTextNode().SetText("Write");
	button.SetTransform({
		.pos = { 0, -10 },
		.parentPivot = { 0.0f, 0.0f },
		.pivot = { 0.0f, 1.0f},
		.size = { 80, 20 },
		.scale = 1.0f,
		.rotation = 0
		});
	button.pressedEventDispatcher.AddHandler(buttonPressedEventHandler);
	buttonPressedEventHandler.SetFunction([&](UI::TextButton::PressedEvent event) {
		auto text = inputBox.GetText();

		if (text.Empty())
			return;				

		Console::WriteLine(text);

		auto words = StringParsing::Split(text, ' ');

		if (words.Count() == 3)
		{
			if (words[0] == "rotation" && words[1] == "=")
			{
				float rot;
				if (!StringParsing::Convert(words[2], rot))
				{
					auto t = panel.GetTransform();
					t.rotation = rot;
					panel.SetTransform(t);
				}
			}
			if (words[0] == "scale" && words[1] == "=")
			{
				float scale;
				if (!StringParsing::Convert(words[2], scale))
				{
					auto t = panel.GetTransform();
					t.scale = scale;
					panel.SetTransform(t);
				}
			}
		}

		inputBox.SetText("");
		});
}

inline MainScreen::~MainScreen()
{

}
