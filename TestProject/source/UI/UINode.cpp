#include "pch.h"
#include "UINode.h"

namespace UI2
{
	Node::Node(Node* parent, const NodeTransform& transform)
		: parent(parent), screen(parent == nullptr ? (Screen*)this : parent->screen), transform(transform), transformDirty(true)
	{
		if (parent != nullptr)
		{
			parent->children.AddBack(this);
			screen->AddNode(this);
		}		
	}
	Node::~Node()
	{
		if (parent != nullptr)
			screen->RemoveNode(this);
	}
	void Node::SetTransform(const NodeTransform& transform)
	{
		this->transform = transform;

		MakeTransformDirty();
	}
	Vec2f Node::GetFinalPosition()
	{
		CalculateTransformRecursive();
		return finalPosition;
	}
	Vec2f Node::GetFinalSize()
	{
		CalculateTransformRecursive();
		return finalSize;
	}
	float Node::GetFinalRotation()
	{
		CalculateTransformRecursive();
		return finalRotation;
	}
	void Node::MakeTransformDirty()
	{		
		transformDirty = true;

		for (auto& child : children)
			child->MakeTransformDirty();
	}
	void Node::CalculateTransformRecursive()
	{
		if (!transformDirty)
			return;

		if (parent != nullptr)
			parent->CalculateTransformRecursive();

		CalculateTransform();
	}
	Vec2f RotatePoint(Vec2f point, Vec2f around, float angle)
	{
		float cos = Math::Cos(angle);
		float sin = Math::Sin(angle);

		return Vec2f(
			cos * (point.x - around.x) - sin * (point.y - around.y),
			sin * (point.x - around.x) + cos * (point.y - around.y)
		) + around;
	}
	void Node::CalculateTransform()
	{
		if (!transformDirty)
			return;

		transformPreUpdatedEventDipatcher.Call({ .node = this });

		if (parent != nullptr)
		{
			//if (parent->transformDirty)
			//	parent->CalculateTransform();

			finalScale = parent->finalScale * transform.scale;
			finalSize = transform.size * finalScale;
			finalRotation = transform.rotation + parent->finalRotation;

			Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
			Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

			Vec2f parentRight = { Math::Cos(parent->finalRotation), Math::Sin(parent->finalRotation) };
			Vec2f parentUp = { -parentRight.y, parentRight.x };

			Vec2f offset = -finalSize * transform.pivot;
			Vec2f parentRelativePos = 
				transform.parentPivot * parent->finalSize + transform.pos * parent->finalScale +
				parentRelativeRight * offset.x + parentRelativeUp * offset.y;
			
			finalPosition =
				parent->finalPosition +
				parentRight * parentRelativePos.x + parentUp * parentRelativePos.y;

			finalPosition = Vec2f(std::ceil(finalPosition.x), std::ceil(finalPosition.y));
			//matrix =
			//	parent->matrix *
			//	Mat4f::TranslationMatrix(Vec3f(position, 0)) *
			//	Mat4f::RotationMatrixAxisZ(transform.rotation) *
			//	Mat4f::TranslationMatrix(Vec3f(-offset, 0));
		}
		else
		{
			finalScale = transform.scale;
			finalSize = transform.size * finalScale;
			finalRotation = transform.rotation;

			Vec2f position = transform.pos;
			Vec2f offset = finalSize * transform.pivot;

			finalPosition = RotatePoint(transform.pos + transform.pivot * finalSize, transform.pos, finalRotation);

			finalPosition = Vec2f(std::ceil(finalPosition.x), std::ceil(finalPosition.y));

			//matrix =
			//	Mat4f::TranslationMatrix(Vec3f(position, 0)) *
			//	Mat4f::RotationMatrixAxisZ(transform.rotation) *
			//	Mat4f::TranslationMatrix(Vec3f(-offset, 0));
		}

		transformDirty = false;
		transformUpdatedEventDipatcher.Call({ .node = this });
	}
	Image::Image(Node* parent, const NodeTransform& transform)
		: Node(parent, transform)
	{

	}
	Screen::Screen()
		: Node(nullptr, { })
	{

	}
	Screen::~Screen()
	{
		screenDestructionEventDispatcher.Call({ this });
	}
	void Screen::CalculateAllTransforms()
	{
		CalculateTransform();

		for (auto& child : children)
			CalculateAllTransforms(child);
	}
	void Screen::CalculateAllTransforms(Node* node)
	{
		node->CalculateTransform();

		for (auto& child : node->children)
			CalculateAllTransforms(child);
	}
	void Screen::AddNode(Node* node)
	{		
		nodeCreatedEventDispatcher.Call({ .node = node });
	}
	void Screen::RemoveNode(Node* node)
	{		
		nodeDestroyedEventDispatcher.Call({ .node = node });
	}
	Text::Text(Node* parent, const NodeTransform& transform)
		: Node(parent, transform), font(nullptr), layouter(nullptr), pixelFontHeight(20), sizingType(ByFontHeight), dirty(false)
	{
		transformPreUpdatedEventDipatcher.AddHandler(*this);
	}
	Text::~Text()
	{
		delete layouter;
	}
	void Text::SetText(StringUTF8 text)
	{
		this->text = text;
		dirty = true;
	}
	void Text::SetTextColors(Array<ColorRGBAf> colors)
	{
		this->colors = std::move(colors);		
		dirty = true;
	}
	void Text::SetFont(Font& font)
	{
		this->font = &font;
		dirty = true;
	}
	void Text::SetTextWidth(float width)
	{
		sizingType = ByTextWidth;
		textSize.x = width;		
		dirty = true;
	}
	void Text::SetTextHeight(float height)
	{
		sizingType = ByTextHeight;
		textSize.y = height;
		dirty = true;
	}
	void Text::SetTextSize(Vec2f size)
	{
		sizingType = ByTextSize;
		textSize = size;
		dirty = true;
	}	
	void Text::SetFontHeight(uint pixelFontHeight)
	{		
		sizingType = ByFontHeight;
		this->pixelFontHeight = pixelFontHeight;				
		dirty = true;
	}				
	void Text::GenerateCharacterData()
	{
		switch (sizingType)
		{
		case UI2::Text::ByTextWidth:
			GenerateCharacterDataByTextWidth();
			break;
		case UI2::Text::ByTextHeight:
			GenerateCharacterDataByTextHeight();
			break;
		case UI2::Text::ByTextSize:
			break;
			GenerateCharacterDataByTextSize();
		case UI2::Text::ByFontHeight:
			GenerateCharacterDataByFontHeight();
			break;		
		}
	}
	void Text::GenerateCharacterDataByTextWidth()
	{
		dirty = false;

		if (font == nullptr)
			return;

		if (layouter == nullptr)
			layouter = new SingleLineTextLayouter();

		layouter->SetText(text, font->GetUnscalledMetrics());

		CopyCharacterDataFromLayouter(Vec2f(textSize.x / layouter->GetSize().x));

		pixelFontHeight = textSize.y / (layouter->GetSize().y / font->GetFontUnitScale());
	}
	void Text::GenerateCharacterDataByTextHeight()
	{
		dirty = false;

		if (font == nullptr)
			return;

		if (layouter == nullptr)
			layouter = new SingleLineTextLayouter();

		layouter->SetText(text, font->GetUnscalledMetrics());

		CopyCharacterDataFromLayouter(Vec2f(textSize.y / layouter->GetSize().y));

		pixelFontHeight = textSize.y / (layouter->GetSize().y / font->GetFontUnitScale());
	}
	void Text::GenerateCharacterDataByTextSize()
	{
		dirty = false;

		if (font == nullptr)
			return;		

		if (layouter == nullptr)
			layouter = new SingleLineTextLayouter();

		layouter->SetText(text, font->GetUnscalledMetrics());		
		
		CopyCharacterDataFromLayouter(textSize / Vec2f(layouter->GetSize()));
		
		pixelFontHeight = textSize.y / (layouter->GetSize().y / font->GetFontUnitScale());			
	}
	void Text::GenerateCharacterDataByFontHeight()
	{		
		dirty = false;

		if (font == nullptr)
			return;

		if (layouter == nullptr)
			layouter = new SingleLineTextLayouter();

		layouter->SetText(text, font->GetMetrics(pixelFontHeight));

		CopyCharacterDataFromLayouter(Vec2f(1, 1));

		textSize = Vec2f(layouter->GetSize());
	}
	void Text::CopyCharacterDataFromLayouter(Vec2f scaling)
	{
		characterData.ReserveExactly(text.CharacterCount());
		auto data = layouter->GetCharactersData();
		auto it = text.begin();
		for (uintMem i = 0; i != data.Count(); ++i, ++it)
		{
			if (data[i].size.x == 0 || data[i].size.y == 0)
				continue;

			ColorRGBAf color = 0xffffffff;
			if (i < colors.Count())
				color = colors[i];
			else if (colors.Count() != 0)
				color = colors.Last();

			Vec2f pos = Vec2f(data[i].pos) * scaling;
			Vec2f size = Vec2f(data[i].size) * scaling;

			CharacterData charData;
			charData.character = it.ToUnicode();
			charData.color = color;
			charData.pos = Vec2i(std::ceil(pos.x), std::ceil(pos.y));
			charData.size = Vec2u(std::round(size.x), std::round(size.y));

			characterData.AddBack(charData);
		}		
	}

	void Text::UpdateSize()
	{
		if (dirty)
			GenerateCharacterData();

		auto transform = GetTransform(); 

		transform.size.x = textSize.x;		
		transform.size.y = std::ceil(textSize.y - pixelFontHeight * 0.6f);

		SetTransform(transform);
	}
	void Text::OnEvent(NodePreTransformUpdatedEvent event)
	{
		UpdateSize();
	}
	Panel::Panel(Node* parent, const NodeTransform& transform)
		: Node(parent, transform)
	{
	}
}