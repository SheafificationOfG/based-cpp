/** ********
 * GIL Standard Library - I/O
 *
 * This header implements the standard I/O functions for reading
 * and writing elementary data types in C++.
 */

#ifndef GIL_STD_IO_HPP_
#define GIL_STD_IO_HPP_

#include "std.base.hpp"

namespace gil {
namespace std {
namespace io {

namespace _impl_ {

constexpr struct : local {
} _io_idx_;
static constexpr auto idx = global_(_io_idx_);

constexpr struct : local {
} _io_tmp_;
static constexpr auto tmp = global_(_io_tmp_);

} // namespace _impl_

static constexpr auto isspace(auto ch) noexcept {
  using _impl_::tmp;
  return block_(tmp = ch, break_(tmp == ' ' || tmp == '\t' || tmp == '\n' ||
                                 tmp == '\v' || tmp == '\f' || tmp == '\r'));
}

static constexpr auto skip_whitespace() noexcept {
  return loop_(if_(isspace(peek_()))(advance_())->else_(break_));
}

template <typename> static constexpr auto read() noexcept;

template <typename> static constexpr auto write = [](auto...) {};

template <> constexpr auto read<char>() noexcept {
  return block_(skip_whitespace(), break_(getc_()));
}

template <> constexpr auto write<char> = [](auto expr) { return putc_(expr); };

namespace integral {

template <typename Int> static constexpr auto read_int() noexcept {
  using _impl_::tmp;
  constexpr auto is_neg = tmp[0];
  constexpr auto succ = tmp[1];
  return block_(skip_whitespace(), is_neg = peek_() == '-', succ = false,
                tmp = static_cast<Int>(0),
                loop_(if_(peek_(*is_neg) != none_ && peek_(*is_neg) <= '9' &&
                          peek_(*is_neg) >= '0')(
                          tmp = tmp * 10 + cast_<Int>(peek_(*is_neg) - '0'),
                          advance_(), succ = true)
                          ->else_(break_)),
                if_(succ)(if_(is_neg)(advance_(), tmp *= static_cast<Int>(-1)),
                          break_(*tmp)));
}

template <typename Int> static constexpr auto write_int(auto var) noexcept {
  using _impl_::tmp;
  constexpr auto digit = tmp[0];
  constexpr auto magnitude = tmp[1];
  constexpr auto ten = static_cast<Int>(10);
  constexpr auto zero = static_cast<Int>(0);

  return block_(tmp = var, if_(tmp == zero)(putc_('0'), break_),
                if_(tmp < zero)(putc_('-'), tmp *= static_cast<Int>(-1)),
                for_(magnitude = static_cast<Int>(1), magnitude <= tmp,
                     magnitude *= ten)(),
                while_((magnitude /= ten) >
                       zero)(putc_(cast_<char>(tmp / magnitude) + '0'),
                             tmp %= magnitude));
}

} // namespace integral

#define INTIO(signed, int)                                                     \
  template <> constexpr auto read<signed int>() noexcept {                     \
    return integral::read_int<signed int>();                                   \
  }                                                                            \
  template <>                                                                  \
  constexpr auto write<signed int> =                                           \
      [](auto expr) { return integral::write_int<signed int>(expr); };
#define INTIO_PAIR(int)                                                        \
  INTIO(signed, int)                                                           \
  INTIO(unsigned, int)

INTIO_PAIR(char)
INTIO_PAIR(short)
INTIO_PAIR(int)
INTIO_PAIR(long)
INTIO_PAIR(long long)

#undef INTIO_PAIR
#undef INTIO

} // namespace io
} // namespace std
} // namespace gil

#endif // GIL_STD_IO_HPP_
