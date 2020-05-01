#pragma once

#include <type_traits>
#include <functional>

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T, typename U>
concept IsSame = std::is_same_v<T, U>;

template <typename From, typename To>
concept ConvertibleTo = std::is_convertible_v<From, To> && requires(std::add_rvalue_reference_t<From>(&f)()) {
    static_cast<To>(f());
};

template <typename T>
concept Arithmetic = requires (T lhs, T rhs) {
    {lhs * rhs} -> IsSame<T>;
    {lhs + rhs} -> IsSame<T>;
    {lhs - rhs} -> IsSame<T>;
    {lhs / rhs} -> IsSame<T>;
    {-lhs} -> IsSame<T>;
};

template <typename T>
concept SuitableFieldType = Arithmetic<T> && requires (T value) {
    { T() } -> IsSame<T>;
    {value == value} -> IsSame<bool>;
};

template <typename T, typename U>
concept Order = requires (T value, U lhs, U rhs) {
    {value.operator()(lhs, rhs)} -> IsSame<bool>;
};
