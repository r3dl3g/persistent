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
#include <string>
#include <vector>
#include <map>
#include <iostream>

// --------------------------------------------------------------------------
//
// Project includes
//0
#include "ostreamfmt.h"
#include "variadic_util.h"
#include "named_property.h"
#include "string_util.h"


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
  using byte = type<uint8_t>;
  using int16 = type<int16_t>;
  using word = type<uint16_t>;
  using int32 = type<int32_t>;
  using dword = type<uint32_t>;
  using int64 = type<int64_t>;
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
  namespace detail {

    // --------------------------------------------------------------------------
    //
    // copy operation for N th element of a tuple
    //
    template<std::size_t N, typename ... Properties>
    struct copy {
      typedef std::tuple<Properties& ...> property_list;

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
      typedef std::tuple<Properties& ...> property_list;
      static inline void properties (const property_list& /*from*/, const property_list& /*to*/) {}
    };

    // --------------------------------------------------------------------------
    //
    // helper to recoursive copy the members of a tuple
    //
    template<typename ... Properties>
    void copy_properties (const std::tuple<Properties& ...>&from, const std::tuple<Properties& ...>&to) {
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
    typedef std::tuple<Properties& ...> property_list;

    basic_struct (Properties&... members)
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

  namespace io {
    /**
    * default i/o mapper.
    * Allows specialized implementations.
    */
    template<typename P, std::size_t I, typename T>
    struct out {
      /// write a named value.
      //static void write (P&, const T&);
    };

    template<typename P, std::size_t I, typename T>
    struct in {
      /// read a named value.
      //static void read (P&, T&);
    };

    // --------------------------------------------------------------------------
    //
    // write 1st value to ostream
    //
    template<typename T>
    struct out<std::ostream, 0, T> {
      static inline void write (std::ostream& os, const T& t) {
        os << t;
      }
    };

    // --------------------------------------------------------------------------
    //
    // read 1st value from istream
    //
    template<typename T>
    struct in<std::istream, 0, T> {
      static inline void read (std::istream& is, T& t) {
        is >> t;
      }
    };

    // --------------------------------------------------------------------------
    //
    // write following values to ostream
    //
    template<std::size_t I, typename T>
    struct out<std::ostream, I, T> {
      static void write (std::ostream& os, const T& t) {
        os << ',';
        out<std::ostream, 0, T>::write(os, t);
      }
    };

    // --------------------------------------------------------------------------
    //
    // read following values from istream
    //
    template<std::size_t I, typename T>
    struct in<std::istream, I, T> {
      static void read (std::istream& is, T& t) {
        char delim = 0;
        is >> std::ws >> delim;
        if (delim != ',') {
          throw std::runtime_error(ostreamfmt("Expected list delemiter ',' but got '" << delim << "'!"));
        }
        is >> std::ws >> t;
      }
    };

    // --------------------------------------------------------------------------
    //
    // write string to ostream
    //
    template<>
    struct out<std::ostream, 0, const std::string> {
      static inline void write (std::ostream& os, const std::string& t) {
        os << util::string::quoted(t);
      }
    };

    template<>
    struct out<std::ostream, 0, const char*> {
      static inline void write (std::ostream& os, const char* t) {
        os << util::string::quoted(t);
      }
    };

    // --------------------------------------------------------------------------
    //
    // read string from istream
    //
    template<>
    struct in<std::istream, 0, std::string> {
      static void read (std::istream& is, std::string& t) {
        is >> std::ws >> util::string::quoted(t);
      }
    };

    // --------------------------------------------------------------------------
    //
    // write vector values to ostream
    //
    template<typename T>
    struct out<std::ostream, 0, const std::vector<T>> {
      static void write (std::ostream& os, const std::vector<T>& t) {
        os << '[';
        const char* delim = "";
        for (const T& v : t) {
          os << delim;
          delim = ",";
          out<std::ostream, 0, const T>::write(os, v);
        }
        os << ']';
      }
    };

    // --------------------------------------------------------------------------
    //
    // read vector from istream
    //
    template<typename T>
    struct in<std::istream, 0, std::vector<T>> {
      static void read (std::istream& is, std::vector<T>& v) {
        char delim;
        is >> std::ws >> delim;
        if (delim != '[') {
          throw std::runtime_error(ostreamfmt("Expected array open bracket '[' but got '" << delim << "'!"));
        }
        while (delim != ']') {
          is >> std::ws;
          T t;
          in<std::istream, 0, T>::read(is, t);
          v.emplace_back(t);
          is >> std::ws >> delim;
          if ((delim != ',') && (delim != ']')) {
            throw std::runtime_error(ostreamfmt("Expected coma ',' or array close bracket ']' but got '" << delim << "'!"));
          }
        }
      }
    };

    // --------------------------------------------------------------------------
    //
    // write array values to ostream
    //
    template<typename T, size_t S>
    struct out<std::ostream, 0, const std::array<T, S>> {
      static void write (std::ostream& os, const std::array<T, S>& a) {
        os << '[';
        const char* delim = "";
        for (const T& e : a) {
          os << delim;
          delim = ",";
          out<std::ostream, 0, const T>::write(os, e);
        }
        os << ']';
      }
    };

    // --------------------------------------------------------------------------
    //
    // read array from istream
    //
    template<typename T, size_t S>
    struct in<std::istream, 0, std::array<T, S>> {
      static void read (std::istream& is, std::array<T, S>& a) {
        char expected = '[';
        char delim = 0;
        for (T& e : a) {
          is >> std::ws >> delim;
          if (delim != expected) {
            throw std::runtime_error(ostreamfmt("Expected array open bracket '[' but got '" << delim << "'!"));
          }
          expected = ',';
          in<std::istream, 0, T>::read(is >> std::ws, e);
        }
        is >> std::ws >> delim;
        if (delim != ']') {
          throw std::runtime_error(ostreamfmt("Expected array close bracket ']' but got '" << delim << "'!"));
        }
      }
    };

    // --------------------------------------------------------------------------
    //
    // write named type to ostream
    //
    template<typename T>
    struct out<std::ostream, 0, type<T>> {
      static inline void write (std::ostream& os, const type<T>& t) {
        os << t.name() << ':';
        out<std::ostream, 0, const T>::write(os, t());
      }
    };

    // --------------------------------------------------------------------------
    //
    // read named type from istream
    //
    template<typename T>
    struct in<std::istream, 0, type<T>> {
      static void read (std::istream& is, const type<T>& t) {
        std::string name;
        is >> std::ws >> util::string::name(name);
        if (name != t.name()) {
          throw std::runtime_error(ostreamfmt("Expected name property '" << t.name()
                                              << "' but got '" << name << "'!"));
        }
        std::getline(is >> std::ws, name, ':');
        in<std::istream, 0, T>::read(is, t.access().get());
      }
    };

    // --------------------------------------------------------------------------
    template<typename P, std::size_t I, typename... Properties>
    struct tuple_io {
      static void write (P& p, std::tuple<Properties&...> const& t) {
        tuple_io<P, I - 1, Properties...>::write(p, t);
        using tuple_type = typename util::variadic_element<I - 1, Properties...>::type;
        const auto& m = std::get<I - 1>(t);
        out<P, I - 1, tuple_type>::write(p, m);
      }

      static void read (P& p, std::tuple<Properties& ...>& t) {
        tuple_io<P, I - 1, Properties...>::read(p, t);
        using tuple_type = typename util::variadic_element<I - 1, Properties...>::type;
        auto& m = std::get<I - 1>(t);
        in<P, I - 1, tuple_type>::read(p, m);
      }
    };

    template<typename P, typename... Properties>
    struct tuple_io<P, 0, Properties...> {
      static void write (P&, const std::tuple<Properties&...>&) {}
      static void read (P&, std::tuple<Properties&...>&) {}
    };

    template<typename P, typename... Properties>
    void write_tuple (P& p, const std::tuple<Properties&...>& t) {
      tuple_io<P, sizeof...(Properties), Properties...>::write(p, t);
    }

    template<typename P, typename... Properties>
    void read_tuple (P& p, std::tuple<Properties&...>& t) {
      tuple_io<P, sizeof...(Properties), Properties...>::read(p, t);
    }

    // --------------------------------------------------------------------------
    //
    // write basic_struct to ostream
    //
    template<typename ... Properties>
    struct out<std::ostream, 0, basic_struct<Properties...>> {
      static void write (std::ostream& os, const basic_struct<Properties...>& t) {
        os << '{';
        write_tuple(os, t.properites());
        os << '}';
      }
    };

    // --------------------------------------------------------------------------
    //
    // read basic_struct from istream
    //
    template<typename ... Properties>
    struct in<std::istream, 0, basic_struct<Properties...>> {
      static void read (std::istream& is, basic_struct<Properties...>& t) {
        char delim;
        is >> delim;
        if (delim != '{') {
          throw std::runtime_error(ostreamfmt("Expected struct open brace '{' but got '" << delim << "'!"));
        }
        read_tuple(is, t.properites());
        is >> delim;
        if (delim != '}') {
          throw std::runtime_error(ostreamfmt("Expected struct close bracket '}' but got '" << delim << "'!"));
        }
      }
    };

    // --------------------------------------------------------------------------
    //
    // write basic_struct
    //
    template<typename P, typename ... Properties>
    struct out<P, 0, basic_struct<Properties...>> {
      static inline void write (P& p, const basic_struct<Properties...>& t) {
        write_tuple(p, t.properites());
      }
    };

    // --------------------------------------------------------------------------
    //
    // read basic_struct
    //
    template<typename P, typename ... Properties>
    struct in<P, 0, basic_struct<Properties...>> {
      static inline void read (P& p, basic_struct<Properties...>& t) {
        read_tuple(p, t.properites());
      }
    };

  }

} // namespace persistent

namespace std {

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  inline std::ostream& operator << (std::ostream& os, 
                                    const persistent::basic_struct<Properties...>& t) {
    persistent::io::out<std::ostream, 0, persistent::basic_struct<Properties...>>::write(os, t);
    return os;
  }

  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const persistent::type<T>& t) {
    persistent::io::out<std::ostream, 0, persistent::type<T>>::write(os, t);
    return os;
  }

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  inline std::istream& operator >> (std::istream& is, persistent::basic_struct<Properties...>& t) {
    persistent::io::in<std::istream, 0, persistent::basic_struct<Properties...>>::read(is, t);
    return is;
  }

  template<typename T>
  inline std::istream& operator >> (std::istream& is, const persistent::type<T>& t) {
    persistent::io::in<std::istream, 0, persistent::type<T>>::read(is, t);
    return is;
  }

}

// --------------------------------------------------------------------------
