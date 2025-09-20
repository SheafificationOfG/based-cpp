//usr/bin/env g++ -Based -std=c++23 -O2 -o - "${@:0}"; exit

#include "gil/std.hpp"
using namespace gil::std;

using Num = long long int;

static constexpr auto read_array(auto array, auto len) noexcept {
  return block_(
    array[0] = io::read<Num>(),
    if_(array[0] == none_)(
      len = 0
    )->else_(
      for_(len = 1, io::read<char>() == ',', ++len)(
        array[*len] = io::read<Num>()
      )
    )
  );
}

static constexpr auto write_array(auto array, auto len) noexcept {
  constexpr struct : local {} stack;
  constexpr auto i = var_(stack)[0];
  return block_(
    putc_('['),
    for_(i = 0, i < len, ++i)(
      if_(i > 0)(
        str::puts(str::literal(", "))
      ),
      io::write<Num>(array[*i])
    ),
    putc_(']'),
    putc_('\n')
  );
}

enum {
  MERGESORT,
  LO,
  HI,
  MID,
  TMP,
  I,
  J,
  K,
  ARRAY,
  LEN,
};

volatile auto run = main<{
  global_(MERGESORT) = fn_(ARRAY, LO, HI)(
    if_(var_(LO) + 1 < var_(HI))(
      var_(MID) = (var_(LO) + var_(HI)) / 2,
      (*global_(MERGESORT))(*var_(ARRAY), *var_(LO), *var_(MID)),
      (*global_(MERGESORT))(*var_(ARRAY), *var_(MID), *var_(HI)),
      var_(I) = *var_(LO),
      var_(J) = *var_(MID),
      for_(var_(K) = 0, var_(I) < var_(MID) && var_(J) < var_(HI), ++var_(K))(
        if_((**var_(ARRAY))[*var_(I)] < (**var_(ARRAY))[*var_(J)])(
          var_(TMP)[*var_(K)] = *(**var_(ARRAY))[var_(I)++]
        )->else_(
          var_(TMP)[*var_(K)] = *(**var_(ARRAY))[var_(J)++]
        )
      ),
      while_(var_(I) < var_(MID))(
        var_(TMP)[var_(K)++] = *(**var_(ARRAY))[var_(I)++]
      ),
      while_(var_(J) < var_(HI))(
        var_(TMP)[var_(K)++] = *(**var_(ARRAY))[var_(J)++]
      ),
      for_(block_(var_(K) = 0, var_(I) = *var_(LO)),
           var_(I) < var_(HI),
           block_(++var_(K), ++var_(I)))(
        (**var_(ARRAY))[*var_(I)] = *var_(TMP)[*var_(K)]
      )
    )
  ),

  read_array(var_(ARRAY), var_(LEN)),
  (*global_(MERGESORT))(&var_(ARRAY), 0, *var_(LEN)),
  write_array(var_(ARRAY), var_(LEN))
}>;
