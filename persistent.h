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
#include <string>

// --------------------------------------------------------------------------
//
// Project includes
//


namespace persistent {

  // --------------------------------------------------------------------------
  //
  // Trivial base class for easier template specialization
  //
  struct persistent_struct {};

  template<typename T>
  auto attributes (T& t) -> decltype(t.attributes()) {
    return t.attributes();
  }

  template<typename T>
  auto attributes (const T& t) -> decltype(t.attributes()) {
    return t.attributes();
  }

  // --------------------------------------------------------------------------
  //
  // Wrapper for a named attribute of a persistent struct.
  //
  template<typename T>
  struct attribute {
    attribute (T& v, char const* n)
      : value(v)
      , name(n)
    {}

    T& value;           /// The attribute to read/write
    char const* name;   /// The name of the attribute
  };

  template<typename T>
  char const* get_property_name (const attribute<T>& t) {
    return t.name;
  }

  template<typename T>
  T& get_property_value (attribute<T>& t) {
    return t.value;
  }

  template<typename T>
  const T& get_property_value (const attribute<T>& t) {
    return t.value;
  }

  template<typename... Ts>
  inline auto make_attributes (attribute<Ts>... a) {
    return std::make_tuple(a...);
  }


  // --------------------------------------------------------------------------

} // namespace persistent

// --------------------------------------------------------------------------
