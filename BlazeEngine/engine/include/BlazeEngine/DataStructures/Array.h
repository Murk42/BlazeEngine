#pragma once

namespace Blaze
{
    template<typename T>
    class ArrayView
    {
        T* ptr;
        size_t count;
    public:
        ArrayView()
            : ptr(nullptr), count(0)
        {
        }
        ArrayView(T* ptr, size_t count)
            : ptr(ptr), count(count)
        {
        }
        template<size_t C>
        explicit ArrayView(T(&array)[C])
            : ptr(array), count(C)
        {

        }

        inline T* begin() { return ptr; }
        inline T* end() { return ptr + count; }

        inline T* Ptr() { return ptr; }
        inline const T* Ptr() const { return ptr; }
        inline size_t Count() const { return count; }

        inline T& operator[](size_t index)
        {
#ifdef _DEBUG
            if (index >= count)
                throw std::out_of_range("Index is out of range");
#endif      
            return ptr[index];
        }
        inline const T& operator[](size_t index) const
        {
#ifdef _DEBUG
            if (index >= count)
                throw std::out_of_range("Index is out of range");
#endif      
            return ptr[index];
        }
        inline ArrayView& operator=(const ArrayView& array)
        {
            ptr = array.ptr;
            count = array.count;
            return *this;
        }
    };

    struct adopt_array_t {} adopt_array;

    template<typename T>
    class DynamicArray
    {
        T* ptr;
        size_t count;
    public:
        DynamicArray()
            : ptr(nullptr), count(0)
        {
        }
        DynamicArray(const DynamicArray& array)
        {
            count = array.count;
            ptr = new T[count];
            memcpy(ptr, array.ptr, count * sizeof(int));
        }
        DynamicArray(DynamicArray&& array)
        {
            ptr = array.ptr;
            count = array.count;
            array.ptr = nullptr;
            array.count = 0;
        }
        DynamicArray(T* ptr, size_t count, adopt_array_t)
            : count(count), ptr(ptr)
        {
        }
        DynamicArray(const T* ptr, size_t count)
            : count(count)
        {
            this->ptr = new T[this->count];
            memcpy(this->ptr, ptr, this->count * sizeof(T));
        }
        DynamicArray(const std::initializer_list<int>& list)
        {
            count = list.size();
            ptr = new int[count];
            memcpy(ptr, list.begin(), count * sizeof(T));
        }
        DynamicArray(size_t count)
            : count(count)
        {
            ptr = new T[count];
        }
        ~DynamicArray()
        {
            delete[] ptr;
        }

        inline T* Ptr() { return ptr; }
        inline const T* Ptr() const { return ptr; }

        inline size_t Count() const { return count; }

        inline T* begin() const { return ptr; }
        inline T* end() const { return ptr + count; }

        inline T& operator[](size_t index)
        {
#ifdef _DEBUG
            if (index >= count)
                throw std::out_of_range("Index is out of range");
#endif      
            return ptr[index];
        }
        inline const T& operator[](size_t index) const
        {
#ifdef _DEBUG
            if (index >= count)
                throw std::out_of_range("Index is out of range");
#endif      
            return ptr[index];
        }
        inline DynamicArray& operator=(const DynamicArray& array)
        {
            delete[] ptr;
            count = array.count;
            ptr = new T[count];
            memcpy(ptr, array.ptr, count * sizeof(T));
            return *this;
        }
        inline DynamicArray& operator=(DynamicArray&& array)
        {
            delete[] ptr;
            ptr = array.ptr;
            count = array.count;
            array.ptr = nullptr;
            array.count = 0;
            return *this;
        }
    };
}