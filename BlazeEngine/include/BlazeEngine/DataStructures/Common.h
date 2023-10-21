#pragma once

namespace Blaze
{
	struct InvalidType
	{
	};

	using uint = unsigned int;

	using int8  = signed char;
	using int16 = signed short;
	using int32 = signed int;
	using int64 = signed long long;	

	using uint8  = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;	
	
	using uintMem = std::make_unsigned_t<decltype(sizeof(int8))>;
	using intMem = std::make_signed_t<decltype(sizeof(int8))>;

	enum byte : char { };
	enum ubyte : unsigned char { };				

	template<typename R, typename ... T>
	using Function = R(*)(T...);	
	
	template<typename T> 
	struct IdentityType_struct
	{
		using type = T;
	};
	template<typename T>
	using IdentityType = typename IdentityType_struct<T>::template type;

	template<typename Dst> 
	Dst implicit_cast(IdentityType<Dst> t)
	{
		return t;
	}

	template<class Base, class Derived>
	inline uintMem BaseOffset()
{
	return reinterpret_cast<char*>(static_cast<Base*>(reinterpret_cast<Derived*>(0xf0000000Ui64))) - reinterpret_cast<char*>(reinterpret_cast<Derived*>(0xf0000000Ui64));
}
}

#define structptr(s, m, p) ((s*)(((char*)p) - (offsetof(s, m))))


#define ENUM_CLASS_BITWISE_OPERATIONS(name) \
	constexpr name operator|(name left, name right) { return (name)((int)left | (int)right); } \
	constexpr name operator&(name left, name right) { return (name)((int)left & (int)right); } \
	constexpr name operator^(name left, name right) { return (name)((int)left ^ (int)right); } \
	constexpr name& operator|=(name& left, name right) { return (left = (name)((int)left | (int)right)); } \
	constexpr name& operator&=(name& left, name right) { return (left = (name)((int)left & (int)right)); } \
	constexpr name& operator^=(name& left, name right) { return (left = (name)((int)left ^ (int)right)); } \
	constexpr name operator~(name value) { return (name)(~(int)value); } 


#define NULL_COPY(a) a(const a&) = delete;
#define NULL_ASSIGN(a) void operator=(const a&) = delete;
#define NULL_COPY_AND_ASSIGN(a) \
NULL_COPY(a) \
NULL_ASSIGN(a)