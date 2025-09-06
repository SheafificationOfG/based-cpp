/** ********
 * GIL Standard Library - Strings
 *
 * This header provides support for string literals and
 * string manipulation.
 */

#ifndef GIL_STD_STR_HPP_
#define GIL_STD_STR_HPP_

#include "std.base.hpp"

namespace gil {
namespace std {
namespace str {

namespace _impl_ {

constexpr struct : local {
} _str_idx_;
static constexpr auto idx = global_(_str_idx_);

constexpr struct : local {
} _str_tmp_;
static constexpr auto tmp = global_(_str_tmp_);

} // namespace _impl_

template <unsigned n> static constexpr auto literal(char const (&str)[n]) {
  detail::string::StringLiteral literal = str;
  return lib::ir::IR{literal};
}

static constexpr auto memcpy(auto dst, auto src, auto size) noexcept {
  using _impl_::idx;

  return for_(idx = 0, idx < size, ++idx)(dst[*idx] = src[*idx]);
}

static constexpr auto end(auto ch) noexcept {
  return ch == none_ || ch == '\0';
}

static constexpr auto strlen(auto str) noexcept {
  using _impl_::idx;
  using _impl_::tmp;
  return for_(block_(idx = 0, tmp = str), true,
              ++idx)(if_(end(*tmp[*idx]))(break_(*idx)));
}

static constexpr auto strcpy(auto dst, auto src) noexcept {
  using _impl_::idx;
  return for_(idx = 0, true, ++idx)(dst[*idx] = *src[*idx],
                                    if_(end(src[*idx]))(break_));
}

static constexpr auto puts(auto str) noexcept {
  using _impl_::idx;
  return for_(idx = 0, !end(str[*idx]), ++idx)(putc_(str[*idx]));
}

} // namespace str
} // namespace std
} // namespace gil

#endif // GIL_STD_STR_HPP_
