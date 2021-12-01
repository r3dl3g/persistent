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
#include "persistent_ios.h"


namespace persistent {

  namespace io {

    // --------------------------------------------------------------------------
    //
    // specializations for ostream
    //
    struct xml_formatter : public ios_formatter {
      xml_formatter (std::ostream& os)
        : ios_formatter(os)
      {}

    };

    namespace xml {
      const std::string s_header = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
      const std::string s_body = "<body>";
      const std::string s_nbody = "</body>";
    }

    template<>
    struct write_traits<xml_formatter> {

      static void list_start (xml_formatter& out) {
        out.endl().fill().os << "<ol>",
        out.endl().inc();
      }

      static void list_element_init (xml_formatter& out, bool) {
        out.fill().inc().os << "<li>";
      }

      static void list_element_finish (xml_formatter& out) {
        out.dec().fill().os << "</li>";
        out.endl();
      }

      static void list_end (xml_formatter& out) {
        out.dec().fill().os << "</ol>";
        out.endl();
      }

      static void members_delemiter (xml_formatter&) {
      }

      static void property_init (xml_formatter& out, const std::string& name) {
        out.fill().inc().os << "<" << name << '>';
      }

      static void property_finish (xml_formatter& out, const std::string& name) {
        out.dec().fill().os << "</" << name << '>';
        out.endl();
      }

      static void object_start (xml_formatter&) {
      }

      static void object_end (xml_formatter&) {
      }
    };

    template<typename T>
    struct write_value_t<xml_formatter, T> {
      static void to (xml_formatter& out, const T& t) {
        write_value(out.os, t);
      }
    };

    template<typename T>
    void write_xml (std::ostream& os, const T& t) {
      xml_formatter out(os);
      os << xml::s_header << std::endl;
      os << xml::s_body;
      out.endl().inc();
      write(out, t);
      out.dec();
      os << xml::s_nbody;
      out.endl();
    }

    // --------------------------------------------------------------------------
    //
    // specializations for xml formatted istream
    //
    struct xml_parser {
      xml_parser (std::istream& is)
        : is(is)
      {}

      std::istream& is;

      const std::string& next_token () const {
        return token;
      }

      const std::string& next () {
        is >> std::ws;
        std::ostringstream str;
        util::string::copy_until(std::istreambuf_iterator<char>(is),
                                 std::istreambuf_iterator<char>(),
                                 std::ostreambuf_iterator<char>(str),
                                 [] (char c) {
          return (c != '>');
        });
        char next = is.peek();
        if (next != '>') {
          throw std::runtime_error(ostreamfmt("Expected '>' but got '" << next << "'!"));
        }
        is >> next;
        return token = str.str() + next;
      }

      void check_token (const std::string& expected) const {
        if (token != expected) {
          throw std::runtime_error(ostreamfmt("Expected '" << expected << "' but got '" << token << "'!"));
        }
      }


    private:
      std::string token;

    };

    template<>
    struct read_traits<xml_parser> {

      static char list_start (xml_parser& in) {
        in.next();
        in.check_token("<ol>");
        in.next();
        return ' ';
      }

      static bool list_element_init (xml_parser& in, char c, bool first) {
        return in.next_token() == "<li>";
      }

      static char list_element_finish (xml_parser& in) {
        in.next();
        in.check_token("</li>");
        in.next();
        return ' ';
      }

      static void list_end (xml_parser& in, char c) {
        in.check_token("</ol>");
      }

      static char property_init (xml_parser& in, std::string& key) {
        const std::string& token = in.next_token();
        if ((token.size() < 3) || (token.front() != '<') || (token.back() != '>')) {
          throw std::runtime_error(ostreamfmt("Expected '<xyz>' but got '" << token << "'!"));
        }
        key = token.substr(1, token.size() - 2);
        return '>';
      }

      static void property_finish (xml_parser& in, const std::string& key) {
        in.next();
        in.check_token("</" + key + ">");
        in.next();
      }

      static char object_delemiter (xml_parser&) {
        return ' ';
      }

      static bool object_continue (xml_parser& in, char c) {
        return in.is.good() && !util::string::starts_with(in.next_token(), "</");
      }

      static char object_start (xml_parser& in) {
        in.next();
        return ' ';
      }

      static void object_end (xml_parser& in, char c) {
      }

    };

    template<typename T>
    struct read_value_t<xml_parser, T> {
      static void from (xml_parser& in, T& t) {
        read_value(in.is, t);
      }
    };

    template<typename T>
    void read_xml (std::istream& is, T& t) {
      xml_parser in(is);
      in.next();
      in.check_token(xml::s_header);
      in.next();
      in.check_token(xml::s_body);
      read(in, t);
      in.check_token(xml::s_nbody);
    }

  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
