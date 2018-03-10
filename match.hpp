#pragma once

#include <tuple>
#include <variant>

namespace ns {

template <typename F, typename ... Types>
struct case_t {
	F function;
};

template <typename F>
struct default_t {
	F function;
};

struct any_type {};

template <typename Type, typename ValueType>
struct type_value_pair {
	using type = Type;
	ValueType value;
};


template <typename Type, typename ValueType>
constexpr auto make_type_value_pair (ValueType && value) {
	return type_value_pair<Type, ValueType> {std::forward<ValueType> (value)};
}

template <typename F, typename ... Match, typename ... Cases>
constexpr auto make_type_to_function_map (case_t<F, Match...> && head, Cases && ... tail) {
	return std::tuple_cat (std::make_tuple (make_type_value_pair<Match, F> (std::forward<F> (head.function))...),
	 		make_type_to_function_map (std::forward<Cases> (tail)...));
};

template <typename F>
constexpr auto make_type_to_function_map (default_t<F> && default_case) {
	return std::make_tuple (make_type_value_pair<any_type, F> (std::forward<F> (default_case.function)));
}

template <typename F, typename ... Match>
constexpr auto make_type_to_function_map (case_t<F, Match...> && case_) {
	return std::make_tuple (make_type_value_pair<Match, F> (std::forward<F> (case_.function))...);
}

template <std::size_t N, typename T, typename ... Pairs>
constexpr decltype (auto) find_type_impl (std::tuple<Pairs...> && t) {
	static_assert (N < std::tuple_size_v<std::tuple<Pairs...>>, "type was not found");
	
	if constexpr (std::is_same_v<T, typename std::decay_t<decltype(std::get<N> (t))>::type> ||
                  std::is_same_v<any_type, typename std::decay_t<decltype(std::get<N> (t))>::type>)
		return std::get<N> (std::move (t)).value;
	else
		return find_type_impl<N + 1, T> (std::move (t));
}

template <typename T, typename ... Pairs>
constexpr decltype (auto) find_type (std::tuple<Pairs...> && t) {
	return find_type_impl<0, T> (std::move (t));
}

template <typename T, typename Map>
constexpr decltype (auto) find_and_apply (T && arg, Map && map) {
	using Type = std::decay_t<decltype (arg)>;
	return find_type<Type> (std::forward<Map> (map)) (std::forward<T> (arg));
}

template <typename ... Types, typename F>
constexpr auto case_ (F && function) {
	return case_t<F, Types...> { std::forward<F> (function) };
}

template <typename F>
constexpr auto default_ (F && function) {
	return default_t< F > { std::forward<F> (function) };
}

template <typename ... Types>
constexpr auto match (const std::variant<Types...> & variant) {
	return [&v = variant] (auto && ... cases) constexpr {
		return std::visit ([map = make_type_to_function_map (std::forward<decltype (cases)> (cases)...)] (auto && arg) mutable constexpr {
			return find_and_apply (std::forward<decltype (arg)> (arg), std::move (map));
		}, v);
	};
}

template <typename ... Types>
constexpr auto match (std::variant<Types...> && variant) {
	return [&v = std::move (variant)] (auto && ... cases) constexpr {
		return std::visit ([map = make_type_to_function_map (std::forward<decltype (cases)> (cases)...)] (auto && arg) mutable constexpr {
			return find_and_apply (std::forward<decltype (arg)> (arg), std::move (map));
		}, std::move (v));
	};
}

template <typename ... Types>
constexpr auto match (std::variant<Types...> & variant) {
	return [&v = variant] (auto && ... cases) constexpr {
		return std::visit ([map = make_type_to_function_map (std::forward<decltype (cases)> (cases)...)] (auto && arg) mutable constexpr {
			return find_and_apply (std::forward<decltype (arg)> (arg), std::move (map));
		}, v);
	};
}

}
