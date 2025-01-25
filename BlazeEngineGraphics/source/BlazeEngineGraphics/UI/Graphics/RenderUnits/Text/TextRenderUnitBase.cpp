#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"

namespace Blaze::UI
{
	TextRenderUnitBase::TextRenderUnitBase(StringView rendererName) :
		RenderUnit(rendererName)
	{
	}
	bool TextRenderUnitBase::GetLineIndexUnderPosition(Vec2f localTargetPos, uintMem& lineIndex)
	{		
		auto& lineData = GetLineData();
		for (lineIndex = 0; lineIndex < lineData.Count(); ++lineIndex)
		{
			auto& line = lineData[lineIndex];
			Vec2f p2 = line.pos + line.size;

			if (localTargetPos.x > line.pos.x && localTargetPos.x < p2.x &&
				localTargetPos.y > line.pos.y && localTargetPos.y < p2.y)
				return true;
		}

		return false;
	}
	uintMem TextRenderUnitBase::GetClosestLineIndex(Vec2f localTargetPos)
	{
		uint lineIndex;

		auto& lineData = GetLineData();
		for (lineIndex = 0; lineIndex < lineData.Count() - 1; ++lineIndex)
			if (localTargetPos.y > (lineData[lineIndex].pos.y + lineData[lineIndex + 1].pos.y + lineData[lineIndex + 1].size.y) / 2)
				break;

		return lineIndex;
	}		
	uintMem TextRenderUnitBase::GetClosestCharacterIndex(Vec2f localTargetPos)
	{
		auto& characterData = GetCharacterData();
		auto& lineData = GetLineData();

		uintMem lineIndex;

		for (lineIndex = 0; lineIndex < lineData.Count() - 1; ++lineIndex)
			if (localTargetPos.y > (lineData[lineIndex].pos.y + lineData[lineIndex + 1].pos.y + lineData[lineIndex + 1].size.y) / 2)
				break;

		uintMem characterBegin = lineData[lineIndex].firstCharacterIndex;
		uintMem characterEnd = lineData[lineIndex].firstCharacterIndex + lineData[lineIndex].characterCount;

		uintMem indexOut;
		for (indexOut = characterBegin; indexOut < characterEnd - 1; ++indexOut)
			if (localTargetPos.x < (characterData[indexOut].pos.x + characterData[indexOut].size.x + characterData[indexOut + 1].pos.x) / 2)
				break;

		return indexOut;
	}
	uintMem TextRenderUnitBase::GetClosestCharacterIndexInLine(Vec2f localTargetPos, uintMem lineIndex)
	{
		auto& characterData = GetCharacterData();
		auto& lineData = GetLineData();

		uintMem characterBegin = lineData[lineIndex].firstCharacterIndex;
		uintMem characterEnd = lineData[lineIndex].firstCharacterIndex + lineData[lineIndex].characterCount;

		uintMem indexOut;
		for (indexOut = characterBegin; indexOut < characterEnd - 1; ++indexOut)
			if (localTargetPos.x < (characterData[indexOut].pos.x + characterData[indexOut].size.x + characterData[indexOut + 1].pos.x) / 2)
				break;

		return indexOut;
	}
	uintMem TextRenderUnitBase::GetClosestCharacterSeparationIndex(Vec2f localTargetPos)
	{
		auto& lineData = GetLineData();
		auto& characterData = GetCharacterData();

		if (lineData.Empty() || characterData.Empty())
			return 0;

		uintMem lineIndex;
		
		for (lineIndex = 0; lineIndex < lineData.Count() - 1; ++lineIndex)
			if (localTargetPos.y > (lineData[lineIndex].pos.y + lineData[lineIndex + 1].pos.y + lineData[lineIndex + 1].size.y) / 2)
				break;

		uintMem characterBegin = lineData[lineIndex].firstCharacterIndex;
		uintMem characterEnd = lineData[lineIndex].firstCharacterIndex + lineData[lineIndex].characterCount;

		if (characterBegin == characterEnd)
			return 0;

		uintMem characterIndex;

		for (characterIndex = characterBegin; characterIndex < characterEnd - 1; ++characterIndex)
			if (localTargetPos.x < characterData[characterIndex].pos.x + characterData[characterIndex].size.x / 2)
				break;

		return characterIndex;
	}
	uintMem TextRenderUnitBase::GetClosestCharacterSeparationIndexInLine(Vec2f localTargetPos, uintMem lineIndex)
	{
		auto& lineData = GetLineData();
		auto& characterData = GetCharacterData();

		uintMem characterBegin = lineData[lineIndex].firstCharacterIndex;
		uintMem characterEnd = lineData[lineIndex].firstCharacterIndex + lineData[lineIndex].characterCount;

		uintMem characterIndex;

		for (characterIndex = characterBegin; characterIndex < characterEnd - 1; ++characterIndex)
			if (localTargetPos.x < characterData[characterIndex].pos.x + characterData[characterIndex].size.x / 2)
				break;

		return characterIndex;
	}	
	Vec2f TextRenderUnitBase::GetCharacterSeparationPosition(uintMem characterIndex)
	{
		auto& characterData = GetCharacterData();
		auto& lineData = GetLineData();

		auto& line = lineData[characterData[characterIndex].lineIndex];
		uintMem lineCharacterIndex = characterIndex - line.firstCharacterIndex;

		if (lineCharacterIndex == line.characterCount)
			return characterData[characterIndex - 1].pos + Vec2f(characterData[characterIndex - 1].size.x, 0);
		else if (lineCharacterIndex == 0)
			return characterData[characterIndex].pos;
		else
			return (characterData[characterIndex - 1].pos + characterData[characterIndex].pos + Vec2f(characterData[characterIndex - 1].size.x, 0)) / 2;		
	}
	Vec2f TextRenderUnitBase::GetCharacterSeparationPosition(uintMem lineIndex, uintMem lineCharacterIndex)
	{
		auto& characterData = GetCharacterData();
		auto& lineData = GetLineData();

		auto& line = lineData[lineIndex];
		uintMem characterIndex = line.firstCharacterIndex + lineCharacterIndex;

		if (lineCharacterIndex == line.characterCount)
			return characterData[characterIndex - 1].pos + Vec2f(characterData[characterIndex - 1].size.x, 0);
		else if (lineCharacterIndex == 0)
			return characterData[characterIndex].pos;
		else
			return (characterData[characterIndex - 1].pos + characterData[characterIndex].pos + Vec2f(characterData[characterIndex - 1].size.x, 0)) / 2;
	}
	void TextRenderUnitBase::FindWord(uintMem characterIndex, uintMem& begin, uintMem& end)
	{
		auto& characterData = GetCharacterData();

		begin = 0;
		end = 0;

		if (characterData.Empty())
			return;

		enum class WordType {
			AlphaNumeric,
			Numeric,
			Space,
			Unknown,
		};

		WordType wordType = WordType::Unknown;
		for (auto it = characterData.FirstIterator(); it != characterData.BehindIterator(); ++it)
		{
			char value = it->character.Value();

			WordType newWordType = WordType::Unknown;

			if (isalpha(value) || isdigit(value) && wordType == WordType::AlphaNumeric)
				newWordType = WordType::AlphaNumeric;
			else if (isdigit(value))
				newWordType = WordType::Numeric;
			else if (value == ' ')
				newWordType = WordType::Space;
			else
				newWordType = WordType::Unknown;

			bool newWord = wordType != newWordType || wordType == WordType::Unknown;

			if (newWord)
			{
				if (characterIndex < end)
					break;

				begin = end;
			}

			wordType = newWordType;

			++end;

		}

		if (wordType == WordType::Unknown)
			begin = --end;
	}
	void TextRenderUnitBase::FindLine(uintMem characterIndex, uintMem& begin, uintMem& end)
	{
		auto& characterData = GetCharacterData();		

		begin = 0;
		end = 0;		

		characterIndex = std::min(characterIndex, characterData.Count() - 1);

		begin = characterIndex;
		end = characterIndex;

		while (begin != 0)
		{
			if (characterData[begin - 1].character == '\n')
				break;

			--begin;
		}

		while (end != characterData.Count())
		{
			if (characterData[end].character == '\n' || characterData[end].character == '\0')
				break;

			++end;
		}
	}
}