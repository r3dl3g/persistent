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
  // template class property_t
  //
  template<typename T, const char* N>
  class property_t : public property<T> {
  public:
    typedef property<T> super;
    typedef T value_type;

    inline property_t (const value_type& value = {})
      : super(value)
    {}

    inline property_t (const property_t&) = default;
    inline property_t (property_t&&) = default;

    inline const char* name () const {
      return N;
    }

    /// create a copy with same name and value
    inline property_t* clone () const {
      return new property_t(*this);
    }

    /// assignment
    inline property_t& operator= (const property_t& rhs) {
      super::operator=(rhs);
      return *this;
    }

  };

} // namespace persistent

// --------------------------------------------------------------------------
