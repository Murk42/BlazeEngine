#pragma once
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
    template<typename T, ViewIterator Iterator>
    inline constexpr View<T, Iterator>::View(Iterator begin, Iterator end)
        : _begin(begin), _end(end)
    {
        if constexpr (RandomAccessIterator<Iterator>)
        {
            if (begin > end)
                BLAZE_LOG_FATAL("Invalid view construction iterators, begin > end is true");
        }
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr View<T, Iterator>::View(Iterator begin, uintMem size) requires RandomAccessIterator<Iterator>
        : _begin(begin), _end(begin + size)
    {
    }

    template<typename T, ViewIterator Iterator>
    template<uintMem Size>
    inline constexpr View<T, Iterator>::View(const T(&arr)[Size]) requires ContiguousIterator<Iterator>
        : _begin(arr), _end(arr + Size)
    {
    }

    template<typename T, ViewIterator Iterator>
    template<ConvertableToView<T> C>
    inline constexpr View<T, Iterator>::View(const C& container)
        : View(begin(container), end(container))
    {
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr bool View<T, Iterator>::Empty() const
    {
        return _end == _begin;
    }
    template<typename T, ViewIterator Iterator>
    inline constexpr View<T, Iterator> View<T, Iterator>::SubView(intMem start, intMem end) const requires ContiguousIterator<Iterator>
    {        
        uintMem count = Count();

        if (start < 0)
            start += static_cast<intMem>(count);

        if (end < 0)
            end += static_cast<intMem>(count);

        if (start < 0 || end > static_cast<intMem>(count) || start > end)
            BLAZE_LOG_FATAL("Invalid sub view");

        return View(_begin + start, _begin + end);
    }
    template<typename T, ViewIterator Iterator>
    inline constexpr uintMem View<T, Iterator>::Count() const requires ContiguousIterator<Iterator>
    {
        return _end - _begin;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr const T* View<T, Iterator>::Data() const requires ContiguousIterator<Iterator>
    {
        return &*_begin;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr const T& View<T, Iterator>::First() const
    {
        if (Empty())
            BLAZE_LOG_FATAL("Accessing an empty view");

        return *_begin;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr const T& View<T, Iterator>::Last() const requires RandomAccessIterator<Iterator>
    {
        if (Empty())
            BLAZE_LOG_FATAL("Accessing an empty view");

        auto it = _end;
        --it;
        return *it;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr Iterator View<T, Iterator>::FirstIterator() const
    {
        return _begin;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr Iterator View<T, Iterator>::LastIterator() const requires BidirectionalIterator<Iterator>
    {
        auto it = _end;
        --it;
        return it;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr Iterator View<T, Iterator>::AheadIterator() const requires BidirectionalIterator<Iterator>
    {
        auto it = _begin;
        --it;
        return it;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr Iterator View<T, Iterator>::BehindIterator() const
    {
        return _end;
    }

    template<typename T, ViewIterator Iterator>
    inline constexpr const T& View<T, Iterator>::operator[](uintMem index) const requires ContiguousIterator<Iterator>
    {
        if (_begin + index >= _end)
            BLAZE_LOG_FATAL("Accessing a view outside its bounds");
        return _begin[index];
    }
    
    template<typename T, typename Iterator>
    constexpr auto begin(const View<T, Iterator>& range)
    {
        return range.FirstIterator();
    }    
    template<typename T, typename Iterator>
    constexpr auto end(const View<T, Iterator>& range)
    {
        return range.BehindIterator();
    }
}