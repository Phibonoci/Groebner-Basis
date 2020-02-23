#pragma once

#include <type_traits>

template <class T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept Arithmetic = requires (T lhs, T rhs) {
    {lhs * rhs} -> T;
    {lhs + rhs} -> T;
    {lhs - rhs} -> T;
    {lhs / rhs} -> T;
    {-lhs} -> T;
};

template <typename T>
concept SuitableFieldType = Arithmetic<T> && requires (T value) {
    value == T();
};
