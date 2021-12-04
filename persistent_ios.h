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

      static void write_list_element_init (std::ostream& os, int num) {
        if (num) {
          os << ',';
        }
      }

      static void write_list_element_finish (std::ostream&) {}

      static void write_members_delemiter (std::ostream& os) {
        os << ',';
      }

      static void write_list_start (std::ostream& os) {
        os << '[';
      }

      static void write_list_end (std::ostream& os) {
        os << ']';
      }

      static void write_property_init (std::ostream& os, const std::string& key) {
        os << key << ':';
      }

      static void write_property_finish (std::ostream&, const std::string&) {
      }

      static void write_object_start (std::ostream& os) {
        os << '{';
      }

      static void write_object_end (std::ostream& os) {
        os << '}';
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

    template<typename T>
    void write_stream(std::ostream& os, const T& t) {
      write(os, t);
    }


    // --------------------------------------------------------------------------
    //
    // specializations for formatted ostream
    //
    struct ios_formatter {

      ios_formatter (std::ostream& os, bool beautify = true)
        : os(os)
        , beautify(beautify)
      {}

      std::ostream& os;

      int deepth = 0;
      bool multi_line = false;
      bool beautify;

      inline ios_formatter& inc () {
        ++deepth;
        return *this;
      }

      inline ios_formatter& dec () {
        --deepth;
        return *this;
      }

      ios_formatter& fill () {
        if (beautify && multi_line) {
          for (int i = 0; i < deepth; ++i) {
            os << "  ";
          }
          multi_line = false;
        }
        return *this;
      }

      ios_formatter& endl () {
        if (beautify) {
          os << std::endl;
          multi_line = true;
        }
        return *this;
      }

    };

    template<>
    struct write_traits<ios_formatter> {

      static void write_list_element_init (ios_formatter& out, int num) {
        if (num) {
          out.os << ',';
          out.endl().fill();
        }
      }

      static void write_list_element_finish (ios_formatter&) {}

      static void write_members_delemiter (ios_formatter& out) {
        out.os << ',';
        out.endl().fill();
      }

      static void write_list_start (ios_formatter& out) {
        out.os << "[";
        out.endl().inc().fill();
      }

      static void write_list_end (ios_formatter& out) {
        out.endl().dec().fill().os << "]";
      }

      static void write_property_init (ios_formatter& out, const std::string& key) {
        out.os << key << ": ";
      }

      static void write_property_finish (ios_formatter& out, const std::string&) {
      }

      static void write_object_start (ios_formatter& out) {
        out.os << '{';
        out.endl().inc().fill();
      }

      static void write_object_end (ios_formatter& out) {
        out.endl().dec().fill().os << '}';
      }
    };

    template<typename T>
    struct write_value_t<ios_formatter, T> {
      static void to (ios_formatter& out, const T& t) {
        write_value(out.os, t);
      }
    };

    template<typename T>
    void write_formatted (std::ostream& os, const T& t, bool beautify = true) {
      ios_formatter out(os, beautify);
      write(out, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for istream
    //
    template<>
    struct read_traits<std::istream> {
      static inline void read_list_start (std::istream& is) {
        read_char(is, '[');
      }

      static inline bool read_list_element_init (std::istream& is, int) {
        is >> std::ws;
        char delim = is.peek();
        return is.good() && (delim != ']');
      }

      static inline void read_list_element_finish (std::istream& is) {
        is >> std::ws;
        char delim = is.peek();
        if ((delim != ',') && (delim != ']')) {
          throw std::runtime_error(ostreamfmt("Expected coma ',' or array close bracket ']' but got '" << delim << "'!"));
        }
        if (delim == ',') {
          is >> delim;
        }
      }

      static inline void read_list_end (std::istream& is) {
        read_char(is, ']');
      }

      static inline void read_property_init (std::istream& is, std::string& key) {
        is >> std::ws >> util::string::name(key);
        read_char(is, ':');
      }

      static inline void read_property_finish (std::istream&, const std::string&) {}

      static inline bool read_object_element_init (std::istream& is, std::string& key) {
        char delim = 0;
        is >> std::ws >> delim >> std::ws;
        if ((delim != ',') && (delim != '{') && (delim != '}')) {
          throw std::runtime_error(ostreamfmt("Expected coma ',' or curly bracket '{' or '}' but got '" << delim << "'!"));
        }
        if (is.good() && (delim == '{') && (is.peek() == '}')) {
          is >> delim;
        }
        if (is.good() && (delim != '}')) {
          read_property_init(is, key);
          return true;
        }
        return false;
      }

      static inline void read_object_element_finish (std::istream&, std::string&) {}

      static inline char read_char (std::istream& is, const char expected) {
        char delim = 0;
        is >> std::ws >> delim;
        if (delim != expected) {
          throw std::runtime_error(ostreamfmt("Expected character '" << expected
                                              << "' but got '" << delim << "'!"));
        }
        return delim;
      }
    };

    template<typename T>
    struct read_value_t<std::istream, T> {
      static void from (std::istream& is, T& t) {
        is >> t;
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

    template<typename T>
    void read_stream (std::istream& is, T& t) {
      read(is, t);
    }


  } // namespace io

} // namespace persistent

namespace std {

  // --------------------------------------------------------------------------
  template<typename ... Types>
  inline std::ostream& operator << (std::ostream& os, 
                                    const persistent::basic_struct<Types...>& t) {
    persistent::io::write(os, t);
    return os;
  }

  template<typename T>
  inline std::ostream& operator << (std::ostream& os, const persistent::type<T>& t) {
    persistent::io::write(os, t);
    return os;
  }

  // --------------------------------------------------------------------------
  template<typename ... Types>
  inline std::istream& operator >> (std::istream& is, persistent::basic_struct<Types...>& t) {
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
