#ifndef _UBLIB_INTEGERS_H
#define _UBLIB_INTEGERS_H

#include <cstddef>
#include <limits>
#include <type_traits>

namespace ublib {

static_assert(
    std::numeric_limits<unsigned char>::digits == 8,
    "Size of char in bits must be eight");
static_assert(
    std::numeric_limits<signed char>::lowest() == -128,
    "This library guarantees two's complement, "
    "yet char is not two's complement");

template <std::size_t _Sz>
struct __signed_integer_type;

template <std::size_t _Sz>
struct __unsigned_integer_type {
  using type =
      typename std::make_unsigned_t<typename __signed_integer_type<_Sz>::type>;
};

template <>
struct __signed_integer_type<8> {
  using type = signed char;
};
template <>
struct __signed_integer_type<16> {
  static_assert(sizeof(signed short) == 2, "");
  using type = signed short;
};
template <>
struct __signed_integer_type<32> {
  static_assert(sizeof(signed int) == 4, "");
  using type = signed int;
};
template <>
struct __signed_integer_type<64> {
  static_assert(sizeof(signed long long) == 8, "");
  using type = signed long long;
};

template <std::size_t _Sz, typename _Ty, typename = void>
struct __signed_integer_implicit;

template <std::size_t _Sz, typename _Ty>
struct __signed_integer_implicit<
    _Sz,
    _Ty,
    std::enable_if_t<std::is_integral_v<_Ty>>>
    : std::bool_constant<std::is_signed_v<_Ty> and (sizeof(_Ty) * 8 <= _Sz)> {};

template <std::size_t _Sz, typename _Ty>
struct __signed_integer_explicit
    : std::bool_constant<not __signed_integer_implicit<_Sz, _Ty>()> {};

template <std::size_t _Sz>
class signed_integer {
public:
  using underlying_type = typename __signed_integer_type<8>::type;

private:
  underlying_type __underlying;

public:
  signed_integer() = default;

  template <typename _Ty,
    std::enable_if_t<__signed_integer_implicit<_Sz, _Ty>::value>* = nullptr>
  constexpr signed_integer(_Ty __v) noexcept : __underlying(__v) {}

  template <typename _Ty,
    std::enable_if_t<__signed_integer_explicit<_Sz, _Ty>::value>* = nullptr>
  constexpr explicit signed_integer(_Ty __v) noexcept : __underlying(__v) {}

  constexpr underlying_type value() const noexcept { return __underlying; }
};

template <std::size_t _Sz>
class unsigned_integer {
public:
  using underlying_type = typename __unsigned_integer_type<8>::type;

private:
  underlying_type __underlying;

public:
  unsigned_integer() = default;

  constexpr unsigned_integer(underlying_type __v) noexcept
      : __underlying(__v) {}

  constexpr underlying_type value() const noexcept { return __underlying; }
};

inline namespace integers {
  using i8 = signed_integer<8>;
  using i16 = signed_integer<16>;
  using i32 = signed_integer<32>;
  using i64 = signed_integer<64>;

  using u8 = unsigned_integer<8>;
  using u16 = unsigned_integer<16>;
  using u32 = unsigned_integer<32>;
  using u64 = unsigned_integer<64>;
} // namespace integers

} // namespace ublib

#endif