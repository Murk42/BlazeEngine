#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UIGraphics
{
	class BLAZE_GRAPHICS_API TextRenderUnitBase :
		public Graphics::RenderUnit
	{
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
			uintMem lineIndex;
		};
		struct CharacterRenderData
		{
			//Wether the character is culled. If it is culled other member variables arent filled in.
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
			//Count of characters in the line.
			uintMem characterCount = 0;		
		};		
		struct RenderDataUpdatedEvent
		{
			TextRenderUnitBase* renderUnit;
		};		
		
		EventDispatcher<RenderDataUpdatedEvent> renderDataUpdatedEventDispatcher;		

		TextRenderUnitBase(StringView rendererName);		
			
		virtual UI::NodeFinalTransform GetFinalTransform() = 0;
		virtual UI::NodeFinalTransform GetCullingTransform() = 0;
		virtual uint GetPixelFontHeight() = 0;

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

		//Find the word in which a character indexed by characterIndex is found
		void FindWord(uintMem characterIndex, uintMem& begin, uintMem& end);
		//Find the line in which a character indexed by characterIndex is found. Lines are separated by '\n', wrapped lines are treated
		//as one line
		void FindLine(uintMem characterIndex, uintMem& begin, uintMem& end);	
	};
}