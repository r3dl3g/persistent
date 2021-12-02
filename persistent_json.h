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

// --------------------------------------------------------------------------
//
// Library includes
//

// --------------------------------------------------------------------------
//
// Project includes
//
#include "persistent_ios.h"


namespace persistent {

  namespace io {

    // --------------------------------------------------------------------------
    //
    // specializations for json formatted ostream
    //
    struct json_formatter : public ios_formatter {
      json_formatter (std::ostream& os, bool beautify = true)
        : ios_formatter(os, beautify)
      {}
    };

    template<>
    struct write_traits<json_formatter> : public write_traits<ios_formatter> {

      static void write_property_init (json_formatter& out, const std::string& key) {
        out.os << '"' << key << "\":";
        if (out.beautify) {
          out.os << " ";
        }
      }

    };

    template<typename T>
    struct write_value_t<json_formatter, T> {
      static void to (json_formatter& out, const T& t) {
        out.os << '"' << t << '"';
      }
    };

    template<typename T>
    void inline write_json (std::ostream& os, const T& t, bool beautify = true) {
      json_formatter out(os, beautify);
      write_any(out, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for json formatted istream
    //
    struct json_parser {
      json_parser (std::istream& is)
        : is(is)
      {}

      std::istream& is;
    };

    template<>
    struct read_traits<json_parser> {
      static void read_list_start (json_parser& in) {
        read_traits<std::istream>::read_list_start(in.is);
      }

      static bool read_list_element_init (json_parser& in, bool first) {
        return read_traits<std::istream>::read_list_element_init(in.is, first);
      }

      static void read_list_element_finish (json_parser& in) {
        read_traits<std::istream>::read_list_element_finish(in.is);
      }

      static void read_list_end (json_parser& in) {
        read_traits<std::istream>::read_list_end(in.is);
      }

      static void read_property_init (json_parser& in, std::string& key) {
        in.is >> std::ws >> util::string::quoted(key);
        read_traits<std::istream>::read_char(in.is, ':');
      }

      static void read_property_finish (json_parser&, const std::string&) {
      }

      static bool read_object_element_init (json_parser& in, std::string& key) {
        char delim = 0;
        in.is >> std::ws >> delim >> std::ws;
        if ((delim != ',') && (delim != '{') && (delim != '}')) {
          throw std::runtime_error(ostreamfmt("Expected coma ',' or curly bracket '{' or '}' but got '" << delim << "'!"));
        }
        if (in.is.good() && (delim == '{') && (in.is.peek() == '}')) {
          in.is >> delim;
        }
        if (in.is.good() && (delim != '}')) {
          read_property_init(in, key);
          return true;
        }
        return false;
      }

      static void read_object_element_finish (json_parser&, const std::string&) {}

    };

    template<typename T>
    struct read_value_t<json_parser, T> {
      static void from (json_parser& in, T& t) {
        in.is >> std::ws;
        if ((in.is.peek() != '"') && (in.is.peek() != '\'')) {
          throw std::runtime_error("Expected string delemiter ' or \" !");
        }
        char delim;
        in.is >> delim;
        in.is >> t;
        char delim2;
        in.is >> delim2;
        if (delim != delim2) {
          throw std::runtime_error(ostreamfmt("Expected string delemiter " << delim << " but got " << delim2 << " !"));
        }
      }
    };

    template<>
    struct read_value_t<json_parser, std::string> {
      static void from (json_parser& in, std::string& t) {
        read_value(in.is, t);
      }
    };

    template<typename T>
    void read_json (std::istream& is, T& t) {
      json_parser f(is);
      read(f, t);
    }


  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
