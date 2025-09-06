//usr/bin/env g++ -Based -std=c++23 -o - "$0" -DLANGUAGE="$1" "${@:2}"; exit

#include "gil/gil.hpp"
using namespace gil;

enum Language {
  English,
  Français,
  中文,
};

enum {
  name,
  index,
};

template <auto string>
using Puts = Block<
    Set<index, val<0>>,
    While<(var<index> < len<string>),
          Block<Put<string[var<index>]>, Set<index, var<index> + val<1>>>>>;

template <Language = English> struct SayGreeting;

template <> struct SayGreeting<English> {
  template <auto name> using To = Puts<str<"Hello, "> + name + str<"!">>;
};

template <> struct SayGreeting<Français> {
  template <auto name> using To = Puts<str<"Bonjour, "> + name + str<" !">>;
};

template <> struct SayGreeting<中文> {
  template <auto name> using To = Puts<name + str<"好。">>;
};

using SkipWhitespace =
    While<peek<> == val<' '> || peek<> == val<'\n'> || peek<> == val<'\t'>,
          Advance<>>;

template <auto dst>
using Reads = Block<Set<dst, str<"">>,
                    While<peek<> != val<'\n'> && peek<> != none,
                          Block<Set<dst, var<dst> + peek<>>, Advance<>>>>;

auto run = start<SkipWhitespace, Reads<name>,
                 SayGreeting<LANGUAGE>::template To<var<name>>, Put<val<'\n'>>>;
