/** ********
 * GIL Standard Library - Implementation
 *
 * This header provides the interpreter engine for translating C++
 * into instructions for the C++ abstract machine.
 */

#ifndef GIL_STD_IMPL_HPP_
#define GIL_STD_IMPL_HPP_

#include "gil.impl.hpp"

namespace gil {
namespace std {

namespace lib {

namespace none {

struct None {
  constexpr bool operator==(None) const noexcept { return true; }
  constexpr bool operator!=(None) const noexcept { return false; }
};

} // namespace none

namespace bundle {

template <typename...> struct Bundle;

template <> struct Bundle<> {};
template <typename T, typename... Ts> struct Bundle<T, Ts...> {
  T head;
  Bundle<Ts...> tail;
  constexpr Bundle(T head, Bundle<Ts...> tail) noexcept
      : head(head), tail(tail) {}
  constexpr Bundle(T head, Ts... tail) noexcept : head(head), tail(tail...) {}

  template <unsigned i> constexpr auto const &get() const noexcept {
    if constexpr (i == 0)
      return head;
    else
      return tail.template get<i - 1>();
  }
  template <unsigned i> constexpr auto &get() noexcept {
    if constexpr (i == 0)
      return head;
    else
      return tail.template get<i - 1>();
  }
};

template <typename... Ts> Bundle(Ts... ts) -> Bundle<Ts...>;

constexpr bool operator==(Bundle<> const &, Bundle<> const &) { return true; }
template <typename... Ts>
  requires(sizeof...(Ts) > 0)
constexpr bool operator==(Bundle<Ts...> const &lhs, Bundle<Ts...> const &rhs) {
  return lhs.head == rhs.head && lhs.tail == rhs.tail;
}

template <typename T, typename... Ts>
static constexpr auto join(T head, Bundle<Ts...> tail) noexcept {
  return Bundle<T, Ts...>{head, tail};
}

template <typename Lambda>
constexpr auto invoke(Lambda lambda, Bundle<> const &) noexcept {
  return lambda();
}

template <typename Lambda, typename T, typename... Ts>
constexpr auto invoke(Lambda lambda, Bundle<T, Ts...> const &bundle) noexcept {
  return invoke([&](Ts... args) { return lambda(bundle.head, args...); },
                bundle.tail);
};

template <typename Lambda>
constexpr auto fold(Lambda, Bundle<> const &, Bundle<> const &) noexcept {
  return Bundle<>{};
}

template <typename Lambda, typename L, typename... Ls, typename R,
          typename... Rs>
constexpr auto fold(Lambda lambda, Bundle<L, Ls...> lhs,
                    Bundle<R, Rs...> rhs) noexcept {
  return join(lambda(lhs.head, rhs.head), fold(lambda, lhs.tail, rhs.tail));
};

} // namespace bundle

namespace code {

template <typename Name> struct Var {
  Name name;
};

template <typename Name> struct Ref {
  Name name;
  constexpr auto operator*() const noexcept { return Var{name}; }
};

template <typename Name, typename Value> struct BoundVar {
  Name name;
  Value value;

  constexpr operator Value() const noexcept { return value; }
  constexpr auto operator&() const noexcept { return Ref{name}; }
  constexpr auto operator*() const noexcept { return value; }
  constexpr auto operator[](auto... params) const noexcept {
    return Var{bundle::Bundle{name, params...}};
  }
};

template <typename Var, typename Expr> struct Assign {
  Var var;
  Expr expr;
};

template <detail::string::StringLiteral, typename... Args> struct Operator {
  bundle::Bundle<Args...> args;
};

template <typename To, typename From> struct Cast {
  From from;
};

template <typename Offset> struct Peek {
  Offset offset;
};

template <typename Offset> struct Advance {
  Offset offset;
};

struct GetC {};

template <typename Ch> struct PutC {
  Ch ch;
};

template <typename Fn, typename... Args> struct Invoke {
  Fn fn;
  bundle::Bundle<Args...> args;
};

namespace ctrl {

struct Continue {};

template <typename T = none::None> struct Break {
  T result;

  constexpr operator T() const noexcept { return result; }
};

template <> struct Break<> {
  none::None result{};

  constexpr operator none::None() const noexcept { return result; }
  constexpr auto operator()() const noexcept { return *this; }
  constexpr auto operator()(auto value) const noexcept {
    return Break<decltype(value)>{value};
  }
};

template <typename... Code> struct Block {
  bundle::Bundle<Code...> code;
};

// These workarounds are unnecessary in clang, but my channel isn't called the
// animated closure of g.
namespace _impl_ {

template <auto code> struct Flow;

template <typename T, Break<T> break_> struct Flow<break_> {};
template <Continue continue_> struct Flow<continue_> {};

template <auto code> struct IsBreak;

template <typename T, Break<T> break_> struct IsBreak<break_> {};

} // namespace _impl_

template <typename Cond, typename IfTrue, typename IfFalse> struct IfBlock {
  Cond cond;
  IfTrue iftrue;
  IfFalse iffalse;
};

template <typename Cond, typename IfTrue> struct IfBlock<Cond, IfTrue, void> {
  Cond cond;
  IfTrue iftrue;
  static constexpr auto iffalse = bundle::Bundle<>{};

  template <typename... Else>
  constexpr auto else_(Else... iffalse) const noexcept {
    return IfBlock<Cond, IfTrue, Block<Else...>>{cond, iftrue, {iffalse...}};
  }
};

template <typename Code> struct LoopBlock {
  Code code;
};

} // namespace ctrl

} // namespace code

namespace op {

namespace _impl_ {

template <typename Name, typename Value>
constexpr auto deref(code::BoundVar<Name, Value> bv) noexcept {
  return bv.value;
}

constexpr auto deref(auto other) noexcept { return other; }

} // namespace _impl_

template <detail::string::StringLiteral, auto...> struct PureOp;

#define PURE_UN_OP(O)                                                          \
  template <auto arg>                                                          \
    requires requires { O arg; }                                               \
  struct PureOp<#O "#", arg> {                                                 \
    static constexpr auto result = O arg;                                      \
  };
#define MOD_UN_OP(O)                                                           \
  template <auto arg>                                                          \
    requires requires(decltype(arg) copy) { O##O copy; }                       \
  struct PureOp<#O #O "#", arg> {                                              \
    static constexpr auto result = [] {                                        \
      auto copy = arg;                                                         \
      return O##O copy;                                                        \
    }();                                                                       \
  };
#define POST_UN_OP(O)                                                          \
  template <auto arg>                                                          \
    requires requires(decltype(arg) copy) { copy O##O; }                       \
  struct PureOp<"#" #O #O, arg> {                                              \
    static constexpr auto result = [] {                                        \
      auto copy = arg;                                                         \
      copy O##O;                                                               \
      return copy;                                                             \
    }();                                                                       \
  };
#define PURE_BIN_OP(O)                                                         \
  template <auto lhs, auto rhs>                                                \
    requires requires { _impl_::deref(lhs) O _impl_::deref(rhs); }             \
  struct PureOp<#O, lhs, rhs> {                                                \
    static constexpr auto result = _impl_::deref(lhs) O _impl_::deref(rhs);    \
  };
#define ASSIGN_BIN_OP(O)                                                       \
  template <auto lhs, auto rhs>                                                \
    requires requires(decltype(lhs) l) { l O## = _impl_::deref(rhs); }         \
  struct PureOp<#O "=", lhs, rhs> {                                            \
    static constexpr auto result = [] {                                        \
      auto copy = lhs;                                                         \
      copy O## = _impl_::deref(rhs);                                           \
      return copy;                                                             \
    }();                                                                       \
  };                                                                           \
  template <auto lhs, auto rhs>                                                \
    requires(                                                                  \
        !requires(decltype(lhs) l) { l O## = _impl_::deref(rhs); } &&          \
        requires { _impl_::deref(lhs) O _impl_::deref(rhs); })                 \
  struct PureOp<#O "=", lhs, rhs> {                                            \
    static constexpr auto result = _impl_::deref(lhs) O _impl_::deref(rhs);    \
  };
#include "ops.inc"

template <auto lhs, auto rhs>
  requires(!requires { _impl_::deref(lhs) == _impl_::deref(rhs); })
struct PureOp<"==", lhs, rhs> {
  static constexpr auto result = false;
};

template <auto lhs, auto rhs>
  requires(!requires { _impl_::deref(lhs) == _impl_::deref(rhs); })
struct PureOp<"!=", lhs, rhs> {
  static constexpr auto result = true;
};

} // namespace op

namespace ir {

namespace _impl_ {

template <auto ir> struct Compile {
  static constexpr auto expr = detail::expr::val<ir>;
};

template <code::Var var> struct Compile<var> {
  static constexpr auto expr = detail::expr::var<var.name>;
};

template <typename V> struct Value;

template <auto x> struct Value<detail::type::Value<x>> {
  static constexpr auto get = x;
};
template <> struct Value<none::None> {
  static constexpr auto get = none::None{};
};

} // namespace _impl_

template <auto ir> static constexpr auto compile = _impl_::Compile<ir>::expr;

template <typename V> static constexpr auto value = _impl_::Value<V>::get;

} // namespace ir

namespace ir {

template <typename Code> struct IR;

template <typename Code> constexpr auto get_code(IR<Code> const &ir) {
  return ir.code;
}
constexpr auto get_code(auto other) { return other; }

template <typename Code> struct IR {
  Code code;

  constexpr Code const *operator->() const noexcept { return &code; }

#define UN_OP(O)                                                               \
  constexpr auto operator O() const noexcept {                                 \
    code::Operator<#O "#", decltype(code)> op{{code}};                         \
    return IR<decltype(op)>{op};                                               \
  }
#define POST_UN_OP(O)                                                          \
  constexpr auto operator O##O(int) const noexcept {                           \
    code::Operator<"#" #O #O, decltype(code)> op{{code}};                      \
    return IR<decltype(op)>{op};                                               \
  }
#define BIN_OP(O)                                                              \
  constexpr auto operator O(auto rhs) const noexcept {                         \
    code::Operator<#O, decltype(code), decltype(get_code(rhs))> op{            \
        {code, get_code(rhs)}};                                                \
    return IR<decltype(op)>{op};                                               \
  }
#define POLY_OP(O)                                                             \
  constexpr auto operator O(auto... args) const noexcept {                     \
    code::Operator<#O, decltype(code), decltype(get_code(args))...> op{        \
        {code, get_code(args)...}};                                            \
    return IR<decltype(op)>{op};                                               \
  }

#include "ops.inc"
};

#define PURE_BIN_OP(O)                                                         \
  template <typename Lhs, typename Code>                                       \
    requires(!requires(Lhs lhs) { lhs.code; })                                 \
  constexpr auto operator O(Lhs lhs, IR<Code> const &ir) noexcept {            \
    code::Operator<#O, Lhs, decltype(ir.code)> op{{lhs, ir.code}};             \
    return IR<decltype(op)>{op};                                               \
  }
#include "ops.inc"

} // namespace ir

namespace interpret {

template <typename Runtime, auto code> struct Interpret {
  using Effect = Runtime;
  static constexpr auto retval = code;
};

template <typename Runtime, bundle::Bundle<> bundle>
struct Interpret<Runtime, bundle> {
  using Effect = Runtime;
  static constexpr auto retval = bundle;
};

template <typename Runtime, typename T, typename... Ts,
          bundle::Bundle<T, Ts...> bundle>
struct Interpret<Runtime, bundle> {
  using InterpretHead = Interpret<Runtime, bundle.head>;
  using InterpretTail = Interpret<typename InterpretHead::Effect, bundle.tail>;

  using Effect = InterpretTail::Effect;
  static constexpr auto retval =
      bundle::join(InterpretHead::retval, InterpretTail::retval);
};

template <typename Runtime, code::Var var> struct Interpret<Runtime, var> {
  using InterpretName = Interpret<Runtime, var.name>;

  using Effect = InterpretName::Effect;
  static constexpr auto retval = code::BoundVar{
      InterpretName::retval,
      detail::tfunc::GetItem<typename Effect::State, InterpretName::retval,
                             detail::type::Value<none::None{}>>::value};
};

template <typename Runtime, code::Ref ref> struct Interpret<Runtime, ref> {
  using InterpretName = Interpret<Runtime, ref.name>;

  using Effect = InterpretName::Effect;
  static constexpr auto retval = code::Ref{InterpretName::retval};
};

template <typename Runtime, code::Assign assign>
struct Interpret<Runtime, assign> {
  using InterpretVar = Interpret<Runtime, assign.var>;
  using InterpretExpr = Interpret<typename InterpretVar::Effect, assign.expr>;

  static constexpr auto retval = InterpretExpr::retval;
  using Effect = InterpretExpr::Effect::template Run<
      detail::exec::Set<InterpretVar::retval.name, ir::compile<retval>>>;
};

template <typename Runtime, typename Lhs, typename Rhs,
          code::Operator<"=", Lhs, Rhs> assign>
struct Interpret<Runtime, assign> {
  using InterpretArgs = Interpret<Runtime, assign.args>;

  static constexpr auto retval = InterpretArgs::retval.template get<1>();
  using Effect = InterpretArgs::Effect::template Run<detail::exec::Set<
      InterpretArgs::retval.template get<0>().name, ir::compile<retval>>>;
};

namespace _impl_ {
template <detail::string::StringLiteral, bool, auto, auto>
struct ShortCircuit {};
} // namespace _impl_

template <typename Runtime, detail::string::StringLiteral scop, bool want,
          auto lhs, auto rhs, _impl_::ShortCircuit<scop, want, lhs, rhs> sc>
  requires(static_cast<bool>(lhs) == want)
struct Interpret<Runtime, sc> {
  using Effect = Runtime;
  static constexpr auto retval = want;
};

template <typename Runtime, detail::string::StringLiteral scop, bool want,
          auto lhs, auto rhs, _impl_::ShortCircuit<scop, want, lhs, rhs> sc>
  requires(!requires { static_cast<bool>(lhs); } ||
           static_cast<bool>(lhs) != want)
struct Interpret<Runtime, sc> {
  using InterpretOther = Interpret<Runtime, rhs>;
  static constexpr auto result =
      op::PureOp<scop, lhs, InterpretOther::retval>::result;

  using InterpretResult = Interpret<typename InterpretOther::Effect, result>;

  using Effect = InterpretResult::Effect;
  static constexpr auto retval = InterpretResult::retval;
};

#define PURE_UN_OP(O)                                                          \
  template <typename Runtime, typename Arg, code::Operator<#O "#", Arg> op>    \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto result =                                             \
        op::PureOp<#O "#", InterpretArgs::retval.template get<0>()>::result;   \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    using Effect = InterpretResult::Effect;                                    \
    static constexpr auto retval = InterpretResult::retval;                    \
  };
#define MOD_UN_OP(O)                                                           \
  template <typename Runtime, typename Arg, code::Operator<#O #O "#", Arg> op> \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto arg = InterpretArgs::retval.template get<0>();       \
    static constexpr auto result = op::PureOp<#O #O "#", arg.value>::result;   \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    static constexpr auto retval = InterpretResult::retval;                    \
    using Effect = InterpretResult::Effect::template Run<                      \
        detail::exec::Set<arg.name, ir::compile<retval>>>;                     \
  };
#define POST_UN_OP(O)                                                          \
  template <typename Runtime, typename Arg, code::Operator<"#" #O #O, Arg> op> \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto arg = InterpretArgs::retval.template get<0>();       \
    static constexpr auto result = op::PureOp<"#" #O #O, arg.value>::result;   \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    static constexpr auto retval = arg.value;                                  \
    using Effect = InterpretResult::Effect::template Run<                      \
        detail::exec::Set<arg.name, ir::compile<InterpretResult::retval>>>;    \
  };
#define PURE_BIN_OP(O)                                                         \
  template <typename Runtime, typename Lhs, typename Rhs,                      \
            code::Operator<#O, Lhs, Rhs> op>                                   \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto result =                                             \
        op::PureOp<#O, InterpretArgs::retval.template get<0>(),                \
                   InterpretArgs::retval.template get<1>()>::result;           \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    using Effect = InterpretResult::Effect;                                    \
    static constexpr auto retval = InterpretResult::retval;                    \
  };
#define ASSIGN_BIN_OP(O)                                                       \
  template <typename Runtime, typename Lhs, typename Rhs,                      \
            code::Operator<#O "=", Lhs, Rhs> op>                               \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto result =                                             \
        op::PureOp<#O "=", InterpretArgs::retval.template get<0>().value,      \
                   InterpretArgs::retval.template get<1>()>::result;           \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    using Effect = InterpretResult::Effect::template Run<                      \
        detail::exec::Set<InterpretArgs::retval.template get<0>().name,        \
                          ir::compile<InterpretResult::retval>>>;              \
    static constexpr auto retval = InterpretResult::retval;                    \
  };
#define POLY_OP(O)                                                             \
  template <typename Runtime, typename Call, typename... Args,                 \
            code::Operator<#O, Call, Args...> op>                              \
  struct Interpret<Runtime, op> {                                              \
    using InterpretArgs = Interpret<Runtime, op.args>;                         \
    static constexpr auto result = bundle::invoke(                             \
        [](auto... args) {                                                     \
          return InterpretArgs::retval.head.operator O(args...);               \
        },                                                                     \
        InterpretArgs::retval.tail);                                           \
                                                                               \
    using InterpretResult = Interpret<typename InterpretArgs::Effect, result>; \
                                                                               \
    using Effect = InterpretResult::Effect;                                    \
    static constexpr auto retval = InterpretResult::retval;                    \
  };
#define NO_SHORTCIRCUIT_OPS
#include "ops.inc"

template <typename Runtime, typename Lhs, typename Rhs,
          code::Operator<"&&", Lhs, Rhs> op>
struct Interpret<Runtime, op> {
  using InterpretLhs = Interpret<Runtime, op.args.template get<0>()>;
  using InterpretOp =
      Interpret<typename InterpretLhs::Effect,
                _impl_::ShortCircuit<"&&", false, InterpretLhs::retval,
                                     op.args.template get<1>()>{}>;

  using Effect = InterpretOp::Effect;
  static constexpr auto retval = InterpretOp::retval;
};

template <typename Runtime, typename Lhs, typename Rhs,
          code::Operator<"||", Lhs, Rhs> op>
struct Interpret<Runtime, op> {
  using InterpretLhs = Interpret<Runtime, op.args.template get<0>()>;
  using InterpretOp =
      Interpret<typename InterpretLhs::Effect,
                _impl_::ShortCircuit<"||", true, InterpretLhs::retval,
                                     op.args.template get<1>()>{}>;

  using Effect = InterpretOp::Effect;
  static constexpr auto retval = InterpretOp::retval;
};

template <typename Runtime, typename To, typename From,
          code::Cast<To, From> cast>
struct Interpret<Runtime, cast> {
  using InterpretFrom = Interpret<Runtime, cast.from>;

  using Effect = InterpretFrom::Effect;
  static constexpr auto retval = static_cast<To>(InterpretFrom::retval);
};

template <typename Runtime, code::Peek peek> struct Interpret<Runtime, peek> {
  using InterpretOffset = Interpret<Runtime, peek.offset>;

  using Effect = InterpretOffset::Effect;
  static constexpr auto retval =
      ir::value<detail::tfunc::Get<typename InterpretOffset::Effect::Stdin,
                                   InterpretOffset::retval, none::None>>;
};

template <typename Runtime, code::Advance advance>
struct Interpret<Runtime, advance> {
  using InterpretOffset = Interpret<Runtime, advance.offset>;

  using Effect = InterpretOffset::Effect::template Run<
      detail::exec::Advance<detail::expr::val<InterpretOffset::retval>>>;
  static constexpr auto retval = none::None{};
};

template <typename Runtime, code::GetC getc> struct Interpret<Runtime, getc> {
  using Effect = Runtime::template Run<detail::exec::Advance<>>;
  static constexpr auto retval =
      ir::value<detail::tfunc::Get<typename Runtime::Stdin, 0, none::None>>;
};

template <typename Runtime, code::PutC putc> struct Interpret<Runtime, putc> {
  using InterpretCh = Interpret<Runtime, putc.ch>;

  static constexpr auto retval = InterpretCh::retval;
  using Effect =
      InterpretCh::Effect::template Run<detail::exec::Put<ir::compile<retval>>>;
};

template <typename Runtime, code::ctrl::Block<> block>
struct Interpret<Runtime, block> {
  using Effect = Runtime;
  static constexpr auto retval = none::None{};
};

namespace _impl_ {

template <auto prev, auto next> struct BlockPhase {};

} // namespace _impl_

template <typename Runtime, typename Head, typename... Code,
          code::ctrl::Block<Head, Code...> block>
struct Interpret<Runtime, block> {
  using InterpretHead = Interpret<Runtime, block.code.head>;
  using InterpretTail =
      Interpret<typename InterpretHead::Effect,
                _impl_::BlockPhase<InterpretHead::retval,
                                   code::ctrl::Block{block.code.tail}>{}>;

  using Effect = InterpretTail::Effect;
  static constexpr auto retval = InterpretTail::retval;
};

template <typename Runtime, auto prev, auto next,
          _impl_::BlockPhase<prev, next> phase>
  requires(!requires { code::ctrl::_impl_::Flow<prev>{}; })
struct Interpret<Runtime, phase> {
  using InterpretNext = Interpret<Runtime, next>;

  using Effect = InterpretNext::Effect;
  static constexpr auto retval = InterpretNext::retval;
};

template <typename Runtime, code::ctrl::Continue continue_, auto next,
          _impl_::BlockPhase<continue_, next> phase>
struct Interpret<Runtime, phase> {
  using Effect = Runtime;
  static constexpr auto retval = continue_;
};

template <typename Runtime, typename T, code::ctrl::Break<T> break_, auto next,
          _impl_::BlockPhase<break_, next> phase>
struct Interpret<Runtime, phase> {
  using InterpretBreak = Interpret<Runtime, break_.result>;

  using Effect = InterpretBreak::Effect;
  static constexpr auto retval = code::ctrl::Break{InterpretBreak::retval};
};

namespace _impl_ {

template <bool, auto iftrue, auto iffalse> struct Branch {};

template <auto prev_iter, auto loop> struct Loop {};

} // namespace _impl_

template <typename Runtime, typename Cond, typename IfTrue, typename IfFalse,
          code::ctrl::IfBlock<Cond, IfTrue, IfFalse> if_>
struct Interpret<Runtime, if_> {
  using InterpretCond = Interpret<Runtime, if_.cond>;
  using InterpretBranch = Interpret<
      typename InterpretCond::Effect,
      _impl_::Branch<InterpretCond::retval, if_.iftrue, if_.iffalse>{}>;

  using Effect = InterpretBranch::Effect;
  static constexpr auto retval = InterpretBranch::retval;
};

template <typename Runtime, auto iftrue, auto iffalse,
          _impl_::Branch<true, iftrue, iffalse> branch>
struct Interpret<Runtime, branch> {
  using InterpretTrue = Interpret<Runtime, iftrue>;

  using Effect = InterpretTrue::Effect;
  static constexpr auto retval = InterpretTrue::retval;
};

template <typename Runtime, auto iftrue, auto iffalse,
          _impl_::Branch<false, iftrue, iffalse> branch>
struct Interpret<Runtime, branch> {
  using InterpretFalse = Interpret<Runtime, iffalse>;

  using Effect = InterpretFalse::Effect;
  static constexpr auto retval = InterpretFalse::retval;
};

template <typename Runtime, typename Code, code::ctrl::LoopBlock<Code> loop>
struct Interpret<Runtime, loop> {
  using InterpretLoop =
      Interpret<Runtime, _impl_::Loop<code::ctrl::Continue{}, loop.code>{}>;

  using Effect = InterpretLoop::Effect;
  static constexpr auto retval = InterpretLoop::retval;
};

template <typename Runtime, typename T, code::ctrl::Break<T> break_, auto code,
          _impl_::Loop<break_, code> loop>
struct Interpret<Runtime, loop> {
  using InterpretBreak = Interpret<Runtime, break_.result>;

  using Effect = InterpretBreak::Effect;
  static constexpr auto retval = InterpretBreak::retval;
};

template <typename Runtime, auto prev_iter, auto code,
          _impl_::Loop<prev_iter, code> loop>
  requires(!requires { code::ctrl::_impl_::IsBreak<prev_iter>{}; })
struct Interpret<Runtime, loop> {
  using InterpretOnce = Interpret<Runtime, code>;
  using InterpretLoop = Interpret<typename InterpretOnce::Effect,
                                  _impl_::Loop<InterpretOnce::retval, code>{}>;

  using Effect = InterpretLoop::Effect;
  static constexpr auto retval = InterpretLoop::retval;
};

template <typename Runtime, typename Code, ir::IR<Code> ir>
struct Interpret<Runtime, ir> {
  using InterpretCode = Interpret<Runtime, ir.code>;

  using Effect = InterpretCode::Effect;
  static constexpr auto retval = InterpretCode::retval;
};

} // namespace interpret

} // namespace lib

} // namespace std

} // namespace gil

#endif // GIL_STD_IMPL_HPP_
