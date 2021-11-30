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
  using vector = type<std::vector<T>>;

  // --------------------------------------------------------------------------
  //
  // List of fix count of persistent values with same name
  //
  template<typename T, std::size_t S>
  using array = type<std::array<T, S>>;

  // --------------------------------------------------------------------------
  namespace detail {

    // --------------------------------------------------------------------------
    //
    // copy operation for N th element of a tuple
    //
    template<std::size_t N, typename ... Properties>
    struct copy {
      typedef std::tuple<type<Properties>& ...> property_list;

      static void properties (const property_list& from, const property_list& to) {
        copy<N - 1, Properties...>::properties(from, to);
        const auto& f = std::get<N - 1>(from);
        const auto& t = std::get<N - 1>(to);
        t.access().set(f());
      }
    };

    // --------------------------------------------------------------------------
    //
    // Stop recoursion at element 0
    //
    template<typename ... Properties>
    struct copy<0, Properties...> {
      typedef std::tuple<type<Properties>& ...> property_list;
      static inline void properties (const property_list& /*from*/, const property_list& /*to*/) {}
    };

    // --------------------------------------------------------------------------
    //
    // helper to recoursive copy the members of a tuple
    //
    template<typename ... Properties>
    void copy_properties (const std::tuple<type<Properties>& ...>&from, const std::tuple<type<Properties>& ...>&to) {
      copy<sizeof...(Properties), Properties...>::properties(from, to);
    }

  } // namespace detail

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
  template<typename ... Properties>
  struct basic_struct : public basic_container {

    // A list with references to all member properties of this struct
    typedef std::tuple<type<Properties>& ...> property_list;

    basic_struct (type<Properties>&... members)
      : m_properites(members...)
    {}

    basic_struct (const basic_struct&) = delete;
    basic_struct (const basic_struct&&) = delete;

    const property_list& properites () const {
      return m_properites;
    }

    property_list& properites () {
      return m_properites;
    }

    const basic_struct& operator= (const basic_struct& rhs) {
      copy_from(rhs);
      return *this;
    }

    bool operator== (const basic_struct& rhs) const {
      return m_properites == rhs.m_properites;
    }

    bool operator!= (const basic_struct& rhs) const {
      return !operator==(rhs);
    }

  protected:
    void copy_from (const basic_struct& rhs) {
      detail::copy_properties(rhs.m_properites, m_properites);
    }

  private:
    // hold references to all members
    property_list m_properites;
  };

} // namespace persistent

// --------------------------------------------------------------------------
