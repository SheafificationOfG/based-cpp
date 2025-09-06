/** ********
 * GIL Standard Library - Base Language
 *
 * This header implements the C++ language (variables, blocks, keywords).
 */

#ifndef GIL_STD_BASE_HPP_
#define GIL_STD_BASE_HPP_

#include "std.impl.hpp"

namespace gil {
namespace std {

struct local {
  template <typename This> constexpr bool operator==(this This &&, This &&) {
    return true;
  }
};

static constexpr auto peek_(auto offset) noexcept {
  return lib::ir::IR{lib::code::Peek{offset}};
}

static constexpr auto peek_() noexcept { return peek_(0u); }

static constexpr auto advance_(auto offset) noexcept {
  return lib::ir::IR{lib::code::Advance{offset}};
}

static constexpr auto advance_() noexcept { return advance_(1u); }

template <typename To> static constexpr auto cast_(auto expr) noexcept {
  return lib::ir::IR{lib::code::Cast<To, decltype(expr)>{expr}};
}

static constexpr auto getc_() noexcept {
  return lib::ir::IR{lib::code::GetC{}};
}

static constexpr auto putc_(auto code) noexcept {
  return lib::ir::IR{lib::code::PutC{lib::ir::get_code(code)}};
}

static constexpr struct : local {
} _stack_local_scope_;

static constexpr auto global_(auto name) noexcept {
  return lib::ir::IR{lib::code::Var{lib::ir::get_code(name)}};
}

static constexpr auto var_(auto name) noexcept {
  return global_(name)[*global_(_stack_local_scope_)];
}

static constexpr auto bundle(auto... parts) {
  return lib::ir::IR{lib::bundle::Bundle{parts...}};
}

static constexpr auto if_(auto cond) noexcept {
  return [=](auto... code) {
    return lib::ir::IR{lib::code::ctrl::IfBlock<
        decltype(cond), lib::code::ctrl::Block<decltype(code)...>, void>{
        cond, {{code...}}}};
  };
}

static constexpr auto loop_(auto... code) noexcept {
  return lib::ir::IR{lib::code::ctrl::LoopBlock{
      lib::code::ctrl::Block<decltype(code)...>{{code...}}}};
}

static constexpr auto continue_ = lib::code::ctrl::Continue{};
static constexpr auto break_ = lib::code::ctrl::Break{};

static constexpr auto block_(auto... code) { return loop_(code..., break_); }

static constexpr auto while_(auto cond) noexcept {
  return [=](auto... code) { return loop_(if_(cond)(code...)->else_(break_)); };
}

static constexpr auto for_(auto init, auto cond, auto post) noexcept {
  return [=](auto... code) {
    return loop_(init, break_(while_(cond)(code..., post)));
  };
}

namespace _impl_ {

static constexpr struct : local {
} switcher;

template <typename Expr, typename Body> struct Case {
  Expr expr;
  Body body;
};

template <typename Body> struct Default {
  Body body;
};

static constexpr auto switch_impl(auto expr) noexcept { return block_(); }

template <typename Expr, typename Body>
static constexpr auto switch_impl(auto expr, Case<Expr, Body> case_,
                                  auto... cases) noexcept {
  return if_(expr == case_.expr)(case_.body)
      ->else_(switch_impl(expr, cases...));
}

template <typename Body>
static constexpr auto switch_impl(auto expr, Default<Body> default_,
                                  auto... cases) noexcept {
  if constexpr (sizeof...(cases) > 0)
    return switch_impl(cases..., default_);
  else
    return default_.body;
}

} // namespace _impl_

static constexpr auto case_(auto expr) noexcept {
  return [=](auto... body) {
    return _impl_::Case{expr,
                        lib::code::ctrl::Block<decltype(body)...>{{body...}}};
  };
}

static constexpr auto default_(auto... body) noexcept {
  return _impl_::Default{lib::code::ctrl::Block<decltype(body)...>{{body...}}};
}

static constexpr auto switch_(auto expr) noexcept {
  return [=](auto... cases) {
    constexpr auto switcher = global_(_impl_::switcher);
    return block_(switcher = expr, _impl_::switch_impl(*switcher, cases...));
  };
}

static constexpr auto none_ = lib::ir::IR{lib::none::None{}};

namespace _impl_ {

static constexpr struct : local {
} _lambda_return_;

template <typename Args, typename Body> struct Lambda {
  Args args;
  Body body;

  constexpr auto operator()(auto... vars) const noexcept {
    lib::bundle::Bundle varbundle{vars...};
    return lib::ir::IR{loop_(
        ++global_(_stack_local_scope_),
        lib::bundle::fold([](auto arg, auto var) { return var_(arg) = var; },
                          args, varbundle),
        global_(_lambda_return_) = body, --global_(_stack_local_scope_),
        break_(*global_(_lambda_return_)))};
  }
};

} // namespace _impl_

static constexpr auto fn_(auto... args) {
  return [=](auto... code) {
    return _impl_::Lambda{lib::bundle::Bundle{args...}, loop_(code..., break_)};
  };
}

namespace _impl_ {

template <lib::bundle::Bundle code>
using Main = lib::interpret::Interpret<detail::runtime::Start,
                                       lib::bundle::Bundle{
                                           global_(_stack_local_scope_) = 0,
                                           code,
                                       }>;

} // namespace _impl_

#if DEBUG == 0
template <lib::bundle::Bundle code>
static constexpr auto main =
    detail::string::as_literal<typename _impl_::Main<code>::Effect::Stdout>;
#else
template <lib::bundle::Bundle code>
static constexpr auto main =
    detail::runtime::_impl_::Debug<typename _impl_::Main<code>
#if DEBUG == 1
                                   ::Effect
#endif
                                   >{};
#endif

} // namespace std
} // namespace gil

#endif // GIL_STD_BASE_HPP_
