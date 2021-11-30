/**
* @copyright (c) 2015-2021 Ing. Buero Rothfuss
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
#include <iostream>

// --------------------------------------------------------------------------
//
// Library includes
//

// --------------------------------------------------------------------------
//
// Project includes
//
#include "persistent.h"


namespace persistent {

  namespace io {

    // --------------------------------------------------------------------------
    //
    // specializations for ostream
    //
    template<>
    struct write_traits<std::ostream> {

      static void list_delemiter (std::ostream& p) {
        p << ',';
      }

      static void members_delemiter (std::ostream& p) {
        p << ',';
      }

      static void list_start (std::ostream& p) {
        p << '[';
      }

      static void list_end (std::ostream& p) {
        p << ']';
      }

      static void object_key (std::ostream& p, const std::string& key) {
        p << key << ':';
      }

      static void object_start (std::ostream& p) {
        p << '{';
      }

      static void object_end (std::ostream& p) {
        p << '}';
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
    // specializations for formatted ostream
    //
    struct formatter {

      formatter (std::ostream& os)
        : os(os)
      {}

      std::ostream& os;
      int deepth = 0;

      inline void inc () {
        ++deepth;
      }

      inline void dec () {
        --deepth;
      }
    };

    template<>
    struct write_traits<formatter> {

      static void fill (formatter& f) {
        for (int i = 0; i < f.deepth; ++i) {
          f.os << "    ";
        }
      }

      static void list_delemiter (formatter& f) {
        f.os << ',' << std::endl;
        fill(f);
      }

      static void members_delemiter (formatter& f) {
        f.os << ',' << std::endl;
        fill(f);
      }

      static void list_start (formatter& f) {
        f.os << "[" << std::endl;
        f.inc();
        fill(f);
      }

      static void list_end (formatter& f) {
        f.os << std::endl;
        f.dec();
        fill(f);
        f.os << "]";
      }

      static void object_key (formatter& f, const std::string& key) {
        f.os << key << ": ";
      }

      static void object_start (formatter& f) {
        f.os << '{' << std::endl;
        f.inc();
        fill(f);
      }

      static void object_end (formatter& f) {
        f.os << std::endl;
        f.dec();
        fill(f);
        f.os << '}';
      }
    };

    template<typename T>
    struct write_value_t<formatter, T> {
      static void to (formatter& f, const T& t) {
        write_value(f.os, t);
      }
    };

    template<typename T>
    void write_formatted (std::ostream& os, const T& t) {
      formatter f(os);
      write(f, t);
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
