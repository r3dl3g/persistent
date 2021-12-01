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

      static void property_init (ini_formatter& out, const std::string& key) {
        out.last_key = key;
        out.os << key << "=";
      }

      static void property_finish (ini_formatter&, const std::string&) {
      }

      static void object_start (ini_formatter& out) {
        if (!out.last_key.empty()) {
          out.path.push_back(out.last_key);
          out.print_path();
        }
      }

      static void object_end (ini_formatter& out) {
        out.os << std::endl;
        if (!out.path.empty()) {
          out.path.pop_back();
          out.last_key.erase();
        }
      }

      static void members_delemiter (ini_formatter& out) {
        out.os << std::endl;
      }

      static void list_element_init (ini_formatter& out, bool first) {
        write_traits<std::ostream>::list_element_init(out.os, first);
      }

      static void list_element_finish (ini_formatter& out) {
        write_traits<std::ostream>::list_element_finish(out.os);
      }

      static void list_start (ini_formatter& out) {
        write_traits<std::ostream>::list_start(out.os);
      }

      static void list_end (ini_formatter& out) {
        write_traits<std::ostream>::list_end(out.os);
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
      static char list_start (ini_parser& in) {
        return read_traits<std::istream>::list_start(in.is);
      }

      static bool list_element_init (ini_parser& in, char c, bool first) {
        return read_traits<std::istream>::list_element_init(in.is, c, first);
      }

      static char list_element_finish (ini_parser& in) {
        return read_traits<std::istream>::list_element_finish(in.is);
      }

      static void list_end (ini_parser& in, char c) {
        return read_traits<std::istream>::list_end(in.is, c);
      }

      static char property_init (ini_parser& in, std::string& key) {
        std::getline(in.is >> std::ws, key, '=');
        return '=';
      }

      static void property_finish (ini_parser&, const std::string&) {
      }

      static char object_delemiter (ini_parser&) {
        return ' ';
      }

      static bool object_continue (ini_parser& in, char c) {
        in.is >> std::ws;
        return in.is.good() && (in.is.peek() != '[');
      }

      static char object_start (ini_parser&) {
        return ' ';
      }

      static void object_end (ini_parser&, char) {
      }

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
