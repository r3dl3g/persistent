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
#include "property_t.h"


namespace persistent {

  namespace prop {

    // --------------------------------------------------------------------------
    template<typename T>
    using type = named_property<T, const char*>;

    // --------------------------------------------------------------------------
    //
    // item declarations
    //
    using text = type<std::string>;
    using boolean = type<bool>;
    using character = type<char>;
    using int8 = type<int8_t>;
    using int16 = type<int16_t>;
    using int32 = type<int32_t>;
    using int64 = type<int64_t>;
    using uint8 = type<uint8_t>;
    using uint16 = type<uint16_t>;
    using uint32 = type<uint32_t>;
    using uint64 = type<uint64_t>;
    using byte = type<uint8_t>;
    using word = type<uint16_t>;
    using dword = type<uint32_t>;
    using qword = type<uint64_t>;
    using integer = type<int>;
    using uinteger = type<unsigned>;
    using float32 = type<float>;
    using float64 = type<double>;

    // --------------------------------------------------------------------------
    //
    // List of persistent values with same name
    //
    template<typename T>
    using list = type<std::vector<T>>;

    // --------------------------------------------------------------------------
    //
    // List of fix count of persistent values with same name
    //
    template<typename T, std::size_t S>
    using fix_list = type<std::array<T, S>>;

    // --------------------------------------------------------------------------
    //
    // shared persistent item
    //
    template<typename T>
    using shared = type<std::shared_ptr<T>>;
  }

  namespace prop_t {

    // --------------------------------------------------------------------------
    template<typename T, const char* N>
    using type = property_t<T, N>;

    template<const char* N> using text = type<std::string, N>;
    template<const char* N> using boolean = type<bool, N>;
    template<const char* N> using character = type<char, N>;
    template<const char* N> using int8 = type<int8_t, N>;
    template<const char* N> using int16 = type<int16_t, N>;
    template<const char* N> using int32 = type<int32_t, N>;
    template<const char* N> using int64 = type<int64_t, N>;
    template<const char* N> using uint8 = type<uint8_t, N>;
    template<const char* N> using uint16 = type<uint16_t, N>;
    template<const char* N> using uint32 = type<uint32_t, N>;
    template<const char* N> using uint64 = type<uint64_t, N>;
    template<const char* N> using byte = type<uint8_t, N>;
    template<const char* N> using word = type<uint16_t, N>;
    template<const char* N> using dword = type<uint32_t, N>;
    template<const char* N> using qword = type<uint64_t, N>;
    template<const char* N> using integer = type<int, N>;
    template<const char* N> using uinteger = type<unsigned, N>;
    template<const char* N> using float32 = type<float, N>;
    template<const char* N> using float64 = type<double, N>;

    template<typename T, const char* N> using list = type<std::vector<T>, N>;

    template<typename T, std::size_t S, const char* N>
    using fix_list = type<std::array<T, S>, N>;

    template<typename T, const char* N> using shared = type<std::shared_ptr<T>, N>;

  }

  // --------------------------------------------------------------------------
  //
  // structure to hold all persistent members of a struct.
  //
  template<typename... Types>
  using member_variables_t = std::tuple<Types& ...>;

  // --------------------------------------------------------------------------
  //
  // Trivial base class for easier template specialization
  //
  struct basic_container {};

  template<typename T>
  auto get_members (T& t) -> decltype(t.get_members()) {
    return t.get_members();
  }

  template<typename T>
  auto get_members (const T& t) -> decltype(t.get_members()) {
    return t.get_members();
  }

} // namespace persistent

// --------------------------------------------------------------------------
