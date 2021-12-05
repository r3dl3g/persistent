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
* @brief     property with name.
*
* @file
*/

#pragma once

#include "property.h"

namespace persistent {

  // --------------------------------------------------------------------------
  //
  // template class named_property
  //
  template<typename T, typename N = const char*>
  class named_property : public property<T> {
  public:
    typedef property<T> super;
    typedef typename super::value_type value_type;
    typedef N name_type;

    using text = named_property<std::string, name_type>;
    using boolean = named_property<bool, name_type>;
    using int8 = named_property<int8_t, name_type>;
    using int16 = named_property<int16_t, name_type>;
    using int32 = named_property<int32_t, name_type>;
    using int64 = named_property<int64_t, name_type>;
    using uint8 = named_property<uint8_t, name_type>;
    using uint16 = named_property<uint16_t, name_type>;
    using uint32 = named_property<uint32_t, name_type>;
    using uint64 = named_property<uint64_t, name_type>;
    using byte = named_property<uint8_t, name_type>;
    using word = named_property<uint16_t, name_type>;
    using dword = named_property<uint32_t, name_type>;
    using qword = named_property<uint64_t, name_type>;
    using integer = named_property<int, name_type>;
    using uinteger = named_property<unsigned, name_type>;
    using float32 = named_property<float, name_type>;
    using float64 = named_property<double, name_type>;

    using list = named_property<std::vector<T>, name_type>;

    template<std::size_t S>
    using fix_list = named_property<std::array<T, S>, name_type>;

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
