//usr/bin/env g++ -Based -std=c++23 -O2 -o - "$0" -DLANGUAGE="$1" "${@:2}"; exit

#include "gil/std.hpp"
using namespace gil::std;

enum {
  NAME,
  INDEX,
  IS_WHITESPACE,
  CH,
};

enum Language {
  English,
  Français,
  中文,
};

template <Language language = English>
static constexpr auto greet(auto name) noexcept {
  if constexpr (language == English) {
      return block_(
        str::puts(str::literal("Hello, ")),
        str::puts(name),
        str::puts(str::literal("!\n"))
      );
  } else if constexpr (language == Français) {
    return block_(
      str::puts(str::literal("Bonjour, ")),
      str::puts(name),
      str::puts(str::literal(" !\n"))
    );
  } else if constexpr (language == 中文) {
    return block_(
      str::puts(name),
      str::puts(str::literal("好。\n"))
    );
  }
}

volatile auto run = main<{
  var_(IS_WHITESPACE) = fn_(CH)(
    break_(var_(CH) == ' ' || var_(CH) == '\t' || var_(CH) == '\n')
  ),
  while_((*var_(IS_WHITESPACE))(var_(NAME)[0] = getc_()))(),
  var_(INDEX) = 1,
  while_((var_(NAME)[*var_(INDEX)] = getc_()) != none_)(
    ++var_(INDEX)
  ),
  for_(--var_(INDEX),
       var_(INDEX) >= 0 && (*var_(IS_WHITESPACE))(var_(NAME)[*var_(INDEX)]),
       --var_(INDEX))(
    var_(NAME)[*var_(INDEX)] = none_
  ),
  greet<LANGUAGE>(var_(NAME))
}>;
