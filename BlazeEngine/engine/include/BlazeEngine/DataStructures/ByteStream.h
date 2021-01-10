#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include <vector>

namespace Blaze
{
	class BLAZE_API ByteStream
	{
	protected:
		size_t offset;
		std::vector<uint8> data;
	public:
		ByteStream();
		ByteStream(const ByteStream&);
		ByteStream(ByteStream&&);
		ByteStream(const std::vector<uint8>& data, uint ofbset = 0);
		ByteStream(std::vector<uint8>&& data, uint ofbset = 0);
		ByteStream(void* data, size_t size, uint offet = 0);
		~ByteStream();

		inline size_t GetOffset() const { return offset; }
		inline std::vector<uint8>& GetData() { return data; }
		inline size_t GetSize() { return data.size(); }
		inline const std::vector<uint8>& GetData() const { return data; }

		inline void Skip(uint count) { offset += count; }

		inline void Resize(size_t size) { data.resize(size); }

		inline void SetOffset(const uint& ofbset) { this->offset = ofbset; }
		inline void SetData(const std::vector<uint8>& data) { this->data = data; }
		inline void SetData(std::vector<uint8>&& data) { this->data = std::move(data); }

		inline bool IsEnd() const { return offset == data.size(); }

		ByteStream& Get(void* ptr, uint size);
		ByteStream& Set(const void* ptr, uint size);
		
		template<typename T>
		inline T Get()
		{
			T value;
			Get(&value, sizeof(T));
			return value;
		}

		template<typename T>
		inline ByteStream& Get(T& v)
		{
			return Get(&v, sizeof(T));			
		}
		template<typename T>
		inline ByteStream& Set(const T& v)
		{
			return Set(&v, sizeof(T));			
		}


		template<typename T>
		inline ByteStream& operator>>(T& v)
		{
			return Get<T>(v);			
		}
		template<typename T>
		inline ByteStream& operator<<(const T& v)
		{
			return Set<T>(v);
		}

		void operator=(const ByteStream&);
		void operator=(ByteStream&&);
	};
}