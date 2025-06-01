#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineCore/DataStructures/UnicodeChar.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Common/TextContainerBase.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineGraphics/Text/FontAtlas.h"

namespace Blaze::UI
{
	struct FontStyle
	{
		Font& font;
		uint fontPixelHeight;
		Graphics::FontAtlas atlas;

		FontStyle(Font& font, uint fontPixelHeight, const CharacterSet& characterSet)
			: font(font), fontPixelHeight(fontPixelHeight), atlas(font, fontPixelHeight, characterSet)
		{

		}
	};

	class BLAZE_GRAPHICS_API TextRenderUnitBase :
		public Graphics::RenderUnit
	{
		/*
			TODO
			make sure characterRenderData and characterData are the same size
		*/
	public:
		struct CharacterData
		{
			//Character value
			UnicodeChar character;
			//Size of the character in line transform space. Includes line squishing.
			Vec2f size;
			//Position of the character in line transform space. Includes line squishing.
			Vec2f pos;
			//Index of the line the character is in.
			uint32 lineIndex;

			//bool culled;
			//ColorRGBAf color;
			//
			//Vec2f finalPosition;
			//Vec2f finalRightVector;
			//Vec2f finalUpVector;
			//Vec2f uv1;
			//Vec2f uv2;
		};
		struct CharacterRenderData
		{
			//Wether the character is culled. If it is culled other member variables aren't filled in.
			bool isCulled;
			//Color of the character.
			ColorRGBAf color;
			//Final position of the character.
			Vec2f pos;
			//Right vector of the character.
			Vec2f right;
			//Up vector of the character.
			Vec2f up;
			//UV of the lower left point.
			Vec2f uv1;
			//UV of the upper right point.
			Vec2f uv2;
		};
		struct LineData
		{
			//Position of the line in text transform space.
			Vec2f pos;
			//Size of the line in text transform space.
			Vec2f size;
			//Index of the first character in the line.
			uintMem firstCharacterIndex = 0;
			//Count of characters in the line. This includes the '\n' character, so every line except the last one will have at least one character
			uintMem characterCount = 0;
		};
		struct RenderDataUpdatedEvent
		{
			TextRenderUnitBase* renderUnit;
		};

		TextContainerBase& textContainer;
		EventDispatcher<RenderDataUpdatedEvent> renderDataUpdatedEventDispatcher;

		TextRenderUnitBase(TextContainerBase& textContainer, StringView rendererName);

		//TODO remove this function
		virtual NodeFinalTransform GetFinalTransform() = 0;
		virtual NodeFinalTransform GetCullingTransform() = 0;		
		virtual const FontStyle* GetFontStyle() const = 0;

		virtual bool CleanData() = 0;
		virtual bool CleanRenderData() = 0;

		//Returns character data
		virtual const Array<CharacterData>& GetCharacterData() = 0;
		//Returns character render data.
		virtual const Array<CharacterRenderData>& GetCharacterRenderData() = 0;
		//Returns line data. There always must be at least one line data element, even if it's empty
		virtual const Array<LineData>& GetLineData() = 0;

		bool GetLineIndexUnderPosition(Vec2f localTargetPos, uintMem& lineIndex);
		uintMem GetClosestLineIndex(Vec2f localTargetPos);
		//Finds the closest character to the given screen position in the given line and returns its index.
		uintMem GetClosestCharacterIndex(Vec2f localTargetPos);
		uintMem GetClosestCharacterIndexInLine(Vec2f localTargetPos, uintMem lineIndex);
		//Finds the closest character separation to the given local node space position in the given line and returns its characterLineIndex. 
		//A character separation is any place where a cursor could be placed. This is useful for the 'Up' and 'Down' buttons on the keyboard
		//when traversing text, so that you can find the closes character separation in the line above and put the cursor there.
		uintMem GetClosestCharacterSeparationIndex(Vec2f localTargetPos);
		uintMem GetClosestCharacterSeparationIndexInLine(Vec2f localTargetPos, uintMem lineIndex);

		Vec2f GetCharacterSeparationPosition(uintMem characterIndex);
		Vec2f GetCharacterSeparationPosition(uintMem lineIndex, uintMem lineCharacterIndex);
	};
}