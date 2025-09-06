/** ********
 * GNU Interface Layer - Implementation
 *
 * This header implements the C++ abstract machine and the behaviour
 * of its insrtuction set.
 */

#ifndef GIL_IMPL_HPP_
#define GIL_IMPL_HPP_

namespace gil {

namespace detail {

namespace type {
struct None;
struct Undefined;
template <auto> struct Value;
} // namespace type

namespace tfunc {

namespace _impl_ {

template <typename X> struct Len;

template <template <auto...> typename X, auto... xs> struct Len<X<xs...>> {
  using Result = type::Value<sizeof...(xs)>;
};

template <template <typename...> typename X, typename... Ts>
struct Len<X<Ts...>> {
  using Result = type::Value<sizeof...(Ts)>;
};

template <typename... Xs> struct Join;

template <typename X> struct Join<X> {
  using Result = X;
};

template <template <auto...> typename X, auto... ls, auto... rs, typename... Xs>
struct Join<X<ls...>, X<rs...>, Xs...> {
  using Result = Join<X<ls..., rs...>, Xs...>::Result;
};

template <template <typename...> typename X, typename... Ls, typename... Rs,
          typename... Xs>
struct Join<X<Ls...>, X<Rs...>, Xs...> {
  using Result = Join<X<Ls..., Rs...>, Xs...>::Result;
};

template <typename X, typename... Vs> struct Push;

template <template <auto...> typename X, auto... xs, template <auto> typename V,
          auto... vs>
struct Push<X<xs...>, V<vs>...> {
  using Result = X<xs..., vs...>;
};

template <template <typename...> typename X, typename... Ts, typename... Vs>
struct Push<X<Ts...>, Vs...> {
  using Result = X<Ts..., Vs...>;
};

template <typename X, typename... Vs> struct PushFront;

template <template <auto...> typename X, auto... xs, template <auto> typename V,
          auto... vs>
struct PushFront<X<xs...>, V<vs>...> {
  using Result = X<vs..., xs...>;
};

template <template <typename...> typename X, typename... Ts, typename... Vs>
struct PushFront<X<Ts...>, Vs...> {
  using Result = X<Vs..., Ts...>;
};

template <typename X, unsigned n> struct Pop;

template <typename X> struct Pop<X, 0> {
  using Result = X;
};

template <template <auto...> typename X, unsigned n>
  requires(n > 0)
struct Pop<X<>, n> {
  using Result = X<>;
};

template <template <auto...> typename X, auto x, auto... xs, unsigned n>
  requires(n > 0)
struct Pop<X<x, xs...>, n> {
  using Result = Pop<X<xs...>, n - 1>::Result;
};

template <template <typename...> typename X, typename T, typename... Ts,
          unsigned n>
  requires(n > 0)
struct Pop<X<T, Ts...>, n> {
  using Result = Pop<X<Ts...>, n - 1>::Result;
};

template <typename X, unsigned i, typename Default> struct Get;

template <template <auto...> typename X, unsigned i, typename Default>
struct Get<X<>, i, Default> {
  using Result = Default;
};

template <template <auto...> typename X, auto x, auto... xs, typename Default>
struct Get<X<x, xs...>, 0, Default> {
  using Result = type::Value<x>;
};

template <template <auto...> typename X, auto x, auto... xs, unsigned i,
          typename Default>
struct Get<X<x, xs...>, i, Default> {
  using Result = Get<X<xs...>, i - 1, Default>::Result;
};

template <template <typename...> typename X, unsigned i, typename Default>
struct Get<X<>, i, Default> {
  using Result = Default;
};

template <template <typename...> typename X, typename T, typename... Ts,
          typename Default>
struct Get<X<T, Ts...>, 0, Default> {
  using Result = T;
};

template <template <typename...> typename X, typename T, typename... Ts,
          unsigned i, typename Default>
struct Get<X<T, Ts...>, i, Default> {
  using Result = Get<X<Ts...>, i - 1, Default>::Result;
};

template <typename X, unsigned lo, unsigned hi> struct Slice;

template <template <auto...> typename X, auto... xs, unsigned lo, unsigned hi>
  requires(lo >= hi)
struct Slice<X<xs...>, lo, hi> {
  using Result = X<>;
};

template <template <auto...> typename X, auto x, auto... xs, unsigned lo,
          unsigned hi>
struct Slice<X<x, xs...>, lo, hi> {
  using Result =
      Join<typename Slice<X<xs...>, lo, hi - 1>::Result, X<x>>::Result;
};

template <typename X, unsigned i, typename V> struct Set;

template <template <typename...> typename X, typename... Ts, unsigned i,
          typename V>
struct Set<X<Ts...>, i, V> {
  using Result =
      Join<typename Slice<X<Ts...>, 0, i>::Result, X<V>,
           typename Slice<X<Ts...>, i + 1, sizeof...(Ts)>::Result>::Result;
};

template <template <auto...> typename X, template <auto> typename V, auto... xs,
          unsigned i, auto x>
struct Set<X<xs...>, i, V<x>> {
  using Result =
      Join<typename Slice<X<xs...>, 0, i>::Result, X<x>,
           typename Slice<X<xs...>, i + 1, sizeof...(xs)>::Result>::Result;
};

template <typename Map, auto key, typename Default> struct GetItem;

template <template <typename...> typename Map, auto key, typename Default>
struct GetItem<Map<>, key, Default> {
  using Result = Default;
};

template <template <typename...> typename Map,
          template <auto, typename> typename Entry, typename... Entries, auto k,
          typename V, auto key, typename Default>
  requires(k == key)
struct GetItem<Map<Entry<k, V>, Entries...>, key, Default> {
  using Result = V;
};

template <template <typename...> typename Map, typename Entry,
          typename... Entries, auto key, typename Default>
struct GetItem<Map<Entry, Entries...>, key, Default> {
  using Result = GetItem<Map<Entries...>, key, Default>::Result;
};

template <typename Map, typename Entry> struct SetItem;

template <template <typename...> typename Map,
          template <auto, typename> typename Entry, auto key, typename V>
struct SetItem<Map<>, Entry<key, V>> {
  using Result = Map<Entry<key, V>>;
};

template <template <typename...> typename Map,
          template <auto, typename> typename Entry, typename... Entries,
          auto key, typename V, typename W>
struct SetItem<Map<Entry<key, W>, Entries...>, Entry<key, V>> {
  using Result = Map<Entry<key, V>, Entries...>;
};

template <template <typename...> typename Map,
          template <auto, typename> typename Entry, typename Head,
          typename... Entries, auto key, typename V>
struct SetItem<Map<Head, Entries...>, Entry<key, V>> {
  using Result =
      Join<Map<Head>,
           typename SetItem<Map<Entries...>, Entry<key, V>>::Result>::Result;
};

} // namespace _impl_

template <typename X> using Len = _impl_::Len<X>::Result;

template <typename... Xs> using Join = _impl_::Join<Xs...>::Result;

template <typename X, typename... Vs>
using Push = _impl_::Push<X, Vs...>::Result;

template <typename X, typename... Vs>
using PushFront = _impl_::PushFront<X, Vs...>::Result;

template <typename X, unsigned n> using Pop = _impl_::Pop<X, n>::Result;

template <typename X, unsigned i, typename Default = type::Undefined>
using Get = _impl_::Get<X, i, Default>::Result;

template <typename X, unsigned lo, unsigned hi>
using Slice = _impl_::Slice<X, lo, hi>::Result;

template <typename X, unsigned i, typename V>
using Set = _impl_::Set<X, i, V>::Result;

template <typename Map, auto key, typename Default = type::Undefined>
using GetItem = _impl_::GetItem<Map, key, Default>::Result;

template <typename Map, typename Entry>
using SetItem = _impl_::SetItem<Map, Entry>::Result;

} // namespace tfunc

namespace type {

template <auto x> struct Value {
  static constexpr auto value = x;

#define UN_OP(O)                                                               \
  constexpr auto operator O() const noexcept { return Value<(O x)>{}; }
#define POST_UN_OP(O)                                                          \
  constexpr auto operator O##O(int) const noexcept { return Value<(x O##O)>{}; }
#define BIN_OP(O)                                                              \
  template <auto y> constexpr auto operator O(Value<y>) const noexcept {       \
    return Value<(x O y)>{};                                                   \
  }
#define POLY_OP(O)                                                             \
  template <auto... ys>                                                        \
  constexpr auto operator O(Value<ys>...) const noexcept {                     \
    return Value<(x.operator O(ys...))>{};                                     \
  }
#include "ops.inc"
};

struct None {
  constexpr auto operator==(None) const noexcept { return Value<true>{}; }
  constexpr auto operator!=(None) const noexcept { return Value<false>{}; }
  template <typename T> friend constexpr auto operator==(None, T) noexcept {
    return Value<false>{};
  }
  template <typename T> friend constexpr auto operator==(T, None) noexcept {
    return Value<false>{};
  }
  template <typename T> friend constexpr auto operator!=(None, T) noexcept {
    return Value<true>{};
  }
  template <typename T> friend constexpr auto operator!=(T, None) noexcept {
    return Value<true>{};
  }
};

template <auto... xs> struct Tuple {
  template <template <auto...> typename Y, auto... ys>
  constexpr auto operator+(Y<ys...>) const noexcept {
    return Tuple<xs..., ys...>{};
  }
  template <template <auto> typename V, auto i>
  constexpr auto operator[](V<i>) const noexcept {
    return tfunc::Get<Tuple<xs...>, i>{};
  }
  template <template <auto> typename V, auto i, template <auto> typename Y,
            auto y>
  constexpr auto operator()(V<i>, Y<y>) const noexcept {
    return tfunc::Set<Tuple<xs...>, i, Y<y>>{};
  }
};

namespace _impl_ {
template <typename T> struct Vector {
  template <T... ts> struct Impl {
    template <template <T...> typename W, T... more>
    constexpr auto operator+(W<more...>) const noexcept {
      return Impl<ts..., more...>{};
    }
    template <template <auto> typename V, auto i>
    constexpr auto operator[](V<i>) const noexcept {
      return tfunc::Get<Impl<ts...>, i>{};
    }
    template <template <auto> typename V, auto i, template <auto> typename Y,
              T t>
    constexpr auto operator()(V<i>, Y<t>) const noexcept {
      return tfunc::Set<Impl<ts...>, i, Y<t>>{};
    }
  };
};
} // namespace _impl_

template <typename T, T... ts>
using Vector = _impl_::Vector<T>::template Impl<ts...>;

template <typename...> struct Pack {};

template <auto key, typename V> struct MapEntry {};

} // namespace type

namespace string {

template <unsigned n> struct StringLiteral {
  char str[n + 1];
  constexpr StringLiteral(char const *ptr) noexcept {
    for (unsigned i = 0; i < n; ++i)
      str[i] = ptr[i];
    str[n] = '\0';
  }
  static constexpr unsigned size = n;

  constexpr auto operator[](unsigned idx) const noexcept { return str[idx]; }
};

template <unsigned n> StringLiteral(char const (&)[n]) -> StringLiteral<n - 1>;

template <char... cs> using String = type::Vector<char, cs...>;

namespace _impl_ {
template <unsigned n, StringLiteral<n> str, unsigned m> struct SplitFrom {
  using Result = tfunc::PushFront<typename SplitFrom<n, str, m + 1>::Result,
                                  type::Value<str.str[m]>>;
};

template <unsigned n, StringLiteral<n> str, unsigned m>
  requires(m >= n)
struct SplitFrom<n, str, m> {
  using Result = String<>;
};

template <typename String> struct Join;

template <char... cs> struct Join<String<cs...>> {
  static constexpr auto result = [] {
    char result[] = {cs..., '\0'};
    return StringLiteral(result);
  }();
};

} // namespace _impl_

template <StringLiteral str>
using ToString = _impl_::SplitFrom<str.size, str, 0>::Result;

template <typename String>
static constexpr auto as_literal = _impl_::Join<String>::result;

} // namespace string

namespace expr {

template <typename Runtime, auto expr>
using Eval = decltype(expr)::template Eval<Runtime>;

namespace _impl_ {

template <auto op, typename... Args> struct Expr {
  template <typename Runtime>
  using Eval = decltype(op(typename Args::template Eval<Runtime>{}...));

#define UN_OP(O)                                                               \
  constexpr auto operator O() const noexcept {                                 \
    return Expr<[](auto self) { return O self; }, Expr<op, Args...>>{};        \
  }
#define POST_UN_OP(O)                                                          \
  constexpr auto operator O##O(int) const noexcept {                           \
    return Expr<[](auto self) { return self O##O; }, Expr<op, Args...>>{};     \
  }
#define BIN_OP(O)                                                              \
  template <typename Rhs> constexpr auto operator O(Rhs) const noexcept {      \
    return Expr<[](auto self, auto rhs) { return (self O rhs); },              \
                Expr<op, Args...>, Rhs>{};                                     \
  }
#define POLY_OP(O)                                                             \
  template <typename... Rhs>                                                   \
  constexpr auto operator O(Rhs...) const noexcept {                           \
    return Expr<[](auto self, auto... rhs) {                                   \
      return self.operator O(rhs...);                                          \
    },                                                                         \
                Expr<op, Args...>, Rhs...>{};                                  \
  }
#include "ops.inc"
};

constexpr auto id = [](auto x) { return x; };
template <typename E> static constexpr auto expr = Expr<id, E>{};

struct None {
  template <typename Runtime> using Eval = type::None;
};

template <auto value> struct Val {
  template <typename Runtime> using Eval = type::Value<value>;
};

template <auto... xs> struct Tuple {
  template <typename Runtime> using Eval = type::Tuple<xs...>;
};

template <typename T, T... ts> struct Vec {
  template <typename Runtime> using Eval = type::Vector<T, ts...>;
};

template <string::StringLiteral s> struct Str {
  template <typename Runtime> using Eval = string::ToString<s>;
};

template <auto var> struct Var {
  template <typename Runtime>
  using Eval = tfunc::GetItem<typename Runtime::State, var>;
};

template <auto expr> struct Peek {
  template <typename Runtime>
  using Eval = tfunc::Get<typename Runtime::Stdin, Eval<Runtime, expr>::value,
                          type::None>;
};

template <auto expr> struct Len {
  template <typename Runtime> using Eval = tfunc::Len<Eval<Runtime, expr>>;
};

} // namespace _impl_

template <auto op, auto... args>
static constexpr auto expr = _impl_::Expr<op, decltype(args)...>{};
static constexpr auto none = _impl_::expr<_impl_::None>;
template <auto x> static constexpr auto val = _impl_::expr<_impl_::Val<x>>;
template <auto... xs>
static constexpr auto tuple = _impl_::expr<_impl_::Tuple<xs...>>;
template <typename T, T... ts>
static constexpr auto vec = _impl_::expr<_impl_::Vec<T, ts...>>;
template <string::StringLiteral s>
static constexpr auto str = _impl_::expr<_impl_::Str<s>>;
template <auto v> static constexpr auto var = _impl_::expr<_impl_::Var<v>>;
template <auto expr = val<0u>>
static constexpr auto peek = _impl_::expr<_impl_::Peek<expr>>;
template <auto expr>
static constexpr auto len = _impl_::expr<_impl_::Len<expr>>;

} // namespace expr

namespace exec {

template <auto var, auto expr> struct Set {
  template <typename Runtime>
  using Run = Runtime::template WithState<tfunc::SetItem<
      typename Runtime::State, type::MapEntry<var, expr::Eval<Runtime, expr>>>>;
};

template <auto expr = expr::val<1u>> struct Advance {
  template <typename Runtime>
  using Run = Runtime::template WithStdin<
      tfunc::Pop<typename Runtime::Stdin, expr::Eval<Runtime, expr>::value>>;
};

template <auto... exprs> struct Put {
  template <typename Runtime>
  using Run = Runtime::template WithStdout<
      tfunc::Push<typename Runtime::Stdout, expr::Eval<Runtime, exprs>...>>;
};

template <typename... Instructions> struct Block;

template <> struct Block<> {
  template <typename Runtime> using Run = Runtime;
};

template <typename Instruction, typename... Instructions>
struct Block<Instruction, Instructions...> {
  template <typename Runtime>
  using Run = Block<Instructions...>::template Run<
      typename Instruction::template Run<Runtime>>;
};

namespace _impl_ {

template <bool cond, typename True, typename False> struct Branch;

template <typename True, typename False> struct Branch<true, True, False> {
  using Result = True;
};
template <typename True, typename False> struct Branch<false, True, False> {
  using Result = False;
};

} // namespace _impl_

template <auto expr, typename Then, typename Else = Block<>> struct If {
  template <typename Runtime>
  using Run =
      _impl_::Branch<static_cast<bool>(expr::Eval<Runtime, expr>::value), Then,
                     Else>::Result::template Run<Runtime>;
};

template <auto expr, typename Loop> struct While {
  template <typename Runtime>
  using Run = If<expr, Block<Loop, While<expr, Loop>>>::template Run<Runtime>;
};

} // namespace exec

namespace runtime {

template <typename S, typename I, typename O> struct Runtime {
  using State = S;
  using Stdin = I;
  using Stdout = O;

  template <typename SS> using WithState = Runtime<SS, I, O>;
  template <typename II> using WithStdin = Runtime<S, II, O>;
  template <typename OO> using WithStdout = Runtime<S, I, OO>;

  template <typename... Instructions>
  using Run = exec::Block<Instructions...>::template Run<Runtime<S, I, O>>;
};

using Start =
    Runtime<type::Pack<>, string::ToString<__STDIN__>, string::String<>>;

template <typename... Instructions>
using Run = Start::template Run<Instructions...>;

template <typename... Instructions>
static constexpr auto start =
    string::as_literal<typename Run<Instructions...>::Stdout>;

namespace _impl_ {

template <typename> struct Debug;

} // namespace _impl_

template <typename... Instructions>
static constexpr auto debug = _impl_::Debug<Run<Instructions...>>{};

} // namespace runtime

} // namespace detail

} // namespace gil

#endif // GIL_IMPL_HPP_
