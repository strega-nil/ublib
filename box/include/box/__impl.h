namespace ublib {

template <typename _Ptr>
auto __box_unfancy(_Ptr p) {
  return std::addressof(*p);
}

// constructors
template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(std::allocator_arg_t, _Alloc const& __alloc)
    : __underlying(nullptr, __alloc) {}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(box&& __other) noexcept
    : __underlying(nullptr, std::move(__other.__underlying.__allocator())) {
  using std::swap;
  swap(__other.__underlying.__pointer, __underlying.__pointer);
}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(
    std::allocator_arg_t, _Alloc const& __alloc, box&& __other)
    : box(std::allocator_arg, __alloc) {
  if (__underlying.__allocator() == __other.__underlying.__allocator()) {
    using std::swap;
    swap(__other.__underlying.__pointer, __underlying.__pointer);
  } else {
    *this = std::move(__other);
  }
}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(box const& __other)
    : __underlying(
          nullptr, __alloc_copy_construct(__other.__underlying.__allocator())) {
  if (__other) {
    __emplace_empty(*__other);
  }
}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(
    std::allocator_arg_t, _Alloc const& __alloc, box const& __other)
    : box(std::allocator_arg, __alloc) {
  if (__other) {
    __emplace_empty(*__other);
  }
}

template <typename _Ty, typename _Alloc>
template <typename... _Tys>
box<_Ty, _Alloc>::box(std::in_place_t, _Tys&&... __tys)
    : box(std::allocator_arg,
          _Alloc(),
          std::in_place,
          std::forward<_Tys>(__tys)...) {}

template <typename _Ty, typename _Alloc>
template <typename... _Tys>
box<_Ty, _Alloc>::box(
    std::allocator_arg_t,
    _Alloc const& __alloc,
    std::in_place_t,
    _Tys&&... __tys)
    : box(std::allocator_arg, __alloc) {
  __emplace_empty(std::forward<_Tys>(__tys)...);
}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(_Ty&& __value) : box(std::in_place, std::move(__value)) {}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(
    std::allocator_arg_t, _Alloc const& __alloc, _Ty&& __value)
    : box(std::allocator_arg, __alloc, std::in_place, std::move(__value)) {}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(_Ty const& __value) : box(std::in_place, __value) {}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::box(
    std::allocator_arg_t, _Alloc const& __alloc, _Ty const& __value)
    : box(std::allocator_arg, __alloc, std::in_place, __value) {}

// assignment operators
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator=(box&& __other) noexcept -> box& {
  using std::swap;
  if constexpr (__poccma()) {
    clear();
    if (__underlying.__allocator() != __other.__underlying.__allocator()) {
      __underlying.__allocator() =
          std::move(__other.__underlying.__allocator());
    }
    swap(__underlying.__pointer, __other.__underlying.__pointer);
  } else if constexpr (__always_equal()) {
    clear();
    swap(__underlying.__pointer, __other.__underlying.__pointer);
  } else if (__other and *this) {
    **this = std::move(*__other);
    __other.clear();
  } else if (__other.empty()) {
    clear();
  }

  return *this;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator=(box const& __other) -> box& {
  if (std::addressof(__other) != this) {
    if constexpr (__poccca()) {
      if (__underlying.__allocator() != __other.__underlying.__allocator()) {
        clear();
        __underlying.__allocator = __other.__underlying.__allocator();
      }
    }

    if (__other and *this) {
      **this = *__other;
    } else if (__other and empty()) {
      __emplace_empty(*__other);
    } else if (not __other) {
      clear();
    }
  }

  return *this;
}

template <typename _Ty, typename _Alloc>
template <typename... _Tys>
void box<_Ty, _Alloc>::__emplace_empty(_Tys&&... __tys) {
  auto& __allocator = __underlying.__allocator();
  auto __new_ptr = __allocator_traits::allocate(__allocator, 1);
  try {
    __allocator_traits::construct(
        __allocator, __box_unfancy(__new_ptr), std::forward<_Tys>(__tys)...);
  } catch (...) {
    __allocator_traits::deallocate(__allocator, __new_ptr, 1);
    throw;
  }
  __underlying.__pointer = std::move(__new_ptr);
}
template <typename _Ty, typename _Alloc>
template <typename... _Tys>
void box<_Ty, _Alloc>::emplace(_Tys&&... __tys) {
  if (*this) {
    __allocator_traits::destroy(__box_unfancy(__underlying.__pointer));
    try {
      __allocator_traits::construct(
          __box_unfancy(__underlying.__pointer), std::forward<_Tys>(__tys)...);
    } catch (...) {
      __allocator_traits::deallocate(__underlying.__pointer, 1);
      throw;
    }
  } else {
    __emplace_empty(std::forward<_Tys>(__tys)...);
  }
}

template <typename _Ty, typename _Alloc>
void box<_Ty, _Alloc>::assign(_Ty const& __val) {
  emplace(__val);
}
template <typename _Ty, typename _Alloc>
void box<_Ty, _Alloc>::assign(_Ty&& __val) {
  emplace(std::move(__val));
}
template <typename _Ty, typename _Alloc>
void box<_Ty, _Alloc>::clear() noexcept {
  if (*this) {
    auto& __allocator = __underlying.__allocator();
    auto __ptr = std::exchange(__underlying.__pointer, nullptr);
    __allocator_traits::destroy(__allocator, __box_unfancy(__ptr));
    __allocator_traits::deallocate(__allocator, __ptr, 1);
  }
}

template <typename _Ty, typename _Alloc>
void box<_Ty, _Alloc>::swap(box& __other) noexcept(__pocs() or __always_equal()) {
  using std::swap;
  if constexpr (__pocs()) {
    swap(__underlying.__allocator(), __other.__underlying.__allocator());
    swap(__underlying.__pointer, __other.__underlying.__pointer);
  } else if constexpr (__always_equal()) {
    swap(__underlying.__pointer, __other.__underlying.__pointer);
  } else if (__underlying.__allocator() == __other.__underlying.__allocator()) {
    swap(__underlying.__pointer, __other.__underlying.__pointer);
  } else {
    __ublib_undefined_behavior("allocator != other.allocator");
  }
}

template <typename _Ty, typename _Alloc>
void swap(box<_Ty, _Alloc>& __lhs, box<_Ty, _Alloc>& __rhs) {
  __lhs.swap(__rhs);
}

// destructor
template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::~box() {
  clear();
}

// accessors
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator*() -> reference {
  return *__underlying.__pointer;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator*() const -> const_reference {
  return *__underlying.__pointer;
}

template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator-> () -> pointer {
  return __underlying.__pointer;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::operator-> () const -> const_pointer {
  return __underlying.__pointer;
}

template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::get() noexcept -> pointer {
  return __underlying.__pointer;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::get() const noexcept -> const_pointer {
  return __underlying.__pointer;
}

template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::size() const noexcept -> size_type {
  if (*this) {
    return 1;
  } else {
    return 0;
  }
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::max_size() const noexcept -> size_type {
  return 1;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::empty() const noexcept -> bool {
  return static_cast<bool>(__underlying.__pointer == nullptr);
}

template <typename _Ty, typename _Alloc>
box<_Ty, _Alloc>::operator bool() const noexcept {
  return not empty();
}

// iterators
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::begin() noexcept -> iterator {
  return __underlying.__pointer;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::begin() const noexcept -> const_iterator {
  return __underlying.__pointer;
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::cbegin() const noexcept -> const_iterator {
  return begin();
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::rbegin() noexcept -> reverse_iterator {
  return std::make_reverse_iterator(end());
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::rbegin() const noexcept -> const_reverse_iterator {
  return std::make_reverse_iterator(end());
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::crbegin() const noexcept -> const_reverse_iterator {
  return rbegin();
}

template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::end() noexcept -> iterator {
  return begin() + size();
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::end() const noexcept -> const_iterator {
  return begin() + size();
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::cend() const noexcept -> const_iterator {
  return end();
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::rend() noexcept -> reverse_iterator {
  return std::make_reverse_iterator(begin());
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::rend() const noexcept -> const_reverse_iterator {
  return std::make_reverse_iterator(begin());
}
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::crend() const noexcept -> const_reverse_iterator {
  return rend();
}

// allocator
template <typename _Ty, typename _Alloc>
auto box<_Ty, _Alloc>::get_allocator() const -> allocator_type {
  return __underlying.__allocator();
}

// comparisons
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator==(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs == *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs == *__rhs)> {
  if (not __lhs and not __rhs) {
    return true;
  } else if (__lhs and __rhs) {
    return *__lhs == *__rhs;
  } else {
    return false;
  }
}
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator!=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs != *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)> {
  if (not __lhs and not __rhs) {
    return false;
  } else if (__lhs and __rhs) {
    return *__lhs != *__rhs;
  } else {
    return true;
  }
}

template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator<(box<_T1, _A1> const& __lhs, box<_T2, _A2> const& __rhs) noexcept(
    noexcept(*__lhs < *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)> {
  if (not __lhs and not __rhs) {
    return false;
  } else if (not __lhs and __rhs) {
    return true;
  } else if (not __rhs and __lhs) {
    return false;
  } else {
    return *__lhs < *__rhs;
  }
}
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator>(box<_T1, _A1> const& __lhs, box<_T2, _A2> const& __rhs) noexcept(
    noexcept(*__lhs > *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)> {
  if (not __lhs and not __rhs) {
    return false;
  } else if (__lhs and not __rhs) {
    return true;
  } else if (__rhs and not __lhs) {
    return false;
  } else {
    return *__lhs > *__rhs;
  }
}

template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator<=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs <= *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)> {
  if (not __lhs and not __rhs) {
    return true;
  } else if (not __lhs and __rhs) {
    return true;
  } else if (not __rhs and __lhs) {
    return false;
  } else {
    return *__lhs <= *__rhs;
  }
}
template <typename _T1, typename _A1, typename _T2, typename _A2>
auto operator>=(
    box<_T1, _A1> const& __lhs,
    box<_T2, _A2> const& __rhs) noexcept(noexcept(*__lhs >= *__rhs))
    -> std::common_type_t<bool, decltype(*__lhs != *__rhs)> {
  if (not __lhs and not __rhs) {
    return true;
  } else if (__lhs and not __rhs) {
    return true;
  } else if (__rhs and not __lhs) {
    return false;
  } else {
    return *__lhs >= *__rhs;
  }
}

} // namespace ublib
