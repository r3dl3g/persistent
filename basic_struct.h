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
// Project includes
//
#include "persistent.h"


namespace persistent {

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
    basic_struct (prop<typename Type::value_type>& member, prop<typename Types::value_type>&... members)
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

} // namespace persistent

// --------------------------------------------------------------------------
