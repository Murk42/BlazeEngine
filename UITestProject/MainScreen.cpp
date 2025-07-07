#include "pch.h"
#include "MainScreen.h"
#include "Internet.h"

MainScreen::MainScreen(ResourceManager& resourceManager)
	: Screen(nullptr), resourceManager(resourceManager)
{
	auto fontFace = resourceManager.GetResource<FontFace>("default");

	TextStyle style;
	style.baselineOffset = 0;
	style.color = ColorRGBA(255, 255, 255, 255);
	style.fontFace = fontFace;
	style.fontSize = 64;
	style.strikeThrough = false;
	style.underline = false;

	this->textStyle = resourceManager.LoadResource<TextStyle>("", style);

	FontGlyphRenderers::AntialiasedFontGlyphRenderer glyphRenderer;
	atlas = resourceManager.LoadResource<FontAtlas>("", FontAtlas(style, fontFace->GetAllGlyphsInidices(), glyphRenderer, resourceManager));

	Map<FontAtlasIdentifier, const FontAtlas&> atlases;
	atlases.Insert(style, *atlas);
	text.SetParent(this);	
	text.SetText("Ovo je tekst za testiranje!\nIma vise linija\nfi fl ffi ffl th st ct", { }, FLT_MAX, textStyle, atlases );
}