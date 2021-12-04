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

    struct ini_path {

      std::ostream& print (std::ostream& os) {
        bool first = true;
        for (const auto& p : path) {
          if(first) {
            first = false;
          } else {
            os << '.';
          }
          os << p;
        }
        return os;
      }

      inline void set (const std::string& elements) {
        path = util::string::split<'.'>(elements);
      }

      inline bool match (const ini_path& rhs) const {
        return (this == &rhs) || rhs.path == path;
      }

      inline bool operator== (const ini_path& rhs) const {
        return match(rhs);
      }

      const inline std::string& element (int i) const {
        return path.at(i);
      }

      inline std::size_t size () const {
        return path.size();
      }

      bool is_parent_of (const ini_path& key) const {
        typedef std::vector<std::string>::const_iterator it;
        const it ke = key.path.end();
        const it pe = path.end();
        it p = path.begin();
        for (it k = key.path.begin(); (k < ke) && (p < pe); k++, p++) {
          if (*k != *p) {
            return false;
          }
        }
        return p == pe;
      }

      inline void push (const std::string& element) {
        path.push_back(element);
      }

      inline void pop () {
        path.pop_back();
      }

    private:
      std::vector<std::string> path;
    };

    // --------------------------------------------------------------------------
    //
    // specializations for json formatted ostream
    //
    struct ini_formatter_context {
      ini_formatter_context (std::ostream& os)
        : os(os)
      {}

      ini_path path;

      std::ostream& print_path () {
        return path.print(os) << '=';
      }

      void endl () {
        os << std::endl;
      }

    private:
      std::ostream& os;
    };

    template<>
    struct formatter<ini_formatter_context> {

      static void write_property_init (ini_formatter_context& out, const std::string& key) {
        out.path.push(key);
      }

      static void write_property_finish (ini_formatter_context& out, const std::string&) {
        out.path.pop();
      }

      static void write_struct_start (ini_formatter_context& out) {
      }

      static void write_struct_end (ini_formatter_context& out) {
      }

      static void write_members_delemiter (ini_formatter_context& out) {
      }

      static void write_list_element_init (ini_formatter_context& out, int num) {
        out.path.push(ostreamfmt(num));
      }

      static void write_list_element_finish (ini_formatter_context& out) {
        out.path.pop();
      }

      static void write_list_start (ini_formatter_context& out) {
      }

      static void write_list_end (ini_formatter_context& out) {
      }

    };

    template<typename T>
    struct write_value_t<ini_formatter_context, T> {
      static void to (ini_formatter_context& out, const T& t) {
        write_value(out.print_path(), t);
        out.endl();
      }
    };

    template<typename T>
    void write_ini (std::ostream& os, const T& t) {
      ini_formatter_context out(os);
      write(out, t);
    }

    // --------------------------------------------------------------------------
    //
    // specializations for json formatted ostream
    //
    struct ini_parser_context {
      ini_parser_context (std::istream& is)
        : is(is)
      {}

      ini_path path;
      ini_path key;
      std::istream& is;

      bool match() const {
        return path == key;
      }

    };

    template<>
    struct parser<ini_parser_context> {
      static void read_list_start (ini_parser_context&) {}
      static bool read_list_element_init (ini_parser_context&, int) {
        return false;
      }

      static void read_list_element_finish (ini_parser_context&) {}
      static void read_list_end (ini_parser_context&) {}
      static void read_property_init (ini_parser_context&, std::string&) {}
      static void read_property_finish (ini_parser_context&, const std::string&) {}
      static bool read_next_struct_element (ini_parser_context&, std::string&) {
        return false;
      }

      static void read_struct_element_finish (ini_parser_context&, const std::string&) {}

    };

    template<typename T>
    struct read_property_t<ini_parser_context, T> {
      static void from (ini_parser_context& in, prop<T>& t) {
        in.path.push(t.name());
        read_any(in, t());
        in.path.pop();
      }
    };

    template<typename T>
    struct read_vector_t<ini_parser_context, T> {
      static void from (ini_parser_context& in, std::vector<T>& v) {
        if (in.path.is_parent_of(in.key)) {
          const std::string& index = in.key.element(in.path.size());
          in.path.push(index);
          const int idx = std::stoi(index);
          if (v.size() < idx + 1) {
            v.resize(idx + 1);
          }
          read_any(in, v.at(idx));
          in.path.pop();
        }
      }
    };

    template<typename T, std::size_t S>
    struct read_array_t<ini_parser_context, T, S> {
      static void from (ini_parser_context& in, std::array<T, S>& a) {
        if (in.path.is_parent_of(in.key)) {
          const std::string& index = in.key.element(in.path.size());
          in.path.push(index);
          const int idx = std::stoi(index);
          read_any(in, a.at(idx));
          in.path.pop();
        }
      }
    };

    template<typename ... Types>
    struct read_tuple_t<ini_parser_context, Types...> {
      static void from (ini_parser_context& in, std::tuple<prop<Types>&...>& t) {
        if (in.path.is_parent_of(in.key)) {
          const std::string& index = in.key.element(in.path.size());
          in.path.push(index);
          read_named<sizeof...(Types), ini_parser_context, Types...>::property(in, index, t);
          in.path.pop();

        }
      }
    };

    template<typename T>
    struct read_value_t<ini_parser_context, T> {
      static void from (ini_parser_context& in, T& t) {
        if (in.match()) {
          read_value(in.is, t);
        }
      }
    };

    template<typename T>
    void read_ini (std::istream& is, T& t) {
      ini_parser_context in(is);
      is >> std::ws;
      while (is.good()) {
        std::string key;
        is >> util::string::name(key);
        in.key.set(key);
        is >> std::ws;
        is.ignore(std::numeric_limits<std::streamsize>::max(), '=');
        is >> std::ws;
        read(in, t);
        is >> std::ws;
      }
    }


  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
