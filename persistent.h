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
#include <map>

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
#include "basic_struct.h"


namespace persistent {

  namespace io {

    // --------------------------------------------------------------------------
    //
    // write
    //
    // --------------------------------------------------------------------------

    /**
     * traits to allow target specific prefix and suffixes.
     */
    template<typename Target>
    struct write_traits {
      static void list_start (Target&) {}
      static void list_element_init (Target&, bool first) {}
      static void list_element_finish (Target&) {}
      static void list_end (Target&) {}

      static void members_delemiter (Target&) {}

      static void property_init (Target&, const std::string&) {}
      static void property_finish (Target&, const std::string&) {}

      static void object_start (Target&) {}
      static void object_end (Target&) {}
    };

    /**
    * write plain value
    * Allows specialized implementations.
    */
    template<typename Target, typename T>
    struct write_value_t {
      static void to (Target& out, const T& t);
    };

    template<typename Target, typename T>
    void write_value (Target& out, T& t) {
      write_value_t<Target, T>::to(out, t);
    }

    /**
    * write any type
    * type will be detected in later specializations
    */
    template<typename Target, typename T, typename Enable = void>
    struct write_any_t {
      static void to (Target& out, const T& t) {
        write_value(out, t);
      }
    };

    template<typename Target, typename T>
    void write_any (Target& out, const T& t) {
      write_any_t<Target, T>::to(out, t);
    }

    /**
     *  write named property
     */
    template<typename Target, typename T>
    struct write_property_t {
      static void to (Target& out, const type<T>& t) {
        write_traits<Target>::property_init(out, t.name());
        write_any(out, t());
        write_traits<Target>::property_finish(out, t.name());
      }
    };

    template<typename Target, typename T>
    void write_property (Target& out, const type<T>& t) {
      write_property_t<Target, T>::to(out, t);
    }

    /// write vector and array values
    template<typename Target, typename T, typename V>
    struct writeList_t {
      static void to (Target& out, const V& v) {
        write_traits<Target>::list_start(out);
        bool first = true;
        for (const T& t : v) {
          write_traits<Target>::list_element_init(out, first);
          first = false;
          write_any(out, t);
          write_traits<Target>::list_element_finish(out);
        }
        write_traits<Target>::list_end(out);
      }
    };

    /// write vector values
    template<typename Target, typename T>
    struct write_vector_t {
      static void to (Target& out, const std::vector<T>& t) {
        writeList_t<Target, T, std::vector<T>>::to(out, t);
      }
    };

    template<typename Target, typename T>
    void write_vector (Target& out, const std::vector<T>& t) {
      write_vector_t<Target, T>::to(out, t);
    }

    /// write array values
    template<typename Target, typename T, size_t S>
    struct write_array_t {
      static void to (Target& out, const std::array<T, S>& t) {
        writeList_t<Target, T, std::array<T, S>>::to(out, t());
      }
    };

    template<typename Target, typename T, size_t S>
    void write_array (Target& out, const std::array<T, S>& t) {
      write_array_t<Target, T, S>::to(out, t);
    }

    /**
    * write recursive all elements of a tuple
    */
    template<std::size_t I, typename Target, typename... Properties>
    struct write_nth {
      static void to (Target& out, std::tuple<type<Properties>&...> const& t) {
        write_nth<I - 1, Target, Properties...>::to(out, t);
        write_traits<Target>::members_delemiter(out);
        const auto& m = std::get<I>(t);
        write_any(out, m);
      }
    };

    /// end recursion at 0 elemnt.
    template<typename Target, typename... Properties>
    struct write_nth<0, Target, Properties...> {
      static void to (Target& out, std::tuple<type<Properties>&...> const& t) {
        const auto& m = std::get<0>(t);
        write_any(out, m);
      }
    };

    /// write tuple
    template<typename Target, typename ... Properties>
    struct write_tuple_t {
      static void to (Target& out, const std::tuple<type<Properties>&...>& t) {
        write_traits<Target>::object_start(out);
        write_nth<(sizeof...(Properties)) - 1, Target, Properties...>::to(out, t);
        write_traits<Target>::object_end(out);
      }
    };

    template<typename Target, typename... Properties>
    void write_tuple (Target& out, const std::tuple<type<Properties>&...>& t) {
      write_tuple_t<Target, Properties...>::to(out, t);
    }

    /// write basic_struct
    template<typename Target, typename ... Properties>
    struct write_struct_t {
      static void to (Target& out, const basic_struct<Properties...>& t) {
        write_tuple(out, t.properites());
      }
    };

    /// write struct helper
    template<typename Target, typename ... Properties>
    void write_struct (Target& out, const basic_struct<Properties...>& t) {
      write_struct_t<Target, Properties...>::to(out, t);
    }

    /// detect named property
    template<typename Target, typename T>
    struct write_any_t<Target, type<T>> {
      static void to (Target& out, const type<T>& t) {
        write_property(out, t);
      }
    };

    /// detect vector
    template<typename Target, typename T>
    struct write_any_t<Target, std::vector<T>> {
      static void to (Target& out, const std::vector<T>& t) {
        write_vector(out, t);
      }
    };

    /// detect array
    template<typename Target, typename T, std::size_t S>
    struct write_any_t<Target, std::array<T, S>> {
      static void to (Target& out, const std::array<T, S>& t) {
        write_array(out, t);
      }
    };

    /// detect struct
    template<typename Target, typename T>
    struct write_any_t<Target, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void to (Target& out, const T& t) {
        write_struct(out, t);
      }
    };

    /// convenience helper
    template<typename Target, typename T>
    void inline write (Target& out, const T& t) {
      write_any(out, t);
    }

    // --------------------------------------------------------------------------
    //
    // read
    //
    // --------------------------------------------------------------------------

    /**
     * traits to allow source specific parsing.
     */
    template<typename Source>
    struct read_traits {
      static char list_start (Source&) { return ' '; }
      static bool list_element_init (Source&, char, bool first) { return true; }
      static char list_element_finish (Source&) { return true; }
      static void list_end (Source&, char) {}

      static char property_init (Source&, std::string&) { return ' '; }
      static void property_finish (Source&, const std::string&) {}

      static char object_delemiter (Source&) { return ' '; }
      static bool object_continue (Source&, char) { return true; }

      static char object_start (Source&) { return ' '; }
      static void object_end (Source&, char) {}
    };

    /// read value
    template<typename Source, typename T>
    struct read_value_t {
      static void from (Source& in, T& t);
    };

    template<typename Source, typename T>
    void read_value (Source& in, T& t) {
      read_value_t<Source, T>::from(in, t);
    }

    /**
    * read any type
    * type will be detected in later specializations
    */
    template<typename Source, typename T, typename Enable = void>
    struct read_any_t {
      static void from (Source& in, T& t) {
        read_value(in, t);
      }
    };

    template<typename Source, typename T>
    void read_any (Source& in, T& t) {
      read_any_t<Source, T>::from(in, t);
    }

    /// read property
    template<typename Source, typename T>
    struct read_property_t {
      static void from (Source& in, type<T>& t) {
        read_any(in, t());
      }
    };

    template<typename Source, typename T>
    void read_property (Source& in, type<T>& t) {
      read_property_t<Source, T>::from(in, t);
    }

    /// read vector
    template<typename Source, typename T>
    struct read_vector_t {
      static void from (Source& in, std::vector<T>& v) {
        auto delim = read_traits<Source>::list_start(in);
        v.clear();
        while (read_traits<Source>::list_element_init(in, delim, v.empty())) {
          T t;
          read_any(in, t);
          v.push_back(t);
          delim = read_traits<Source>::list_element_finish(in);
        }
        read_traits<Source>::list_end(in, delim);
      }
    };

    template<typename Source, typename T>
    void read_vector (Source& in, std::vector<T>& t) {
      read_vector_t<Source, T>::from(in, t);
    }

    /// read array
    template<typename Source, typename T, std::size_t S>
    struct read_array_t {
      static void from (Source& in, std::array<T, S>& a) {
        auto delim = read_traits<Source>::list_start(in);
        bool first = true;
        for (T& e : a) {
          read_traits<Source>::list_element_init(in, delim, first);
          first = false;
          read_any(in, e);
          delim = read_traits<Source>::list_element_finish(in);
        }
        read_traits<Source>::list_end(in, delim);
      }
    };

    template<typename Source, typename T, std::size_t S>
    void read_array (Source& in, std::array<T, S>& t) {
      read_array_t<Source, T, S>::from(in, t);
    }

    /// read element with name of a tuple
    template<std::size_t I, typename Source, typename ... Properties>
    struct read_named {
      static void property (Source& in, const std::string& name, std::tuple<type<Properties>&...>& t) {
        auto& f = std::get<I - 1>(t);
        if (name == f.name()) {
          read_any(in, f);
        } else {
          read_named<I - 1, Source, Properties...>::property(in, name, t);
        }
      }
    };

    /// Stop recoursion at element 0
    template<typename Source, typename ... Properties>
    struct read_named<0, Source, Properties...> {
      static void property (Source&, const std::string& name, std::tuple<type<Properties>& ...>&) {
        throw std::runtime_error(ostreamfmt("Could not find property with name '" << name << "'!"));
      }
    };

    /// read tuple
    template<typename Source, typename ... Properties>
    struct read_tuple_t {
      static void from (Source& in, std::tuple<type<Properties>&...>& t) {
        auto delim = read_traits<Source>::object_start(in);
        std::string name;
        while (read_traits<Source>::object_continue(in, delim)) {
          name.clear();
          read_traits<Source>::property_init(in, name);
          read_named<sizeof...(Properties), Source, Properties...>::property(in, name, t);
          read_traits<Source>::property_finish(in, name);
          delim = read_traits<Source>::object_delemiter(in);
        };
        read_traits<Source>::object_end(in, delim);
      }
    };

    template<typename Source, typename ... Properties>
    void read_tuple (Source& in, std::tuple<type<Properties>&...>& t) {
      read_tuple_t<Source, Properties...>::from(in, t);
    }

    /// read basic_struct
    template<typename Source, typename ... Properties>
    struct read_struct_t {
      static void from (Source& in, basic_struct<Properties...>& t) {
        read_tuple(in, t.properites());
      }
    };

    template<typename Source, typename ... Properties>
    void read_struct (Source& in, basic_struct<Properties...>& t) {
      read_struct_t<Source, Properties...>::from(in, t);
    }

    /// detect named property
    template<typename Source, typename T>
    struct read_any_t<Source, type<T>> {
      static void from (Source& in, type<T>& t) {
        read_property(in, t);
      }
    };

    /// detect vector
    template<typename Source, typename T>
    struct read_any_t<Source, std::vector<T>> {
      static void from (Source& in, std::vector<T>& t) {
        read_vector(in, t);
      }
    };

    /// detect array
    template<typename Source, typename T, std::size_t S>
    struct read_any_t<Source, std::array<T, S>> {
      static void from (Source& in, std::array<T, S>& t) {
        read_array(in, t);
      }
    };

    /// detect struct
    template<typename Source, typename T>
    struct read_any_t<Source, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void from (Source& in, T& t) {
        read_struct(in, t);
      }
    };

    /// convenience helper
    template<typename Source, typename T>
    void inline read (Source& in, T& t) {
      read_any(in, t);
    }

  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
