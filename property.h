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
#include <vector>
#include <array>
#include <memory>

namespace persistent {

  // --------------------------------------------------------------------------
  //
  // Trivial base class for easier template specialization
  //
  struct basic_property {};

  /**
  * template class property.
  *
  * Acts as a placeholder to have the setter/getter pattern for a value.
  */
  template<typename T>
  class property : public basic_property {
  public:
    typedef T value_type;

    /**
    * Default constructor with optional default value.
    */
    inline property (const T& value = T{})
      : m_value(value)
    {}

    inline property* clone () const {
      return new property(m_value);
    }

    /// compare equal with same property
    inline bool operator== (const property& rhs) const {
      return (m_value == rhs.m_value);
    }

    /// compare not equal with same property
    inline bool operator!= (const property& rhs) const {
      return !operator==(rhs);
    }

    /// getter
    inline const value_type& operator() () const {
      return m_value;
    }

    inline const value_type& get () const {
      return m_value;
    }

    inline operator value_type() const {
      return m_value;
    }

    /// setter
    inline value_type& operator() () {
      return m_value;
    }

    inline void operator() (const value_type& v) {
      m_value = v;
    }

    inline void set (const value_type& v) {
      m_value = v;
    }

    inline void operator= (const value_type& v) {
      m_value = v;
    }

  private:
    mutable value_type m_value;

  };

} // namespace persistent

// --------------------------------------------------------------------------
