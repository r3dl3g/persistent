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
* @brief     property.
*
* @file
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <string>
#include <vector>
#include <array>

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

    using text = property<std::string>;
    using boolean = property<bool>;
    using int8 = property<int8_t>;
    using int16 = property<int16_t>;
    using int32 = property<int32_t>;
    using int64 = property<int64_t>;
    using uint8 = property<uint8_t>;
    using uint16 = property<uint16_t>;
    using uint32 = property<uint32_t>;
    using uint64 = property<uint64_t>;
    using byte = property<uint8_t>;
    using word = property<uint16_t>;
    using dword = property<uint32_t>;
    using qword = property<uint64_t>;
    using integer = property<int>;
    using uinteger = property<unsigned>;
    using float32 = property<float>;
    using float64 = property<double>;

    using list = property<std::vector<T>>;

    template<std::size_t S>
    using fix_list = property<std::array<T, S>>;

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

  private:
    mutable value_type m_value;

  };

} // namespace persistent

// --------------------------------------------------------------------------
