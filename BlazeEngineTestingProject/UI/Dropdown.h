#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "List.h"

namespace Blaze::UI
{
	class DropdownManager;

	class Dropdown : public UIElement
	{
		List* list;
		Text text;
		Image image;
		Button button;
		bool open;

		Graphics::Core::Texture2D* openTexture;
		Graphics::Core::Texture2D* closedTexture;
		Rectf openSource;
		Rectf closedSource;

		void UpdateState();

		void AttachedToManager() override;
	public:
		Dropdown();

		void SetFontResolution(FontResolution* res);
		void SetLabel(StringViewUTF8 name);		
		void SetImages(Graphics::Core::Texture2D* open, Rectf src1, Graphics::Core::Texture2D* closed, Rectf src2);
		
		void SetSize(Vec2f size) override;
		void SetRect(Rectf rect) override;

		void SetDepth(float depth);
		void SetClickableFlag(bool clickable) override;

		void SetList(List* list);
		inline List* GetList() const { return list; }
		inline StringUTF8 GetLabel() const { return text.GetText(); }

		constexpr static const char* typeName = "Dropdown";
		using ManagerType = DropdownManager;

		friend class ManagerType;
	};

	class DropdownManager : public UIElementManager<Dropdown>
	{
	public:
		static UIElementParsingData GetElementParsingData();
	};
}