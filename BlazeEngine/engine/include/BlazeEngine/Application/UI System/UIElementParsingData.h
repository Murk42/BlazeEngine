#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/DataStructures/String.h"
#include <vector>
#include "BlazeEngine/Application/UI System/UIElementMember.h"
#include "BlazeEngine/DataStructures/StringHash.h"
#include <unordered_map>
#include <type_traits>

namespace Blaze::UI
{
	class UIElement;
	class UIScene;

	class BLAZE_API UIElementParsingData
	{		
		std::vector<UIElementMemberData> members;
		std::unordered_map<String, uint> nameMap;

		Result AddMemberRaw(StringView, UIElementMemberTypeData,
			std::function<void(UIScene*, void*, const void*)> setter,
			std::function<void(UIScene*, const void*, void*)> getter);

		template<typename C, typename T, typename S>
		std::function<void(UIScene*, void*, const void*)> GetSetterFunction(const S& setter)
		{
			if constexpr (std::is_null_pointer_v<S>)
				return nullptr;
			else if constexpr (std::is_member_function_pointer_v<S>)
				return [setter](UIScene* scene, void* e, const void* v) { (((C*)e)->*setter)(*(const T*)v); };
			else
				return [setter](UIScene* scene, void* e, const void* v) { setter(*scene, *(C*)e, *(const T*)v); };
		}
		template<typename C, typename T, typename G>
		std::function<void(UIScene*, const void*, void*)> GetGetterFunction(const G& getter)
		{
			if constexpr (std::is_null_pointer_v<G>)
				return nullptr;
			else if constexpr (std::is_member_function_pointer_v<G>)
				return [getter](UIScene* scene, const void* e, void* v) { *(T*)v = (((C*)e)->*getter)(); };
			else
				return [getter](UIScene* scene, const void* e, void* v) { *(T*)v = getter(*scene, *(C*)e); };
		}
	public:
		UIElementParsingData();
		UIElementParsingData(const UIElementParsingData&);
		UIElementParsingData(UIElementParsingData&&) noexcept;
		~UIElementParsingData();

		template<typename C, typename T, typename S, typename G>
		Result AddMember(StringView name, const S& setter, const G& getter)
		{			
			return AddMemberRaw(name, GetUIElementMemberTypeData<T>(),
				GetSetterFunction<C, T, S>(setter),
				GetGetterFunction<C, T, G>(getter)
				);
		}		
		template<typename C, typename T>
		Result AddMember(StringView name, T C::* ptr)
		{
			return AddMemberRaw(name, GetUIElementMemberTypeData<T>(),
				[ptr](UIScene*, void* e, const void* v) { ((C*)e)->*ptr = *(T*)v; },
				[ptr](UIScene*, const void* e, void* v) { *(T*)v = ((const C*)e)->*ptr; }
				);
		}
		
		int GetMemberIndex(StringView name) const;
		UIElementMemberData GetMember(uint index) const;
		uint GetMemberCount() const;

		template<typename C, typename T>
		Result GetValue(uint index, const C* element, T& value) const
		{
			return GetValue(index, C::typeName, element, ptr);
		}
		template<typename T>
		Result GetValue(uint index, StringView typeName, const UIElement* element, T& value) const
		{
			const auto& member = members[index];

 			if (GetUIElementMemberType<T>() != member.type)
				Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Trying to get invalid value. The value types are not the same"));

			if (member.functions)
				member.getter((const void*)element, &value);
			else
				value = *(T*)((char*)element + (size_t)member.offset);

			return Result();
		}				
		template<typename T>
		Result SetValue(uint index, UIScene* scene, UIElement* element, const T& value) const
		{
			const auto& member = members[index];

			if (GetUIElementMemberTypeData<T>() != member.typeData)
				Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Trying to get invalid value. The value types are not the same"));

			member.setter(scene, element, &value);

			return Result();
		}

		UIElementParsingData& operator=(const UIElementParsingData&);
		UIElementParsingData& operator=(UIElementParsingData&&) noexcept;
	};
}