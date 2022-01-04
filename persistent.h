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
#include <functional>

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

  template <typename T, typename S = void>
  struct is_persistent : std::false_type {};

  template <typename T>
  struct is_persistent<T, typename std::enable_if<std::is_base_of<persistent_struct, T>::value>::type> : std::true_type {};

  // --------------------------------------------------------------------------
  template<typename T>
  auto attributes (T& t) {
    return t.attributes();
  }

  template<typename T>
  auto attributes (const T& t) {
    return attributes((const_cast<T&>(t)));
  }

  // --------------------------------------------------------------------------
  template<typename... Ts>
  inline auto make_attributes (Ts... a) {
    return std::make_tuple(a...);
  }

  // --------------------------------------------------------------------------
  //
  // Wrapper for a named attribute of a persistent struct.
  //
  template<typename T>
  struct attribute {
    typedef T type;

    attribute (T& v, char const* n)
      : value(v)
      , name(n)
    {}

    T& value;           /// The attribute to read/write
    char const* name;   /// The name of the attribute
  };

  template<typename T>
  char const* get_property_name (const attribute<T>& a) {
    return a.name;
  }

  template<typename T>
  T& set_property_value (attribute<T>& a) {
    return a.value;
  }

  template<typename T>
  const T& get_property_value (const attribute<T>& a) {
    return a.value;
  }

  namespace detail {

    template<class C, typename T>
    std::function<void(T)> bind (C& c, void(C::*method)(T)) {
      C* cp = &c;
      return [=](T t) {
        return (cp->*method)(t);
      };
    }

  } // namespace detail


  // --------------------------------------------------------------------------
  //
  // Wrapper for a named attribute of a persistent struct.
  //
  template<typename T>
  struct getter {
    typedef T type;

    getter (T value, char const* n)
      : value(value)
      , name(n)
    {}

    T value;    /// The attribute to read
    char const* name; /// The name of the attribute
  };

  template<typename T>
  struct getter<const T&> {
    typedef T type;

    getter (const T& value, char const* n)
      : value(value)
      , name(n)
    {}

    const T& value;    /// The attribute to read
    char const* name; /// The name of the attribute
  };

  template<typename T>
  inline char const* get_property_name (const getter<T>& g) {
    return g.name;
  }

  template<typename T>
  inline const T get_property_value (const getter<T>& g) {
    return g.value;
  }

  template<typename T>
  inline const T& get_property_value (const getter<const T&>& g) {
    return g.value;
  }

  // --------------------------------------------------------------------------
  //
  // Wrapper for a named attribute of a persistent struct.
  //
  template<typename T>
  struct setter {
    typedef T type;

    setter (T& value, char const* n)
      : value(value)
      , name(n)
    {}

    T& value;         /// The attribute to write
    char const* name; /// The name of the attribute
  };

  template<typename T>
  char const* get_property_name (const setter<T>& s) {
    return s.name;
  }

  template<typename T>
  T& set_property_value (setter<T>& s) {
    return s.value;
  }

  // --------------------------------------------------------------------------

} // namespace persistent

// --------------------------------------------------------------------------
