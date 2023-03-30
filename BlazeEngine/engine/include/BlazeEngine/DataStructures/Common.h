#pragma once

namespace Blaze
{
	struct InvalidType
	{
	};

	using uint = unsigned;

	using int8  = char;
	using int16 = short;
	using int32 = int;
	using int64 = long long;	

	using uint8  = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;	

	enum byte : char { };
	enum ubyte : unsigned char { };				

	template<typename R, typename ... T>
	using Function = R(*)(T...);	
}

#define structptr(s, m, p) ((s*)(((char*)p) - (offsetof(s, m))))

template<class Base, class Derived>
inline int BaseOffset()
{
	return reinterpret_cast<char*>(static_cast<Base*>(reinterpret_cast<Derived*>(0xf0000000))) - reinterpret_cast<char*>(reinterpret_cast<Derived*>(0xf0000000));
}

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