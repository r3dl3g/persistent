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

    /**
     * traits to allow target specific prefix and suffixes.
     */
    template<typename P>
    struct write_traits {
      static void list_delemiter (P&) {}
      static void members_delemiter (P&) {}
      static void list_start (P&) {}
      static void list_end (P&) {}
      static void object_key (P&, const std::string& key) {}
      static void object_start (P&) {}
      static void object_end (P&) {}
    };

    /**
    * write plain value
    * Allows specialized implementations.
    */
    template<typename P, typename T>
    struct write_value_t {
      static void to (P& p, const T& t);
    };

    template<typename P, typename T>
    void write_value (P& p, T& t) {
      write_value_t<P, T>::to(p, t);
    }

    /**
    * write any type
    * type will be detected in later specializations
    */
    template<typename P, typename T, typename Enable = void>
    struct write_any_t {
      static void to (P& p, const T& t) {
        write_value(p, t);
      }
    };

    template<typename P, typename T>
    void write_any (P& p, const T& t) {
      write_any_t<P, T>::to(p, t);
    }

    /**
     *  write named property
     */
    template<typename P, typename T>
    struct write_property_t {
      static void to (P& p, const type<T>& t) {
        write_traits<P>::object_key(p, t.name());
        write_any(p, t());
      }
    };

    template<typename P, typename T>
    void write_property (P& p, const type<T>& t) {
      write_property_t<P, T>::to(p, t);
    }

    /// write vector and array values
    template<typename P, typename T, typename V>
    struct writeList_t {
      static void to (P& p, const V& v) {
        write_traits<P>::list_start(p);
        bool first = true;
        for (const T& t : v) {
          if (first) {
            first = false;
          } else {
            write_traits<P>::list_delemiter(p);
          }
          write_any(p, t);
        }
        write_traits<P>::list_end(p);
      }
    };

    /// write vector values
    template<typename P, typename T>
    struct write_vector_t {
      static void to (P& p, const std::vector<T>& t) {
        writeList_t<P, T, std::vector<T>>::to(p, t);
      }
    };

    template<typename P, typename T>
    void write_vector (P& p, const std::vector<T>& t) {
      write_vector_t<P, T>::to(p, t);
    }

    /// write array values
    template<typename P, typename T, size_t S>
    struct write_array_t {
      static void to (P& p, const std::array<T, S>& t) {
        writeList_t<P, T, std::array<T, S>>::to(p, t());
      }
    };

    template<typename P, typename T, size_t S>
    void write_array (P& p, const std::array<T, S>& t) {
      write_array_t<P, T, S>::to(p, t);
    }

    /**
    * write recursive all elements of a tuple
    */
    template<std::size_t I, typename P, typename... Properties>
    struct write_nth {
      static void to (P& p, std::tuple<type<Properties>&...> const& t) {
        write_nth<I - 1, P, Properties...>::to(p, t);
        write_traits<P>::members_delemiter(p);
        const auto& m = std::get<I>(t);
        write_any(p, m);
      }
    };

    /// end recursion at 0 elemnt.
    template<typename P, typename... Properties>
    struct write_nth<0, P, Properties...> {
      static void to (P& p, std::tuple<type<Properties>&...> const& t) {
        const auto& m = std::get<0>(t);
        write_any(p, m);
      }
    };

    /// write tuple
    template<typename P, typename ... Properties>
    struct write_tuple_t {
      static void to (P& p, const std::tuple<type<Properties>&...>& t) {
        write_traits<P>::object_start(p);
        write_nth<(sizeof...(Properties)) - 1, P, Properties...>::to(p, t);
        write_traits<P>::object_end(p);
      }
    };

    template<typename P, typename... Properties>
    void write_tuple (P& p, const std::tuple<type<Properties>&...>& t) {
      write_tuple_t<P, Properties...>::to(p, t);
    }

    /// write basic_struct
    template<typename P, typename ... Properties>
    struct write_struct_t {
      static void to (P& p, const basic_struct<Properties...>& t) {
        write_tuple(p, t.properites());
      }
    };

    /// write struct helper
    template<typename P, typename ... Properties>
    void write_struct (P& p, const basic_struct<Properties...>& t) {
      write_struct_t<P, Properties...>::to(p, t);
    }

    /// detect named property
    template<typename P, typename T>
    struct write_any_t<P, type<T>> {
      static void to (P& p, const type<T>& t) {
        write_property(p, t);
      }
    };

    /// detect vector
    template<typename P, typename T>
    struct write_any_t<P, std::vector<T>> {
      static void to (P& p, const std::vector<T>& t) {
        write_vector(p, t);
      }
    };

    /// detect array
    template<typename P, typename T, std::size_t S>
    struct write_any_t<P, std::array<T, S>> {
      static void to (P& p, const std::array<T, S>& t) {
        write_array(p, t);
      }
    };

    /// detect struct
    template<typename P, typename T>
    struct write_any_t<P, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void to (P& p, const T& t) {
        write_struct(p, t);
      }
    };

    /// convenience helper
    template<typename P, typename T>
    void inline write (P& p, const T& t) {
      write_any(p, t);
    }

    // --------------------------------------------------------------------------
    //
    // read
    //
    // --------------------------------------------------------------------------

    /**
     * traits to allow source specific parsing.
     */
    template<typename P>
    struct read_traits {
      static char list_start (P&) { return ' '; }
      static char list_delemiter (P&) { return ' '; }
      static bool list_continue (P&, char) { return true; }
      static void list_end (P&, char) {}
      static char object_key (P&, std::string&) { return ' '; }
      static char object_start (P&) { return ' '; }
      static char object_delemiter (P&) { return ' '; }
      static bool object_continue (P&, char) { return true; }
      static void object_end (P&, char) {}
    };

    /// read value
    template<typename P, typename T>
    struct read_value_t {
      static void from (P& p, T& t);
    };

    template<typename P, typename T>
    void read_value (P& p, T& t) {
      read_value_t<P, T>::from(p, t);
    }

    /**
    * read any type
    * type will be detected in later specializations
    */
    template<typename P, typename T, typename Enable = void>
    struct read_any_t {
      static void from (P& p, T& t) {
        read_value(p, t);
      }
    };

    template<typename P, typename T>
    void read_any (P& p, T& t) {
      read_any_t<P, T>::from(p, t);
    }

    /// read property
    template<typename P, typename T>
    struct read_property_t {
      static void from (P& p, type<T>& t) {
        read_any(p, t());
      }
    };

    template<typename P, typename T>
    void read_property (P& p, type<T>& t) {
      read_property_t<P, T>::from(p, t);
    }

    /// read vector
    template<typename P, typename T>
    struct read_vector_t {
      static void from (P& p, std::vector<T>& v) {
        auto delim = read_traits<P>::list_start(p);
        while (read_traits<P>::list_continue(p, delim)) {
          T t;
          read_any(p, t);
          v.push_back(t);
          delim = read_traits<P>::list_delemiter(p);
        }
        read_traits<P>::list_end(p, delim);
      }
    };

    template<typename P, typename T>
    void read_vector (P& p, std::vector<T>& t) {
      read_vector_t<P, T>::from(p, t);
    }

    /// read array
    template<typename P, typename T, std::size_t S>
    struct read_array_t {
      static void from (P& p, std::array<T, S>& a) {
        auto delim = read_traits<P>::list_start(p);
        for (T& e : a) {
          read_any(p, e);
          delim = read_traits<P>::list_delemiter(p);
        }
        read_traits<P>::list_end(p, delim);
      }
    };

    template<typename P, typename T, std::size_t S>
    void read_array (P& p, std::array<T, S>& t) {
      read_array_t<P, T, S>::from(p, t);
    }

    /// read element with name of a tuple
    template<std::size_t I, typename P, typename ... Properties>
    struct read_named {
      static void property (P& p, const std::string& name, std::tuple<type<Properties>&...>& t) {
        auto& f = std::get<I - 1>(t);
        if (name == f.name()) {
          read_any(p, f);
        } else {
          read_named<I - 1, P, Properties...>::property(p, name, t);
        }
      }
    };

    /// Stop recoursion at element 0
    template<typename P, typename ... Properties>
    struct read_named<0, P, Properties...> {
      static void property (P& p, const std::string& name, std::tuple<type<Properties>& ...>&) {
        throw std::runtime_error(ostreamfmt("Could not find property with name '" << name << "'!"));
      }
    };

    /// read tuple
    template<typename P, typename ... Properties>
    struct read_tuple_t {
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

    template<typename P, typename ... Properties>
    void read_tuple (P& p, std::tuple<type<Properties>&...>& t) {
      read_tuple_t<P, Properties...>::from(p, t);
    }

    /// read basic_struct
    template<typename P, typename ... Properties>
    struct read_struct_t {
      static void from (P& p, basic_struct<Properties...>& t) {
        read_tuple(p, t.properites());
      }
    };

    template<typename P, typename ... Properties>
    void read_struct (P& p, basic_struct<Properties...>& t) {
      read_struct_t<P, Properties...>::from(p, t);
    }

    /// detect named property
    template<typename P, typename T>
    struct read_any_t<P, type<T>> {
      static void from (P& p, type<T>& t) {
        read_property(p, t);
      }
    };

    /// detect vector
    template<typename P, typename T>
    struct read_any_t<P, std::vector<T>> {
      static void from (P& p, std::vector<T>& t) {
        read_vector(p, t);
      }
    };

    /// detect array
    template<typename P, typename T, std::size_t S>
    struct read_any_t<P, std::array<T, S>> {
      static void from (P& p, std::array<T, S>& t) {
        read_array(p, t);
      }
    };

    /// detect struct
    template<typename P, typename T>
    struct read_any_t<P, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void from (P& p, T& t) {
        read_struct(p, t);
      }
    };

    /// convenience helper
    template<typename P, typename T>
    void inline read (P& p, T& t) {
      read_any(p, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for ostream
    //
    template<>
    struct write_traits<std::ostream> {

      static void list_delemiter (std::ostream& p) {
        p << ',' << std::endl;
      }

      static void members_delemiter (std::ostream& p) {
        p << ',' << std::endl;
      }

      static void list_start (std::ostream& p) {
        p << '[' << std::endl;
      }

      static void list_end (std::ostream& p) {
        p << std::endl << ']';
      }

      static void object_key (std::ostream& p, const std::string& key) {
        p << key << ':';
      }

      static void object_start (std::ostream& p) {
        p << '{' << std::endl;
      }

      static void object_end (std::ostream& p) {
        p << std::endl << '}';
      }
    };

    template<>
    struct write_value_t<std::ostream, const std::string> {
      static void to (std::ostream& os, const std::string& t) {
        os << util::string::quoted(t);
      }
    };

    template<>
    struct write_value_t<std::ostream, const char*> {
      static void to (std::ostream& os, const char* t) {
        os << util::string::quoted(t);
      }
    };

    template<>
    struct write_value_t<std::ostream, const unsigned char> {
      static void to (std::ostream& os, const unsigned char t) {
        os << +t;
      }
    };

    template<>
    struct write_value_t<std::ostream, const char> {
      static void to (std::ostream& os, const char t) {
        os << +t;
      }
    };

    template<typename T>
    struct write_value_t<std::ostream, T> {
      static void to (std::ostream& os, const T& t) {
        os << t;
      }
    };

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
    struct read_value_t<std::istream, T> {
      static void from (std::istream& p, T& t) {
        p >> t;
      }
    };

    template<>
    struct read_value_t<std::istream, std::string> {
      static void from (std::istream& is, std::string& t) {
        is >> std::ws >> util::string::quoted(t);
      }
    };

    template<>
    struct read_value_t<std::istream, unsigned char> {
      static void from (std::istream& is, unsigned char& t) {
        int i;
        is >> std::ws >> i;
        t = static_cast<unsigned char>(i);
      }
    };

    template<>
    struct read_value_t<std::istream, char> {
      static void from (std::istream& is, char& t) {
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
    persistent::io::write(os, t);
    return os;
  }

  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const persistent::type<T>& t) {
    persistent::io::write(os, t);
    return os;
  }

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  inline std::istream& operator >> (std::istream& is, persistent::basic_struct<Properties...>& t) {
    persistent::io::read(is, t);
    return is;
  }

  template<typename T>
  inline std::istream& operator >> (std::istream& is, const persistent::type<T>& t) {
    persistent::io::read(is, t);
    return is;
  }

}

// --------------------------------------------------------------------------
