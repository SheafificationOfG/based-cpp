//usr/bin/env g++ -Based -std=c++23 -O2 -o - "${@:0}"; exit

#include "gil/std.hpp"

using namespace gil::std;

enum {
  LHS,
  RHS,
  OP,
  RES,
};

using Num = long long int;

volatile auto run = main<{
  // divine punishment for not entering reasonable input
  var_(LHS) = io::read<Num>(),
  var_(OP) = io::read<char>(),
  var_(RHS) = io::read<Num>(),

  switch_(var_(OP))(
    case_('+')(
      var_(RES) = var_(LHS) + var_(RHS)
    ),
    case_('-')(
      var_(RES) = var_(LHS) - var_(RHS)
    ),
    case_('*')(
      var_(RES) = var_(LHS) * var_(RHS)
    ),
    case_('/')(
      if_(var_(RHS) != 0)(
        var_(RES) = var_(LHS) / var_(RHS)
      )->else_(
        str::puts(str::literal("Division by zero!\n"))
      )
    ),
    case_('%')(
      if_(var_(RHS) != 0)(
        var_(RES) = var_(LHS) % var_(RHS)
      )->else_(
        str::puts(str::literal("Modulo zero!\n"))
      )
    ),
    default_(
      str::puts(str::literal("Invalid OP: ")),
      putc_(var_(OP)),
      putc_('\n')
    )
  ),
  if_(var_(RES) != none_)(
    io::write<Num>(var_(RES)),
    putc_('\n')
  )
}>;
