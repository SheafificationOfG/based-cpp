/** ********
 * GNU Interface Layer
 *
 * This header file exposes the instruction set architecture
 * for the C++ abstract machine.
 */

#ifndef GIL_HPP_
#define GIL_HPP_

#include "gil.impl.hpp"

namespace gil {

using detail::expr::expr;
using detail::expr::len;
using detail::expr::none;
using detail::expr::peek;
using detail::expr::str;
using detail::expr::tuple;
using detail::expr::val;
using detail::expr::var;
using detail::expr::vec;

using detail::exec::Advance;
using detail::exec::Put;
using detail::exec::Set;

using detail::exec::Block;
using detail::exec::If;
using detail::exec::While;

using detail::runtime::debug;
using detail::runtime::start;

} // namespace gil

#endif // GIL_HPP_
