#ifndef _UBLIB_BOX_H
#define _UBLIB_BOX_H

#include <cstddef>
#include <memory>
#include <utility>

namespace ublib {

template <typename _Alloc>
constexpr static bool __use_ebo =
    std::is_empty_v<_Alloc> and not std::is_final_v<_Alloc> and
    not std::has_virtual_destructor_v<_Alloc>;

template <typename _Ptr, typename _Alloc, bool _UseEbo = __use_ebo<_Alloc>>
struct __box_pair;

template <typename _Ptr, typename _Alloc>
struct __box_pair<_Ptr, _Alloc, true> : _Alloc {
  _Ptr __pointer;

  __box_pair() = default;

  template <typename _Ptr1, typename _Alloc1>
  __box_pair(_Ptr1&& __ptr, _Alloc1&& __alloc)
      : _Alloc(std::forward<_Alloc1>(__alloc)),
        __pointer(std::forward<_Ptr1>(__ptr)) {}

  _Alloc& __allocator() { return static_cast<_Alloc&>(*this); }
  _Alloc const& __allocator() const {
    return static_cast<_Alloc const&>(*this);
  }
};

template <typename _Ptr, typename _Alloc>
struct __box_pair<_Ptr, _Alloc, false> {
  _Ptr __pointer;
  _Alloc __alloc;

  __box_pair() = default;

  template <typename _Ptr1, typename _Alloc1>
  __box_pair(_Ptr1&& __ptr, _Alloc1&& __alloc)
      : __alloc(std::forward<_Alloc1>(__alloc)),
        __pointer(std::forward<_Ptr1>(__ptr)) {}

  _Alloc& __allocator() { return __alloc; }
  _Alloc const& __allocator() const { return __alloc; }
};

template <typename _Ty, typename _Alloc = std::allocator<_Ty>>
class box {
  using __allocator_traits = std::allocator_traits<_Alloc>;

  using __poccca =
      typename __allocator_traits::propagate_on_container_copy_assignment;
  using __poccma =
      typename __allocator_traits::propagate_on_container_move_assignment;
  using __pocs =
      typename __allocator_traits::propagate_on_container_swap;
  using __always_equal = typename __allocator_traits::is_always_equal;

  __box_pair<typename __allocator_traits::pointer, _Alloc> __underlying;

  static _Alloc __alloc_copy_construct(_Alloc const& __other) {
    return __allocator_traits::select_on_container_copy_construction(__other);
  }

  template <typename... _Tys>
  void __emplace_empty(_Tys&&... __tys);

public:
  // typedefs
  using value_type = _Ty;
  using reference = value_type&;
  using const_reference = value_type const&;
  using pointer = typename __allocator_traits::pointer;
  using const_pointer = typename __allocator_traits::const_pointer;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  using allocator_type = _Alloc;

  // constructors
  box() = default;
  box(std::allocator_arg_t, _Alloc const&);

  box(box&&) noexcept;
  box(std::allocator_arg_t, _Alloc const&, box&&);

  box(box const&);
  box(std::allocator_arg_t, _Alloc const&, box const&);

  template <typename... _Tys>
  explicit box(std::in_place_t, _Tys&&...);
  template <typename... _Tys>
  box(std::allocator_arg_t, _Alloc const&, std::in_place_t, _Tys&&...);

  explicit box(_Ty const&);
  box(std::allocator_arg_t, _Alloc const&, _Ty const&);

  explicit box(_Ty&&);
  box(std::allocator_arg_t, _Alloc const&, _Ty&&);

  // assignment operators
  box& operator=(box const&);
  box& operator=(box&&) noexcept;

  template <typename... _Tys>
  void emplace(_Tys&&...);
  void assign(_Ty const&);
  void assign(_Ty&&);

  void clear() noexcept;

  void swap(box&) noexcept(__pocs() or __always_equal());

  // destructor
  ~box();

  // accessors
  reference operator*();
  const_reference operator*() const;

  pointer operator->();
  const_pointer operator->() const;

  pointer get() noexcept;
  const_pointer get() const noexcept;

  size_type size() const noexcept;
  size_type max_size() const noexcept;
  bool empty() const noexcept;

  explicit operator bool() const noexcept;

  // iterators
  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept;
  reverse_iterator rbegin() noexcept;
  const_reverse_iterator rbegin() const noexcept;
  const_reverse_iterator crbegin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept;
  reverse_iterator rend() noexcept;
  const_reverse_iterator rend() const noexcept;
  const_reverse_iterator crend() const noexcept;

  // allocator
  allocator_type get_allocator() const;
};

// comparisons
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator==(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs == *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs == *__rhs)>;
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator!=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs != *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)>;

template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator<(box<_T1, _A1> const& __lhs, box<_T2, _A2> const& __rhs) noexcept(
    noexcept(*__lhs < *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)>;
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator>(box<_T1, _A1> const& __lhs, box<_T2, _A2> const& __rhs) noexcept(
    noexcept(*__lhs > *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)>;

template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator<=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs <= *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)>;
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator>=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs >= *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)>;

} // namespace ublib

#include <ublib-box/__impl.h>

#endif