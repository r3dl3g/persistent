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
#include <map>
#include <iostream>

// --------------------------------------------------------------------------
//
// Library includes
//
#include <util/ostreamfmt.h>
#include <util/variadic_util.h>
#include <util/string_util.h>
#include <util/vector_util.h>

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

  namespace io {

    // --------------------------------------------------------------------------
    //
    // write
    //
    // --------------------------------------------------------------------------

    template<typename P>
    struct write_traits {

      static void list_delemiter (P& p) {
        p << ',';
      }

      static void members_delemiter (P& p) {
        p << ',';
      }

      static void list_start (P& p) {
        p << '[';
      }

      static void list_end (P& p) {
        p << ']';
      }

      static void object_key (P& p, const std::string& key) {
        p << key << ':';
      }

      static void object_start (P& p) {
        p << '{';
      }

      static void object_end (P& p) {
        p << '}';
      }
    };

    /**
    * default i/o mapper.
    * Allows specialized implementations.
    *
    * default: write value
    */
    template<typename P, typename T>
    struct write {
      /// write a named value.
      static void to (P& p, const T& t) {
        p << t;
      }
    };

    /// write property
    template<typename P, typename T>
    struct write<P, type<T>> {
      static void to (P& p, const type<T>& t) {
        write_traits<P>::object_key(p, t.name());
        write<P, const T>::to(p, t());
      }
    };

    template<typename P, typename T>
    void write_property (P& p, const type<T>& t) {
      write<P, type<T>>::to(p, t);
    }

    /// write vector and array values
    template<typename P, typename T, typename V>
    struct writeList {
      static void to (P& p, const std::string& name, const V& v) {
        write_traits<P>::object_key(p, name);
        write_traits<P>::list_start(p);
        bool first = true;
        for (const T& t : v) {
          if (first) {
            first = false;
          } else {
            write_traits<P>::list_delemiter(p);
          }
          write<P, const T>::to(p, t);
        }
        write_traits<P>::list_end(p);
      }
    };

    /// write vector values
    template<typename P, typename T>
    struct write<P, type<std::vector<T>>> {
      static void to (P& p, const type<std::vector<T>>& t) {
        writeList<P, T, std::vector<T>>::to(p, t.name(), t());
      }
    };

    /// write array values
    template<typename P, typename T, size_t S>
    struct write<P, type<std::array<T, S>>> {
      static void to (P& p, const type<std::array<T, S>>& a) {
        writeList<P, T, std::array<T, S>>::to(p, a.name(), a());
      }
    };

//    template<std::size_t I, typename P, typename... Properties>
//    static void write_member (P& p, std::tuple<type<Properties>&...> const& t) {
//      using tuple_type = typename util::variadic_element<I, Properties...>::type;
//      const auto& m = std::get<I>(t);
//      write<P, type<tuple_type>>::to(p, m);
//    }

    template<std::size_t I, typename P, typename... Properties>
    struct write_nth {
      static void to (P& p, std::tuple<type<Properties>&...> const& t) {
        write_nth<I - 1, P, Properties...>::to(p, t);
        write_traits<P>::members_delemiter(p);
        const auto& m = std::get<I>(t);
        write_property(p, m);
      }
    };

    template<typename P, typename... Properties>
    struct write_nth<0, P, Properties...> {
      static void to (P& p, std::tuple<type<Properties>&...> const& t) {
        const auto& m = std::get<0>(t);
        write_property(p, m);
      }
    };

    /// write tuple
    template<typename P, typename... Properties>
    void write_tuple (P& p, const std::tuple<type<Properties>&...>& t) {
      write_traits<P>::object_start(p);
      write_nth<(sizeof...(Properties)) - 1, P, Properties...>::to(p, t);
      write_traits<P>::object_end(p);
    }

    //// write tuple
    template<typename P, typename ... Properties>
    struct write<P, std::tuple<type<Properties>&...>> {
      static void to (P& p, const std::tuple<type<Properties>&...>& t) {
        write_tuple(p, t);
      }
    };

    /// write basic_struct
    template<typename P, typename ... Properties>
    struct write<P, basic_struct<Properties...>> {
      static void to (P& p, const basic_struct<Properties...>& t) {
        write_tuple(p, t.properites());
      }
    };

    /// write struct helper
    template<typename P, typename ... Properties>
    void write_struct (P& p, const basic_struct<Properties...>& t) {
      write_tuple(p, t.properites());
    }

    // --------------------------------------------------------------------------
    //
    // specializations for ostream
    //
    template<>
    struct write<std::ostream, const std::string> {
      static inline void to (std::ostream& os, const std::string& t) {
        os << util::string::quoted(t);
      }
    };

    template<>
    struct write<std::ostream, const char*> {
      static inline void to (std::ostream& os, const char* t) {
        os << util::string::quoted(t);
      }
    };

    template<>
    struct write<std::ostream, const unsigned char> {
      static inline void to (std::ostream& os, const unsigned char t) {
        os << +t;
      }
    };

    template<>
    struct write<std::ostream, const char> {
      static inline void to (std::ostream& os, const char t) {
        os << +t;
      }
    };

    // --------------------------------------------------------------------------
    //
    // read
    //
    // --------------------------------------------------------------------------
    template<typename P>
    struct read_traits {
      static char list_start (P&);
      static char list_delemiter (P&);
      static bool list_continue (P&, char);
      static void list_end (P&, char);
      static char object_key (P&, std::string&);
      static char object_start (P&);
      static char object_delemiter (P&);
      static bool object_continue (P&, char);
      static void object_end (P&, char);
    };

    /// read value
    template<typename P, typename T, typename Enable = void>
    struct read {
      /// read a named value.
      static void from (P& p, T& t) {}
    };

    /// read property
    template<typename P, typename T>
    struct read<P, type<T>> {
      static void from (P& p, type<T>& t) {
        std::string name;
        read_traits<P>::object_key(p, name);
        if (name != t.name()) {
          throw std::runtime_error(ostreamfmt("Expected name property '" << t.name()
                                              << "' but got '" << name << "'!"));
        }
        read<P, T>::from(p, t.access().get());
      }
    };

    template<typename P, typename T>
    inline void read_property (P& p, type<T>& t) {
      read<P, type<T>>::from(p, t);
    }

    /// read vector
    template<typename P, typename T>
    struct read<P, type<std::vector<T>>> {
      static void from (P& p, type<std::vector<T>>& v) {
        auto delim = read_traits<P>::list_start(p);
        while (read_traits<P>::list_continue(p, delim)) {
          T t;
          read<P, T>::from(p, t);
          v().push_back(t);
          delim = read_traits<P>::list_delemiter(p);
        }
        read_traits<P>::list_end(p, delim);
      }
    };

    /// read array from istream
    template<typename P, typename T, size_t S>
    struct read<P, type<std::array<T, S>>> {
      static void from (P& p, type<std::array<T, S>>& a) {
        auto delim = read_traits<P>::list_start(p);
        for (T& e : a()) {
          read<P, T>::from(p, e);
          delim = read_traits<P>::list_delemiter(p);
        }
        read_traits<P>::list_end(p, delim);
      }
    };

    /// read element with name of a tuple
    template<std::size_t I, typename P, typename ... Properties>
    struct read_named {
      static void property (P& p, const std::string& name, std::tuple<type<Properties>&...>& t) {
        auto& f = std::get<I - 1>(t);
        if (name == f.name()) {
          read_property(p, f);
        } else {
          read_named<I - 1, P, Properties...>::property(p, name, t);
        }
      }
    };

    /// Stop recoursion at element 0
    template<typename P, typename ... Properties>
    struct read_named<0, P, Properties...> {
      static inline void property (P& p, const std::string& name, std::tuple<type<Properties>& ...>&) {
        throw std::runtime_error(ostreamfmt("Could not find property with name '" << name << "'!"));
      }
    };

    /// read tuple
    template<typename P, typename ... Properties>
    struct read_t {
      static void from (P& p, std::tuple<type<Properties>&...>& t) {
        auto delim = read_traits<P>::object_start(p);
        while (read_traits<P>::object_continue(p, delim)) {
          std::string name;
          read_traits<P>::object_key(p, name);
          read_named<sizeof...(Properties), P, Properties...>::property(p, name, t);
          delim = read_traits<P>::object_delemiter(p);
        };
        read_traits<P>::object_end(p, delim);
      }
    };

    /// read tuple
    template<typename P, typename... Properties>
    void inline read_tuple (P& p, std::tuple<type<Properties>&...>& t) {
      read_t<P, std::tuple<type<Properties>&...>>::from(p, t);
    }

    /// read basic_struct
    template<typename P, typename ... Properties>
    struct read<P, basic_struct<Properties...>> {
      static inline void from (P& p, basic_struct<Properties...>& t) {
        read_t<P, std::tuple<type<Properties>&...>>::from(p, t.properites());
      }
    };

    /// read basic_struct
    template<typename P, typename ... Properties>
    void inline read_struct (P& p, basic_struct<Properties...>& t) {
      read<P, basic_struct<Properties...>>::from(p, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for istream
    //
    template<>
    struct read_traits<std::istream> {
      static inline char list_start (std::istream& p) {
        return read_char(p, '[');
      }

      static inline char list_delemiter (std::istream& p) {
        char delim = 0;
        p >> std::ws >> delim;
        if ((delim != ',') && (delim != ']')) {
          throw std::runtime_error(ostreamfmt("Expected coma ',' or array close bracket ']' but got '" << delim << "'!"));
        }
        return delim;
      }

      static inline bool list_continue (std::istream& p, char delim) {
        return (delim != ']') && p.good();
      }

      static inline void list_end (std::istream& p, char delim) {
        if (delim != ']') {
          throw std::runtime_error(ostreamfmt("Expected close bracket ']' but got '" << delim << "'!"));
        }
      }

      static inline char object_key (std::istream& p, std::string& key) {
        p >> std::ws >> util::string::name(key);
        return read_char(p, ':');
      }

      static inline char object_start (std::istream& p) {
        return read_char(p, '{');
      }

      static inline char object_delemiter (std::istream& p) {
        char delim = 0;
        p >> std::ws >> delim;
        if ((delim != ',') && (delim != '}')) {
          throw std::runtime_error(ostreamfmt("Expected coma ',' or curly bracket '}' but got '" << delim << "'!"));
        }
        return delim;
      }

      static inline bool object_continue (std::istream& p, char delim) {
        return (delim != '}') && p.good();
      }

      static inline void object_end (std::istream& p, char delim) {
        if (delim != '}') {
          throw std::runtime_error(ostreamfmt("Expected struct close bracket '}' but got '" << delim << "'!"));
        }
      }

    private:
      static inline char read_char (std::istream& p, const char expected) {
        char delim = 0;
        p >> std::ws >> delim;
        if (delim != expected) {
          throw std::runtime_error(ostreamfmt("Expected character '" << expected
                                              << "' but got '" << delim << "'!"));
        }
        return delim;
      }
    };

    template<typename T>
    struct read<std::istream, T, void> {
      /// read a named value.
      static void from (std::istream& p, T& t) {
        p >> t;
      }
    };

    template<>
    struct read<std::istream, std::string, void> {
      static inline void from (std::istream& is, std::string& t) {
        is >> std::ws >> util::string::quoted(t);
      }
    };

    template<>
    struct read<std::istream, unsigned char, void> {
      static inline void from (std::istream& is, unsigned char& t) {
        int i;
        is >> std::ws >> i;
        t = static_cast<unsigned char>(i);
      }
    };

    template<>
    struct read<std::istream, char, void> {
      static inline void from (std::istream& is, char& t) {
        int i;
        is >> std::ws >> i;
        t = static_cast<char>(i);
      }
    };

  } // namespace io

} // namespace persistent

namespace std {

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  inline std::ostream& operator << (std::ostream& os, 
                                    const persistent::basic_struct<Properties...>& t) {
    persistent::io::write<std::ostream, persistent::basic_struct<Properties...>>::to(os, t);
    return os;
  }

  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const persistent::type<T>& t) {
    persistent::io::write<std::ostream, persistent::type<T>>::to(os, t);
    return os;
  }

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  inline std::istream& operator >> (std::istream& is, persistent::basic_struct<Properties...>& t) {
    persistent::io::read<std::istream, persistent::basic_struct<Properties...>>::from(is, t);
    return is;
  }

  template<typename T>
  inline std::istream& operator >> (std::istream& is, const persistent::type<T>& t) {
    persistent::io::read<std::istream, persistent::type<T>>::from(is, t);
    return is;
  }

}

// --------------------------------------------------------------------------
