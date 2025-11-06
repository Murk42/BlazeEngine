#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include <iterator>

namespace Blaze
{
    template<typename Iterator>
    concept ViewIterator = std::input_iterator<Iterator>;
    template<typename Iterator, typename T>
    concept ViewIteratorOf =
        std::input_iterator<Iterator> &&
        requires(const Iterator it) {
            { *it } -> SameAs<const T&>;
    };

    template<typename Container, typename T>
    concept ConvertableToView =
        requires(const Container x) {
            { begin(x) } -> ViewIteratorOf<T>;
            { end(x) } -> ViewIteratorOf<T>;
    };

    template<typename T, ViewIterator Iterator = const T*>
    class View
    {
    public:
        using ValueType = T;
        using IteratorType = Iterator;

        constexpr View() = default;
        constexpr View(const View&) = default;
        constexpr View(Iterator begin, Iterator end);
        constexpr View(Iterator begin, uintMem size) requires RandomAccessIterator<Iterator>;
        template<uintMem Size>
        constexpr View(const T(&arr)[Size]) requires ContiguousIterator<Iterator>;
        template<ConvertableToView<T> C>
        constexpr View(const C& container);

        constexpr bool Empty() const;

        constexpr View<T, Iterator> SubView(intMem start, intMem end) const requires ContiguousIterator<Iterator>;

        constexpr uintMem Count() const requires ContiguousIterator<Iterator>;
        constexpr const T* Data() const requires ContiguousIterator<Iterator>;
        constexpr const T& First() const;
        constexpr const T& Last() const requires RandomAccessIterator<Iterator>;

        constexpr Iterator FirstIterator() const;
        constexpr Iterator LastIterator() const requires BidirectionalIterator<Iterator>;
        constexpr Iterator AheadIterator() const requires BidirectionalIterator<Iterator>;
        constexpr Iterator BehindIterator() const;

        constexpr const T& operator[](uintMem index) const requires ContiguousIterator<Iterator>;

        constexpr View& operator=(const View&) = default;

    private:
        Iterator _begin{};
        Iterator _end{};
    };
    
    template<typename Container>
    View(const Container&) -> View<std::remove_cvref_t<decltype(*begin(std::declval<const Container&>()))>, decltype(begin(std::declval<const Container&>()))>;    
    template<ViewIterator T>
    View(T, T) -> View<RemoveConst<decltype(*std::declval<T>())>, T>;
    template<typename T>
    View(T*, T*) -> View<T, T*>;
}

#include "BlazeEngine/Core/Common/ViewImpl.h"