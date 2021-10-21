#pragma once
#include "BlazeEngine/DataStructures/Buffer.h"
#include <initializer_list>
#include <utility>

namespace Blaze
{       
    template<typename>
    class ArrayView;

    template<bool construct = true, bool destruct = true, bool copy = true, bool move = true>
    class ArraySettings
    {
    public:         
        static constexpr bool Construct = construct;
        static constexpr bool Destruct = destruct;
        static constexpr bool Copy = copy;
        static constexpr bool Move = move;     
    };

    class ArrayPOD : public ArraySettings<false, false, false, false>
    {

    };

    template<typename T, typename Settings = ArraySettings<>>
    class Array
    {
        size_t size;
        T* ptr;
    public:
        Array()
            : ptr(nullptr), size(0)
        {
        }
        template<typename OtherSettings>
        Array(const Array<T, OtherSettings>& array)
            : size(array.size)
        {
            ptr = (T*)new char[size * sizeof(T)];

            if constexpr (Settings::Copy)
            {
                for (size_t i = 0; i < size; ++i)
                    new (ptr + i)T(array.ptr[i]);
            }
            else
                memcpy(ptr, array.ptr, sizeof(T) * size);
        }
        template<typename OtherSettings>
        Array(Array<T, OtherSettings>&& array) noexcept 
            : size(array.size), ptr(array.ptr)
        {
            array.ptr = nullptr;
            array.size = 0;
        }
        Array(size_t size)
            : size(size)
        {
            if constexpr (Settings::Construct)
                ptr = new T[size];
            else
                ptr = new char[size * sizeof(T)];
        }
        Array(ArrayView<T> array);
        ~Array()
        {
            if constexpr (Settings::Destruct)
                for (size_t i = 0; i < size; ++i)
                    ptr[i].~T();

            delete[](char*)ptr;
        }

        inline size_t Size() { return size; }
        inline T* Ptr() { return ptr; }
        inline const T* Ptr() const { return ptr; }

        void Resize(size_t newSize)
        {
            if (newSize > size)
            {
                T* old = ptr;
                ptr = (T*)new char[sizeof(T) * newSize];
                if constexpr (Settings::Move || Settings::Copy)
                {
                    for (size_t i = 0; i < size; ++i)
                        if constexpr (Settings::Move)
                            new (ptr + i)T(std::move(old[i]));
                        else if constexpr (Settings::Copy)
                            new (ptr + i)T(old[i]);
                }
                else
                    memcpy(ptr, old, size * sizeof(T));

                if constexpr (Settings::Construct)
                    for (int i = size; i < newSize; ++i)
                        new (ptr + i)T();

                if constexpr (Settings::Destruct)
                    for (size_t i = 0; i < size; ++i)
                        ptr[i].~T();

                delete[] (char*)old;
                        
            }
            else if (newSize < size)
            {
                T* old = ptr;
                ptr = (T*)new char[sizeof(T) * newSize];

                if constexpr (Settings::Move || Settings::Copy)
                {
                    for (size_t i = 0; i < newSize; ++i)
                        if constexpr (Settings::Move)
                            new (ptr + i)T(std::move(old[i]));
                        else if constexpr (Settings::Copy)
                            new (ptr + i)T(old[i]);
                }
                else
                    memcpy(ptr, old, newSize * sizeof(T));                

                if constexpr (Settings::Destruct)
                    for (size_t i = 0; i < size; ++i)
                        ptr[i].~T();

                delete[](char*)old;
            }

            size = newSize;
        }

        void Add(const T& value)
        {            
            T* old = ptr;
            ptr = new char[sizeof(T) * (size + 1)];
            if constexpr (Settings::Move || Settings::Copy)
            {
                for (size_t i = 0; i < size; ++i)
                    if constexpr (Settings::Move)
                        new (ptr + i)T(std::move(old[i]));
                    else if constexpr (Settings::Copy)
                        new (ptr + i)T(old[i]);
            }
            else
                memcpy(ptr, old, size * sizeof(T));

            if constexpr (Settings::Construct)
                new (ptr + size)T(value);
            else if constexpr (Settings::Move)
                new (ptr + size)T(std::move(value));
            else
                memcpy(ptr + size, &value, sizeof(T));

            if constexpr (Settings::Destruct)
                for (size_t i = 0; i < size; ++i)
                    ptr[i].~T();
            
            delete[](char*)old;
        }
        void Clear()
        {
            if constexpr (Settings::Destruct)            
                for (size_t i = 0; i < size; ++i)
                    ptr[i].~T();
                
            delete[](char*)ptr;

            ptr = nullptr;
            size = 0;
        }

        T& operator[](size_t index)
        {
            return ptr[index];
        }
        const T& operator[](size_t index) const
        {
            return ptr[index];
        }

        template<typename OtherSettings>
        Array& operator=(const Array<T, OtherSettings>& array)
        {
            if constexpr (Settings::Destruct)
                for (size_t i = 0; i < size; ++i)
                    ptr[i].~T();

            delete[] (char*)ptr;

            size = array.size;
            ptr = new char[size * sizeof(T)];

            if constexpr (Settings::Copy)
                for (size_t i = 0; i < size; ++i)
                    new (ptr + i)T(array.ptr[i]);
            else
                memcpy(ptr, array.ptr, sizeof(T) * size);

            return *this;
        }
        template<typename OtherSettings>
        Array& operator=(Array<T, OtherSettings>&& array) noexcept
        {
            size = array.size;
            ptr = array.ptr;
            array.size = 0;
            array.ptr = nullptr;

            return *this;
        }

        T* begin() const 
        {
            return ptr;
        }
        T* end() const 
        {
            return ptr + size;
        }
    };

    template<typename T>
    class ArrayView
    {
        const T* ptr;
        size_t count;
    public:
        ArrayView()
            : ptr(nullptr), count(0)
        {
        }
        ArrayView(const T* ptr, size_t count)
            : ptr(ptr), count(count)
        {
        }
        ArrayView(const std::initializer_list<size_t>& list)
            : ptr(list.begin()), count(list.size())
        {
        }
        template<size_t C>
        explicit ArrayView(T(&array)[C])
            : ptr(array), count(C)
        {

        }

        inline const T* begin() { return ptr; }
        inline const T* end() { return ptr + count; }
        
        inline const T* Ptr() const { return ptr; }
        inline size_t Count() const { return count; }
        
        inline const T& operator[](size_t index) const
        {   
            return ptr[index];
        }
        inline ArrayView& operator=(const ArrayView& array)
        {
            ptr = array.ptr;
            count = array.count;
            return *this;
        }
    };

    template<typename T, typename Settings>
    inline Array<T, Settings>::Array(ArrayView<T> array)
    {
        size = array.Count();
        ptr = (T*)new char[size * sizeof(T)];

        if constexpr (Settings::Copy)
        {
            for (size_t i = 0; i < size; ++i)
                new (ptr + i)T(array[i]);
        }
        else
            memcpy(ptr, array.Ptr(), sizeof(T) * size);
    }
}