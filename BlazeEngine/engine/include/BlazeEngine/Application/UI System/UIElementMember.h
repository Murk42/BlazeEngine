#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Application/UI System/UIEvent.h"

namespace Blaze::UI
{
	class UIElement;
	class UIScene;

	struct UIElementMemberTypeData
	{
		enum class UIElementMemberType
		{
			Int,
			Float,
			Bool,
			Vec2f,
			Vec3f,
			ColorRGBf,
			ColorRGBAf,
			Align,
			Rectf,
			String,
			StringUTF8

		};

		template<typename T> static constexpr UIElementMemberType GetUIElementMemberType();
		template<> static constexpr UIElementMemberType GetUIElementMemberType<int>() { return UIElementMemberType::Int; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<float>() { return UIElementMemberType::Float; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<bool>() { return UIElementMemberType::Bool; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<Vec2f>() { return UIElementMemberType::Vec2f; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<Vec3f>() { return UIElementMemberType::Vec3f; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<ColorRGBf>() { return UIElementMemberType::ColorRGBf; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<ColorRGBAf>() { return UIElementMemberType::ColorRGBAf; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<Align>() { return UIElementMemberType::Align; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<Rectf>() { return UIElementMemberType::Rectf; }
		template<> static constexpr UIElementMemberType GetUIElementMemberType<String>() { return UIElementMemberType::String; }	
		template<> static constexpr UIElementMemberType GetUIElementMemberType<StringUTF8>() { return UIElementMemberType::StringUTF8; }

		UIElementMemberType type;
		bool list;

		inline bool operator==(const UIElementMemberTypeData& o) const { return type == o.type && list == o.list; }
		inline bool operator!=(const UIElementMemberTypeData& o) const { return type != o.type || list != o.list; }
	};

	template<typename T> 
	constexpr UIElementMemberTypeData GetUIElementMemberTypeData()
	{		
		if constexpr (std::_Is_specialization_v<T, std::vector>)
			return { UIElementMemberTypeData::GetUIElementMemberType<T::value_type>(), true };
		else
			return { UIElementMemberTypeData::GetUIElementMemberType<T>(), false };
	}

	class BLAZE_API UIElementMemberData
	{
		UIElementMemberTypeData typeData;
		String name;		
		
		std::function<void(UIScene*, void*, const void*)> setter;
		std::function<void(UIScene*, const void*, void*)> getter;
	public:		
		inline StringView GetName() const { return name; }
		inline UIElementMemberTypeData GetTypeData() const { return typeData; }		

		friend class UIElementParsingData;
	};
}