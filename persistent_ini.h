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
#include <deque>

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
    struct ini_formatter {
      ini_formatter (std::ostream& os)
        : os(os)
      {}

      void print_path () {
        if (path.empty()) {
          return;
        }
        os << "[";
        bool first = true;
        for (const auto& p : path) {
          if (first) {
            first = false;
          } else {
            os << '.';
          }
          os << p;
        }
        os << "]" << std::endl;
      }

      std::ostream& os;
      std::deque<std::string> path;
      std::string last_key;
    };

    template<>
    struct write_traits<ini_formatter> {

      static void write_property_init (ini_formatter& out, const std::string& key) {
        out.last_key = key;
        out.os << key << "=";
      }

      static void write_property_finish (ini_formatter&, const std::string&) {
      }

      static void write_object_start (ini_formatter& out) {
        if (!out.last_key.empty()) {
          out.path.push_back(out.last_key);
          out.print_path();
        }
      }

      static void write_object_end (ini_formatter& out) {
        out.os << std::endl;
        if (!out.path.empty()) {
          out.path.pop_back();
          out.last_key.erase();
        }
      }

      static void write_members_delemiter (ini_formatter& out) {
        out.os << std::endl;
      }

      static void write_list_element_init (ini_formatter& out, bool first) {
        write_traits<std::ostream>::write_list_element_init(out.os, first);
      }

      static void write_list_element_finish (ini_formatter& out) {
        write_traits<std::ostream>::write_list_element_finish(out.os);
      }

      static void write_list_start (ini_formatter& out) {
        write_traits<std::ostream>::write_list_start(out.os);
      }

      static void write_list_end (ini_formatter& out) {
        write_traits<std::ostream>::write_list_end(out.os);
      }

    };

    template<typename T>
    struct write_value_t<ini_formatter, T> {
      static void to (ini_formatter& out, const T& t) {
        write_value(out.os, t);
      }
    };

    template<typename T>
    void write_ini (std::ostream& os, const T& t) {
      ini_formatter out(os);
      write(out, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for json formatted ostream
    //
    struct ini_parser {
      ini_parser (std::istream& is)
        : is(is)
      {}

      std::istream& is;
    };

    template<>
    struct read_traits<ini_parser> : public read_traits<std::istream> {
      static void read_list_start (ini_parser& in) {
        read_traits<std::istream>::read_list_start(in.is);
      }

      static bool read_list_element_init (ini_parser& in, bool first) {
        return read_traits<std::istream>::read_list_element_init(in.is, first);
      }

      static void read_list_element_finish (ini_parser& in) {
        read_traits<std::istream>::read_list_element_finish(in.is);
      }

      static void read_list_end (ini_parser& in) {
        return read_traits<std::istream>::read_list_end(in.is);
      }

      static void read_property_init (ini_parser& in, std::string& key) {
        std::getline(in.is >> std::ws, key, '=');
      }

      static void read_property_finish (ini_parser&, const std::string&) {
      }

      static bool read_object_element_init (ini_parser& in, std::string& key) {
        in.is >> std::ws;
        if (in.is.good() && (in.is.peek() != '[')) {
          std::getline(in.is >> std::ws, key, '=');
          return true;
        }
        return false;
      }

      static void read_object_element_finish (ini_parser&, const std::string&) {}

    };

    template<typename T>
    struct read_value_t<ini_parser, T> {
      static void from (ini_parser& in, T& t) {
        read_value(in.is, t);
      }
    };

    template<typename T>
    void read_ini (std::istream& is, T& t) {
      ini_parser in(is);
      read(in, t);
    }


  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
