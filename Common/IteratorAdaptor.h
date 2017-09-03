#pragma once

#include <utility>
#include <vector>

template<typename... UnderlyingIterators>
class ZipIterator : public std::iterator<
    typename std::iterator_traits<get_type_t<0, UnderlyingIterators...>>::iterator_category,
    std::tuple<typename std::iterator_traits<UnderlyingIterators>::reference...>,
    typename std::iterator_traits<get_type_t<0, UnderlyingIterators...>>::difference_type,
    const std::tuple<typename std::iterator_traits<UnderlyingIterators>::reference...>*,
    const std::tuple<typename std::iterator_traits<UnderlyingIterators>::reference...>&
    >
{
public:
    ZipIterator(UnderlyingIterators... iterators) : m_iterators(std::make_tuple(iterators...)) 
    {}
    
    ZipIterator operator++() 
    { std::for_each_tuple(iterators_, [](auto& it) {++it;}); return *this; }
    ZipIterator& operator=(const ZipIterator& other) { iterators_ = other.iterators_; }
    auto operator*() { return transform_tuple(iterators_, [](auto const& it) {return *it;}); }
    bool operator==(const ZipIterator& other) { return iterators_ == other.iterators_; }
    bool operator!=(const ZipIterator& other) { return !(*this == other); }
    auto operator-(const ZipIterator& other) { return std::get<0>(iterators_) - std::get<0>(other.iterators_); }

private:
    std::tuple<UnderlyingIterators...> m_iterators;
};


template<typename... Iterators>
ZipIterator<Iterators...> MakeZipIterator(Iterators... underlyingIterators)
{
    return ZipIterator<Iterators...>(underlyingIterators...);
}
