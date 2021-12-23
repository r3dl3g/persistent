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
#include "property.h"

namespace persistent {

  // --------------------------------------------------------------------------
  //
  // template class named_property
  //
  template<typename T, typename N = char const*>
  class named_property : public property<T> {
  public:
    typedef property<T> super;
    typedef typename super::value_type value_type;
    typedef N name_type;

    /**
    * Constructor with name assignment.
    * Optional a default value can be given.
    */
    inline named_property (const name_type& name, const value_type& value = T{})
      : super(value)
      , m_name(name)
    {}

    /**
    * A name must be given, so no default constructor is allowed.
    */
    named_property () = delete;

    inline named_property (const named_property&) = default;
    inline named_property (named_property&&) = default;

    inline const name_type& name () const {
      return m_name;
    }

    /// create a copy with same name and value
    inline named_property* clone () const {
      return new named_property(*this);
    }

    /// assignment
    inline named_property& operator= (const named_property& rhs) {
      super::operator=(rhs);
      return *this;
    }

  private:
    const name_type m_name;
  };

} // namespace persistent

// --------------------------------------------------------------------------
