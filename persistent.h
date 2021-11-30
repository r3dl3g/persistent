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
      static void list_delemiter (Target&) {}
      static void members_delemiter (Target&) {}
      static void list_start (Target&) {}
      static void list_end (Target&) {}
      static void object_key (Target&, const std::string& key) {}
      static void object_start (Target&) {}
      static void object_end (Target&) {}
    };

    /**
    * write plain value
    * Allows specialized implementations.
    */
    template<typename Target, typename T>
    struct write_value_t {
      static void to (Target& p, const T& t);
    };

    template<typename Target, typename T>
    void write_value (Target& p, T& t) {
      write_value_t<Target, T>::to(p, t);
    }

    /**
    * write any type
    * type will be detected in later specializations
    */
    template<typename Target, typename T, typename Enable = void>
    struct write_any_t {
      static void to (Target& p, const T& t) {
        write_value(p, t);
      }
    };

    template<typename Target, typename T>
    void write_any (Target& p, const T& t) {
      write_any_t<Target, T>::to(p, t);
    }

    /**
     *  write named property
     */
    template<typename Target, typename T>
    struct write_property_t {
      static void to (Target& p, const type<T>& t) {
        write_traits<Target>::object_key(p, t.name());
        write_any(p, t());
      }
    };

    template<typename Target, typename T>
    void write_property (Target& p, const type<T>& t) {
      write_property_t<Target, T>::to(p, t);
    }

    /// write vector and array values
    template<typename Target, typename T, typename V>
    struct writeList_t {
      static void to (Target& p, const V& v) {
        write_traits<Target>::list_start(p);
        bool first = true;
        for (const T& t : v) {
          if (first) {
            first = false;
          } else {
            write_traits<Target>::list_delemiter(p);
          }
          write_any(p, t);
        }
        write_traits<Target>::list_end(p);
      }
    };

    /// write vector values
    template<typename Target, typename T>
    struct write_vector_t {
      static void to (Target& p, const std::vector<T>& t) {
        writeList_t<Target, T, std::vector<T>>::to(p, t);
      }
    };

    template<typename Target, typename T>
    void write_vector (Target& p, const std::vector<T>& t) {
      write_vector_t<Target, T>::to(p, t);
    }

    /// write array values
    template<typename Target, typename T, size_t S>
    struct write_array_t {
      static void to (Target& p, const std::array<T, S>& t) {
        writeList_t<Target, T, std::array<T, S>>::to(p, t());
      }
    };

    template<typename Target, typename T, size_t S>
    void write_array (Target& p, const std::array<T, S>& t) {
      write_array_t<Target, T, S>::to(p, t);
    }

    /**
    * write recursive all elements of a tuple
    */
    template<std::size_t I, typename Target, typename... Properties>
    struct write_nth {
      static void to (Target& p, std::tuple<type<Properties>&...> const& t) {
        write_nth<I - 1, Target, Properties...>::to(p, t);
        write_traits<Target>::members_delemiter(p);
        const auto& m = std::get<I>(t);
        write_any(p, m);
      }
    };

    /// end recursion at 0 elemnt.
    template<typename Target, typename... Properties>
    struct write_nth<0, Target, Properties...> {
      static void to (Target& p, std::tuple<type<Properties>&...> const& t) {
        const auto& m = std::get<0>(t);
        write_any(p, m);
      }
    };

    /// write tuple
    template<typename Target, typename ... Properties>
    struct write_tuple_t {
      static void to (Target& p, const std::tuple<type<Properties>&...>& t) {
        write_traits<Target>::object_start(p);
        write_nth<(sizeof...(Properties)) - 1, Target, Properties...>::to(p, t);
        write_traits<Target>::object_end(p);
      }
    };

    template<typename Target, typename... Properties>
    void write_tuple (Target& p, const std::tuple<type<Properties>&...>& t) {
      write_tuple_t<Target, Properties...>::to(p, t);
    }

    /// write basic_struct
    template<typename Target, typename ... Properties>
    struct write_struct_t {
      static void to (Target& p, const basic_struct<Properties...>& t) {
        write_tuple(p, t.properites());
      }
    };

    /// write struct helper
    template<typename Target, typename ... Properties>
    void write_struct (Target& p, const basic_struct<Properties...>& t) {
      write_struct_t<Target, Properties...>::to(p, t);
    }

    /// detect named property
    template<typename Target, typename T>
    struct write_any_t<Target, type<T>> {
      static void to (Target& p, const type<T>& t) {
        write_property(p, t);
      }
    };

    /// detect vector
    template<typename Target, typename T>
    struct write_any_t<Target, std::vector<T>> {
      static void to (Target& p, const std::vector<T>& t) {
        write_vector(p, t);
      }
    };

    /// detect array
    template<typename Target, typename T, std::size_t S>
    struct write_any_t<Target, std::array<T, S>> {
      static void to (Target& p, const std::array<T, S>& t) {
        write_array(p, t);
      }
    };

    /// detect struct
    template<typename Target, typename T>
    struct write_any_t<Target, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void to (Target& p, const T& t) {
        write_struct(p, t);
      }
    };

    /// convenience helper
    template<typename Target, typename T>
    void inline write (Target& p, const T& t) {
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
    template<typename Source>
    struct read_traits {
      static char list_start (Source&) { return ' '; }
      static char list_delemiter (Source&) { return ' '; }
      static bool list_continue (Source&, char) { return true; }
      static void list_end (Source&, char) {}
      static char object_key (Source&, std::string&) { return ' '; }
      static char object_start (Source&) { return ' '; }
      static char object_delemiter (Source&) { return ' '; }
      static bool object_continue (Source&, char) { return true; }
      static void object_end (Source&, char) {}
    };

    /// read value
    template<typename Source, typename T>
    struct read_value_t {
      static void from (Source& p, T& t);
    };

    template<typename Source, typename T>
    void read_value (Source& p, T& t) {
      read_value_t<Source, T>::from(p, t);
    }

    /**
    * read any type
    * type will be detected in later specializations
    */
    template<typename Source, typename T, typename Enable = void>
    struct read_any_t {
      static void from (Source& p, T& t) {
        read_value(p, t);
      }
    };

    template<typename Source, typename T>
    void read_any (Source& p, T& t) {
      read_any_t<Source, T>::from(p, t);
    }

    /// read property
    template<typename Source, typename T>
    struct read_property_t {
      static void from (Source& p, type<T>& t) {
        read_any(p, t());
      }
    };

    template<typename Source, typename T>
    void read_property (Source& p, type<T>& t) {
      read_property_t<Source, T>::from(p, t);
    }

    /// read vector
    template<typename Source, typename T>
    struct read_vector_t {
      static void from (Source& p, std::vector<T>& v) {
        auto delim = read_traits<Source>::list_start(p);
        while (read_traits<Source>::list_continue(p, delim)) {
          T t;
          read_any(p, t);
          v.push_back(t);
          delim = read_traits<Source>::list_delemiter(p);
        }
        read_traits<Source>::list_end(p, delim);
      }
    };

    template<typename Source, typename T>
    void read_vector (Source& p, std::vector<T>& t) {
      read_vector_t<Source, T>::from(p, t);
    }

    /// read array
    template<typename Source, typename T, std::size_t S>
    struct read_array_t {
      static void from (Source& p, std::array<T, S>& a) {
        auto delim = read_traits<Source>::list_start(p);
        for (T& e : a) {
          read_any(p, e);
          delim = read_traits<Source>::list_delemiter(p);
        }
        read_traits<Source>::list_end(p, delim);
      }
    };

    template<typename Source, typename T, std::size_t S>
    void read_array (Source& p, std::array<T, S>& t) {
      read_array_t<Source, T, S>::from(p, t);
    }

    /// read element with name of a tuple
    template<std::size_t I, typename Source, typename ... Properties>
    struct read_named {
      static void property (Source& p, const std::string& name, std::tuple<type<Properties>&...>& t) {
        auto& f = std::get<I - 1>(t);
        if (name == f.name()) {
          read_any(p, f);
        } else {
          read_named<I - 1, Source, Properties...>::property(p, name, t);
        }
      }
    };

    /// Stop recoursion at element 0
    template<typename Source, typename ... Properties>
    struct read_named<0, Source, Properties...> {
      static void property (Source& p, const std::string& name, std::tuple<type<Properties>& ...>&) {
        throw std::runtime_error(ostreamfmt("Could not find property with name '" << name << "'!"));
      }
    };

    /// read tuple
    template<typename Source, typename ... Properties>
    struct read_tuple_t {
      static void from (Source& p, std::tuple<type<Properties>&...>& t) {
        auto delim = read_traits<Source>::object_start(p);
        while (read_traits<Source>::object_continue(p, delim)) {
          std::string name;
          read_traits<Source>::object_key(p, name);
          read_named<sizeof...(Properties), Source, Properties...>::property(p, name, t);
          delim = read_traits<Source>::object_delemiter(p);
        };
        read_traits<Source>::object_end(p, delim);
      }
    };

    template<typename Source, typename ... Properties>
    void read_tuple (Source& p, std::tuple<type<Properties>&...>& t) {
      read_tuple_t<Source, Properties...>::from(p, t);
    }

    /// read basic_struct
    template<typename Source, typename ... Properties>
    struct read_struct_t {
      static void from (Source& p, basic_struct<Properties...>& t) {
        read_tuple(p, t.properites());
      }
    };

    template<typename Source, typename ... Properties>
    void read_struct (Source& p, basic_struct<Properties...>& t) {
      read_struct_t<Source, Properties...>::from(p, t);
    }

    /// detect named property
    template<typename Source, typename T>
    struct read_any_t<Source, type<T>> {
      static void from (Source& p, type<T>& t) {
        read_property(p, t);
      }
    };

    /// detect vector
    template<typename Source, typename T>
    struct read_any_t<Source, std::vector<T>> {
      static void from (Source& p, std::vector<T>& t) {
        read_vector(p, t);
      }
    };

    /// detect array
    template<typename Source, typename T, std::size_t S>
    struct read_any_t<Source, std::array<T, S>> {
      static void from (Source& p, std::array<T, S>& t) {
        read_array(p, t);
      }
    };

    /// detect struct
    template<typename Source, typename T>
    struct read_any_t<Source, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static void from (Source& p, T& t) {
        read_struct(p, t);
      }
    };

    /// convenience helper
    template<typename Source, typename T>
    void inline read (Source& p, T& t) {
      read_any(p, t);
    }

  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
