#pragma once

namespace Blaze::UI
{
	class TextDataBase
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
		struct LineData
		{
			//Position of the line in text transform space.
			Vec2f pos;
			//Index of the first character in the line.
			uintMem firstCharacterIndex = 0;
			//Count of characters in the line.
			uintMem characterCount = 0;
			//Horizontal scale factor of the whole line. The line might be squished if the TextHorizontalLayout is set to Squish
			float scaling = 1.0f;
			//Size of the line in text transform space. Includes line squishing.
			Vec2f size;
		};
	};
}