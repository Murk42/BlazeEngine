#pragma once

namespace Blaze::UI2
{
	class TransformSystem;

	class Transform : public ECS::Component
	{
		uint updateState;
	public:
		Vec2f offset;
		Vec2f size;
		Align align;

		Align anchorAlign;
		Transform* anchor;

		Vec2f position;

		COMPONENT(Transform, TransformSystem)

		friend class TransformSystem;
	};

	class TransformSystem : public ECS::System
	{
		uint updateState = 0;

		Vec2f GetAlignOffset(Align align, Vec2f size)
		{
			switch (align)
			{
			case Blaze::Align::Top:			return Vec2f(size.x / 2, size.y);
			case Blaze::Align::TopRight:	return Vec2f(size.x, size.y);
			case Blaze::Align::Right:		return Vec2f(size.x, size.y / 2);
			case Blaze::Align::BottomRight:	return Vec2f(size.x, 0);
			case Blaze::Align::Bottom:		return Vec2f(size.x / 2, 0);
			case Blaze::Align::BottomLeft:	return Vec2f(0, 0);
			case Blaze::Align::Left:		return Vec2f(0, size.y / 2);
			case Blaze::Align::TopLeft:		return Vec2f(0, size.y);
			case Blaze::Align::Center:		return Vec2f(size.x / 2, size.y / 2);
			}
		}

		void UpdateTransform(Transform* transform)
		{
			if (transform->updateState == updateState)
				return;

			if (transform->anchor != nullptr)
				UpdateTransform(transform->anchor);

			if (transform->anchor != nullptr)
			{
				transform->position =
					transform->anchor->position +
					transform->offset +
					GetAlignOffset(transform->anchorAlign, transform->anchor->size) -
					GetAlignOffset(transform->align, transform->size);
			}
			else
			{
				transform->position =
					transform->offset +
					GetAlignOffset(transform->anchorAlign, (Vec2f)Graphics::Core::GetViewportSize()) -
					GetAlignOffset(transform->align, transform->size);
			}

			transform->updateState = updateState;
		}

		Align StringToAlign(String string)
		{
			if (string == "Top")			return Align::Top;
			if (string == "TopLeft")		return Align::TopLeft;
			if (string == "TopRight")		return Align::TopRight;
			if (string == "Left")			return Align::Left;
			if (string == "Center")			return Align::Center;
			if (string == "Right")			return Align::Right;
			if (string == "BottomLeft")		return Align::BottomLeft;
			if (string == "Bottom")			return Align::Bottom;
			if (string == "BottomRight")	return Align::BottomRight;
		}
	public:
		void Update(const ECS::ComponentContainer& container) override
		{
			++updateState;
			for (auto component : container)
			{
				Transform* transform = (Transform*)component;
				UpdateTransform(transform);
			}
		}
		bool SetProperty(Component* component, StringView name, StringView value) override
		{
			auto* transform = component->Cast<Transform>();

			//if (name == "name")
			//{
			//	StringParsing::RemoveQuotes(value, transform->name);
			//}
			if (name == "offset")
			{
				String x, y;
				StringParsing::RemoveBrackets(value, x, '(', ')');
				StringParsing::SplitAtFirst(x, x, y, ',');

				String::ConvertTo(x, transform->offset.x);
				String::ConvertTo(y, transform->offset.y);
			}
			else if (name == "size")
			{
				String x, y;
				StringParsing::RemoveBrackets(value, x, '(', ')');
				StringParsing::SplitAtFirst(x, x, y, ',');

				String::ConvertTo(x, transform->size.x);
				String::ConvertTo(y, transform->size.y);
			}
			else if (name == "width")
			{
				String::ConvertTo(value, transform->size.x);
			}
			else if (name == "height")
			{
				String::ConvertTo(value, transform->size.y);
			}
			else if (name == "align")
			{
				String t;
				StringParsing::RemoveSpace(value, t);
				if (t[0] == '(')
				{
					String anchor, local;
					StringParsing::RemoveBrackets(t, t, '(', ')');
					StringParsing::SplitAtFirst(t, anchor, local, ',');
					StringParsing::RemoveSpace(anchor, anchor);
					StringParsing::RemoveSpace(local, local);


					transform->anchorAlign = StringToAlign(anchor);
					transform->align = StringToAlign(local);
				}
				else
				{
					transform->anchorAlign = transform->align = StringToAlign(t);
				}
			}
			//else if (name == "active")
			//{
			//	if (value == "true")
			//		el->active = true;
			//	else if (value == "false")
			//		el->active = false;
			//}
			else
				return false;

			return true;
		}
	};
}