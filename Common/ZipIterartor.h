#pragma once

#include <tuple>

template<size_t N, typename Type1, typename... Types>
struct get_type
{
	using type = typename get_type<N - 1, Types...>::type;
};

template<typename Type1, typename... Types>
struct get_type<0, Type1, Types...>
{
	using type = Type1;
};

template<size_t N, typename... Types>
using get_type_t = typename get_type<N, Types...>::type;

namespace detail
{
    template<typename TupleT, typename FunctionT, size_t... Indexes>
    auto transform_tuple_impl(const TupleT& tuple, FunctionT&& function, std::index_sequence<Indexes...>)
    {
        return std::forward_as_tuple(std::forward<FunctionT>(function)(std::get<Indexes>(tuple))...);
    }

	// copy-pasted from http://en.cppreference.com/w/cpp/utility/apply
    template <class TupleT, class FunctionT, std::size_t... Indexes>
    constexpr decltype(auto) apply_tuple_impl(TupleT&& tuple, FunctionT&& function, std::index_sequence<Indexes...>)
	{
        return std::forward<FunctionT>(function)(std::get<Indexes>(std::forward<TupleT>(tuple))...);
    }

} // namespace detail

template<typename FunctionT, typename... TupleElementsT>
auto transform_tuple(const std::tuple<TupleElementsT...>& tuple, FunctionT function)
{
	return detail::transform_tuple_impl(tuple, function, std::make_index_sequence<sizeof...(TupleElementsT)>());
}

template <class FunctionT, class TupleT>
constexpr decltype(auto) apply_tuple(TupleT&& tuple, FunctionT&& function) // std::apply is C++17 feature not yet supported by compilers
{
	auto indexes = std::make_index_sequence<std::tuple_size_v<std::decay_t<TupleT>>>{};
    return detail::apply_tuple_impl(std::forward<TupleT>(tuple), std::forward<FunctionT>(function), indexes);
}

template <typename TupleT, typename FunctionT>
void for_each_tuple(TupleT&& tuple, FunctionT&& function)
{
	auto function_to_apply{ [&function](auto&&... elements) { std::initializer_list<char>{ (function(elements), char{}) ... }; } };
	apply_tuple(std::forward<TupleT>(tuple), std::move(function_to_apply));
}

template<typename... UnderlyingIterators>
class ZipIterator
{
public:
	
	// TODO: deduce common types (see https://github.com/matheuspf/ZipIter/tree/master/include/ZipIter)
    using difference_type = typename std::iterator_traits<get_type_t<0, UnderlyingIterators...>>::difference_type;
    using value_type = std::tuple<typename std::iterator_traits<UnderlyingIterators>::value_type...>;
    using pointer = std::tuple<typename std::iterator_traits<UnderlyingIterators>::pointer...>;
    using reference = std::tuple<typename std::iterator_traits<UnderlyingIterators>::reference...>;
    using iterator_category = typename std::iterator_traits<get_type_t<0, UnderlyingIterators...>>::iterator_category;

    explicit ZipIterator(UnderlyingIterators&&... iterators) : m_iterators(std::make_tuple(std::forward<UnderlyingIterators>(iterators)...))
    {}

	ZipIterator& operator++()
	{
		for_each_tuple(m_iterators, [](auto& it) { ++it; });
		return *this;
	}

	ZipIterator operator++(int)
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	// TODO: enable if bidirectional iterator (and add static_assert)
	ZipIterator& operator--()
	{
		for_each_tuple(m_iterators, [](auto& it) { --it; });
		return *this;
	}

	// TODO: enable if iterator_category is bidirectional iterator (and add static_assert)
	ZipIterator operator--(int)
	{
		auto tmp = *this;
		--(*this);
		return tmp;
	}

	// TODO: enable if iterator_category is random_access iterator (and add static_assert)
	ZipIterator& operator+=(int n)
	{
		for_each_tuple(m_iterators, [n](auto& it) { it += n; });
		return *this;
	}

	// TODO: enable if iterator_category is random_access iterator (and add static_assert)
	ZipIterator& operator-=(int n)
	{
		for_each_tuple(m_iterators, [n](auto& it) { it -= n; });
		return *this;
	}

	reference operator*()
    {  
		return Dereference(std::make_index_sequence<sizeof...(UnderlyingIterators)>());
    }

	const reference operator*() const
	{
		return Dereference(std::make_index_sequence<sizeof...(UnderlyingIterators)>());
	}

    pointer operator->()
    {
		return &m_iterators;
    }

	// Non-member interface functions
	template <typename... Iterators>
	friend auto operator+(const ZipIterator<Iterators...>&, const ZipIterator<Iterators...>&);

	template <typename... Iterators>
	friend auto operator-(const ZipIterator<Iterators...>&, const ZipIterator<Iterators...>&);

	template <typename... Iterators>
	friend bool operator==(const ZipIterator<Iterators...>&, const ZipIterator<Iterators...>&);

	template <typename... Iterators>
	friend bool operator<(const ZipIterator<Iterators...>&, const ZipIterator<Iterators...>&);

private:

	template <std::size_t... Indexes>
	decltype(auto) Dereference(std::index_sequence<Indexes...>)
	{
		return std::forward_as_tuple(*std::get<Indexes>(m_iterators)...);
	}

	template <std::size_t... Indexes>
	decltype(auto) Dereference(std::index_sequence<Indexes...>) const
	{
		return std::forward_as_tuple(*std::get<Indexes>(m_iterators)...);
	}

	std::tuple<UnderlyingIterators...> m_iterators;
};

template<typename... Iterators>
decltype(auto) operator+(ZipIterator<Iterators...>& iter, int inc)
{
	iter += inc;
	return iter;
}

template<typename... Iterators>
decltype(auto) operator-(ZipIterator<Iterators...>& iter, int inc)
{
	iter -= inc;
	return iter;
}

template <typename... Iterators>
auto operator+(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return std::get<0>(rhs.m_iterators) + std::get<0>(lhs.m_iterators);
}

template <typename... Iterators>
auto operator-(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return std::get<0>(rhs.m_iterators) - std::get<0>(lhs.m_iterators);
}

template <typename... Iterators>
bool operator==(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return rhs.m_iterators == lhs.m_iterators;
}

template <typename... Iterators>
bool operator!=(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return !(rhs == lhs);
}

template <typename... Iterators>
bool operator<(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return std::get<0>(rhs.m_iterators) < std::get<0>(lhs.m_iterators);
}

template <typename... Iterators>
bool operator>(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return operator<(lhs, rhs);
}

template <typename... Iterators>
bool operator<=(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return !operator>(lhs, rhs);
}

template <typename... Iterators>
bool operator>=(const ZipIterator<Iterators...>& rhs, const ZipIterator<Iterators...>& lhs)
{
	return !operator<(lhs, rhs);
}

template<typename... Iterators>
ZipIterator<Iterators...> MakeZipIterator(Iterators&&... underlyingIterators)
{
	return ZipIterator<Iterators...>{ std::forward<Iterators>(underlyingIterators)... };
}
