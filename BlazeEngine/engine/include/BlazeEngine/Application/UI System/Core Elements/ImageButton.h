#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Application/UI System/Core Elements/Button.h"
#include "BlazeEngine/Application/UI System/Core Elements/Image.h"

namespace Blaze::UI
{
	class ImageButtonManager;

	struct ImageButtonProperties
	{
		Vec2f pos;
		Vec2f size = Vec2f(50, 50);
		UIElement* anchor = nullptr;
		Align localAlign = Align::Center;
		Align anchorAlign = Align::Center;
		bool clickable = true;
		bool active = true;

		float borderWidth = 2;
		float cornerRadius = 3;
		ColorRGBAf normalColor = 0x555555ff;
		ColorRGBAf hoveredColor = 0x494949ff;
		ColorRGBAf pressedColor = 0x434343ff;
		ColorRGBAf borderColor = 0x464646ff;

		Graphics::Core::Texture2D* texture = nullptr;
		Rectf sourceRect = { 0, 0, 30, 30};

		Vec2f imageSize = Vec2f(30, 30);
		Align imageLocalAlign = Align::Center;
		Align imageAnchorAlign = Align::Center;
		Vec2f imageOffset;
		ColorRGBAf imageMask;

		UIEvent pressed;
		UIEvent released;
		UIEvent entered;
		UIEvent left;
	};

	class BLAZE_API ImageButton : public UIElement
	{
		Panel panel;
		Image image;
		Button button;
		ColorRGBAf normalColor;
		ColorRGBAf pressedColor;
		ColorRGBAf hoveredColor;
	public:
		ImageButton();
		~ImageButton();
		
		void SetSize(Vec2f size) override;
		
		void SetActiveFlag(bool active) override;		
		void SetClickableFlag(bool clickable) { button.clickable = clickable; }
		void SetTriggerableFlag(bool triggerable) { button.triggerable = triggerable; }
		void SetDepth(float value) override;

		void SetNormalColor(ColorRGBAf color);
		void SetPressedColor(ColorRGBAf color);
		void SetHoveredColor(ColorRGBAf color);
		void SetBorderColor(ColorRGBAf color) { panel.borderColor = color; }
		void SetBorderWidth(float width) { panel.borderWidth = width; }
		void SetCornerRadius(float radius) { panel.cornerRadius = radius; }

		void SetImageTexture(Graphics::Core::Texture2D* texture) { image.texture = texture; }
		void SetImageSoruceRect(Rectf rect) { image.sourceRect = rect; }
		void SetImageSize(Vec2f size) { image.SetSize(size); }
		void SetImageLocalAlignment(Align align) { image.SetLocalAlignment(align); }
		void SetImageAnchorAlignment(Align align) { image.SetAnchorAlignment(align); }
		void SetImageOffset(Vec2f offset) { image.SetPos(offset); }
		void SetImageMask(ColorRGBAf mask) { image.mask = mask; }

		void AddPressedFunction(const UIEvent& event) { button.pressed += event; }
		void AddReleasedFunction(const UIEvent& event) { button.released += event; }
		void AddEnteredFunction(const UIEvent& event) { button.entered += event; }
		void AddLeftFunction(const UIEvent& event) { button.left += event; }

		ColorRGBAf GetNormalColor() const { return normalColor; }
		ColorRGBAf GetHoveredColor() const { return hoveredColor; }
		ColorRGBAf GetPressedColor() const { return pressedColor; }
		ColorRGBAf GetBorderColor() const { return panel.borderColor; }
		float GetBorderWidth() const { return panel.borderWidth; }
		float GetCornerRadius() const { return panel.cornerRadius; }		

		auto& GetImageElement() { return image; }
		auto& GetImageElement() const { return image; }
		auto& GetPanelElement() { return panel; }
		auto& GetPanelElement() const { return panel; }

		bool IsTriggerable() const { return button.triggerable; }
		bool IsClickable() const { return button.clickable; }

		void SetProperties(const ImageButtonProperties& properties);

		static constexpr const char* typeName = "ImageButton";
		using ManagerType = ImageButtonManager;

		friend class ImageButtonManager;
	};

	class BLAZE_API ImageButtonManager : UIElementManager<ImageButton>
	{
	public:
		void Render(UIElement*) override;
		void Update(UIElement*) override;
	};
}