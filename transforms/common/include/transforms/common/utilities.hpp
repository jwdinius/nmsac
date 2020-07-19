#pragma once
//! c/c++ headers
#include <tuple>
#include <utility>
//! dependency headers
//! project headers

namespace transforms {
template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
/**
 * @brief enumerate over iterable container
 *
 * @param [in] iterable rvalue ref to iterable container
 * @return value of counter and value in container
 */
constexpr auto enumerate(T && iterable) {
  struct iterator {
    size_t i;
    TIter iter;
    bool operator != (const iterator & other) const { return iter != other.iter; }
    void operator ++() { ++i; ++iter; }
    auto operator * () const { return std::tie(i, *iter); }
  };

  struct iterable_wrapper {
    T iterable;
    auto begin() { return iterator{ 0, std::begin(iterable) }; }
    auto end() { return iterator{ 0, std::end(iterable) }; }
  };

  return iterable_wrapper{ std::forward<T>(iterable) };
}
};  // namespace transforms
