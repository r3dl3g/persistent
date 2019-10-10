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

namespace persistent {

  /**
  * template class property.
  *
  * Acts as a placeholder to have the setter/getter pattern for a value.
  */
  template<typename T>
  class property {
  public:
    typedef T value_type;

    using string = property<std::string>;
    using boolean = property<bool>;
    using int8 = property<int8_t>;
    using byte = property<uint8_t>;
    using int16 = property<int16_t>;
    using word = property<uint16_t>;
    using int32 = property<int32_t>;
    using dword = property<uint32_t>;
    using int64 = property<int64_t>;
    using qword = property<uint64_t>;
    using integer = property<int>;
    using uinteger = property<unsigned>;
    using float32 = property<float>;
    using float64 = property<double>;

    /**
    * Default constructor with Optional default value.
    */
    inline property (const T& value = T{})
      : m_value(value)
    {}

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

    /// setter
    inline void operator() (const value_type& v) {
      m_value = v;
    }

    /// write accessor
    inline value_type& operator() () {
      return m_value;
    }

    /// assignment
    inline property& operator= (const property& rhs) {
      m_value = rhs.m_value;
      return *this;
    }

    /// create a copy with same value
    inline property* clone () const {
      return new property(*this);
    }

    // accessor, not protected but hide the simple write accessors.
    struct accessor {
      static inline value_type& get (const property<T>& t) {
        return t.m_value;
      }

      static inline void set (const property<T>& t, const value_type& v) {
        t.m_value = v;
      }

      inline value_type& get () {
        return t.m_value;
      }

      inline void set (const value_type& v) {
        t.m_value = v;
      }

    private:
      friend property;
      inline accessor (const property& t)
        : t(t)
      {}

      const property& t;
    };

    accessor access () const {
      return accessor(*this);
    }

  private:
    mutable value_type m_value;

  };

} // namespace persistent

// --------------------------------------------------------------------------
