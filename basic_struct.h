/**
* @copyright (c) 2015-2018 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    standard lib
*
* Customer   -
*
* @brief     Container for persistent.
*
* @file
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <tuple>

// --------------------------------------------------------------------------
//
// Library includes
//

// --------------------------------------------------------------------------
//
// Project includes
//
#include "named_property.h"


namespace persistent {

  template<typename T>
  using type = named_property<T, const char*>;

  // --------------------------------------------------------------------------
  //
  // item declarations
  //
  using string = type<std::string>;
  using boolean = type<bool>;
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
  using vector = typename type<T>::vector;

  // --------------------------------------------------------------------------
  //
  // List of fix count of persistent values with same name
  //
  template<typename T, std::size_t S>
  using array = typename type<T>::array<S>;

  // --------------------------------------------------------------------------
  //
  // structure to  hold all persistent members of a struct.
  //
  template<typename... Types>
  using member_variables_t = std::tuple<type<typename Types::value_type>& ...>;

  // --------------------------------------------------------------------------
  //
  // Trivial base class for easier template specialization
  //
  struct basic_container {};

  // --------------------------------------------------------------------------
  //
  // class basic_struct.
  // Base class for a persistent struct.
  //
  template<typename Type, typename ... Types>
  struct basic_struct : public basic_container {

    // A list with references to all member properties of this struct
    typedef member_variables_t<Type, Types...> member_variables;

    // a persistent basic_struct needs at least one member
    basic_struct (type<typename Type::value_type>& member, type<typename Types::value_type>&... members)
      : members(member, members...)
    {}

    basic_struct () = delete;
    basic_struct (const basic_struct&) = default;
    basic_struct (basic_struct&&) = default;

    const basic_struct& operator= (const basic_struct& rhs) {
      if (this != &rhs) {
        members = rhs.members;
      }
      return *this;
    }

    bool operator== (const basic_struct& rhs) const {
      return (this == &rhs) || (members == rhs.members);
    }

    bool operator!= (const basic_struct& rhs) const {
      return !operator==(rhs);
    }

    member_variables& get_members () {
      return members;
    }

    const member_variables& get_members () const {
      return members;
    }

  protected:
    // hold references to all members
    member_variables members;
  };

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
