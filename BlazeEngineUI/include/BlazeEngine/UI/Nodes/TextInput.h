#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include "BlazeEngine/UI/Graphics/RenderNode.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/QuadRenderUnit.h"

namespace Blaze::UI::Nodes
{
	class TextInput : public InputNode, public RenderNode
	{
	public:
		TextInput();
		TextInput(Node& parent, const NodeTransform& transform);
		TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText = "", u8String text = "");
		~TextInput();		

		void SetText(u8String text);
		void SetEmptyText(u8String emptyText);
		void SetTextStyle(const TextStyle& textStyle);
		void SetMultiline(bool multiline);

		inline u8StringView GetText() { return text; }
		inline u8StringView GetEmptyText() { return emptyText; }
		inline const TextStyle& GetTextStyle() const { return textStyle; }		

		bool PreRender(const RenderContext& renderContext) override;
		RenderUnitBase* GetRenderUnit(uintMem index) override;
	private:
		u8String text;
		u8String emptyText;
		TextStyle textStyle;
		bool multiline : 1 = false;		

		float textWrapWidth = 0.0f;
		Vec2f textBuildSize = { 0.0f, 0.0f };
		bool rendererTypeIDChanged;

		StaticTextRenderUnit textRenderUnit;
		QuadRenderUnit cursorRenderUnit;

		void TransformUpdated(const TransformUpdatedEvent& event);
		EVENT_MEMBER_FUNCTION(TextInput, TransformUpdated, transformUpdatedEventDispatcher);
		
		bool GetFontAtlasData(FontManager::FontAtlasData& fontAtlasData) const;
		void UpdateTextRenderUnitTransform();
		void UpdateRenderUnits();
	};
}