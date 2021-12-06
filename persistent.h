/**
* @copyright (c) 2015-2021 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    persistent lib
*
* @brief     C++ struct persistence
*
* @license   MIT license. See accompanying file LICENSE.
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <tuple>

// --------------------------------------------------------------------------
//
// Project includes
//
#include "named_property.h"


namespace persistent {

  template<typename T>
  using prop = named_property<T, const char*>;

  // --------------------------------------------------------------------------
  //
  // item declarations
  //
  using text = prop<std::string>;
  using boolean = prop<bool>;
  using int8 = prop<int8_t>;
  using int16 = prop<int16_t>;
  using int32 = prop<int32_t>;
  using int64 = prop<int64_t>;
  using uint8 = prop<uint8_t>;
  using uint16 = prop<uint16_t>;
  using uint32 = prop<uint32_t>;
  using uint64 = prop<uint64_t>;
  using byte = prop<uint8_t>;
  using word = prop<uint16_t>;
  using dword = prop<uint32_t>;
  using qword = prop<uint64_t>;
  using integer = prop<int>;
  using uinteger = prop<unsigned>;
  using float32 = prop<float>;
  using float64 = prop<double>;

  // --------------------------------------------------------------------------
  //
  // List of persistent values with same name
  //
  template<typename T>
  using list = prop<std::vector<T>>;

  // --------------------------------------------------------------------------
  //
  // List of fix count of persistent values with same name
  //
  template<typename T, std::size_t S>
  using fix_list = prop<std::array<T, S>>;

  // --------------------------------------------------------------------------
  //
  // structure to  hold all persistent members of a struct.
  //
  template<typename... Types>
  using member_variables_t = std::tuple<prop<typename Types::value_type>& ...>;

  // --------------------------------------------------------------------------
  //
  // Trivial base class for easier template specialization
  //
  struct basic_container {};

  template<typename T>
  auto get_members (T& t) -> typename T::member_variables& {
    return t.get_members();
  }

  template<typename T>
  auto get_members (const T& t) -> const typename T::member_variables& {
    return t.get_members();
  }

} // namespace persistent

// --------------------------------------------------------------------------
