/**
* @copyright (c) 2015-2021 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    persistent lib
*
* @brief     C++ struct persistence
*
* @license   MIT license. See accompanying file LICENSE.
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <map>
#include <sstream>

// --------------------------------------------------------------------------
//
// Project includes
//
#include "basic_struct.h"


namespace persistent {

  namespace io {

    struct msg_fmt {
      template <typename T>
      inline msg_fmt& operator<< (T const& value) {
        buffer << value;
        return *this;
      }

      inline operator std::string () const {
        return buffer.str();
      }

    private:
      std::ostringstream buffer;
    };

    // --------------------------------------------------------------------------
    //
    // write
    //
    // --------------------------------------------------------------------------

    /**
     * traits to allow target specific prefix and suffixes.
     */
    template<typename Target>
    struct formatter {
      static void write_list_start (Target&) {}
      static void write_list_element_init (Target&, int num) {}
      static void write_list_element_finish (Target&) {}
      static void write_list_end (Target&) {}

      static void write_members_delemiter (Target&) {}

      static void write_property_init (Target&, const std::string&) {}
      static void write_property_finish (Target&, const std::string&) {}

      static void write_struct_start (Target&) {}
      static void write_struct_end (Target&) {}
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
      static void to (Target& out, const prop<T>& t) {
        formatter<Target>::write_property_init(out, t.name());
        write_any(out, t());
        formatter<Target>::write_property_finish(out, t.name());
      }
    };

    template<typename Target, typename T>
    void write_property (Target& out, const prop<T>& t) {
      write_property_t<Target, T>::to(out, t);
    }

    /// write vector and array values
    template<typename Target, typename T, typename V>
    struct writeList_t {
      static void to (Target& out, const V& v) {
        formatter<Target>::write_list_start(out);
        int num = 0;
        for (const T& t : v) {
          formatter<Target>::write_list_element_init(out, num++);
          write_any(out, t);
          formatter<Target>::write_list_element_finish(out);
        }
        formatter<Target>::write_list_end(out);
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
        writeList_t<Target, T, std::array<T, S>>::to(out, t);
      }
    };

    template<typename Target, typename T, size_t S>
    void write_array (Target& out, const std::array<T, S>& t) {
      write_array_t<Target, T, S>::to(out, t);
    }

    /**
    * write recursive all elements of a tuple
    */
    template<std::size_t I, typename Target, typename... Types>
    struct write_nth {
      static void to (Target& out, std::tuple<prop<Types>&...> const& t) {
        write_nth<I - 1, Target, Types...>::to(out, t);
        formatter<Target>::write_members_delemiter(out);
        const auto& m = std::get<I>(t);
        write_any(out, m);
      }
    };

    /// end recursion at 0 elemnt.
    template<typename Target, typename... Types>
    struct write_nth<0, Target, Types...> {
      static void to (Target& out, std::tuple<prop<Types>&...> const& t) {
        const auto& m = std::get<0>(t);
        write_any(out, m);
      }
    };

    /// write tuple
    template<typename Target, typename ... Types>
    struct write_tuple_t {
      static void to (Target& out, const std::tuple<prop<Types>&...>& t) {
        formatter<Target>::write_struct_start(out);
        write_nth<(sizeof...(Types)) - 1, Target, Types...>::to(out, t);
        formatter<Target>::write_struct_end(out);
      }
    };

    template<typename Target, typename... Types>
    void write_tuple (Target& out, const std::tuple<prop<Types>&...>& t) {
      write_tuple_t<Target, Types...>::to(out, t);
    }

    /// write basic_struct
    template<typename Target, typename ... Types>
    struct write_struct_t {
      static void to (Target& out, const basic_struct<Types...>& t) {
        write_tuple(out, persistent::get_members(t));
      }
    };

    /// write struct helper
    template<typename Target, typename ... Types>
    void write_struct (Target& out, const basic_struct<Types...>& t) {
      write_struct_t<Target, Types...>::to(out, t);
    }

    /// write shared values
    template<typename Target, typename T>
    struct write_shared_t {
      static void to (Target& out, const std::shared_ptr<T>& t) {
        write_any_t<Target, T>::to(out, *t.get());
      }
    };

    template<typename Target, typename T>
    void write_shared (Target& out, const std::shared_ptr<T>& t) {
      write_shared_t<Target, T>::to(out, t);
    }

    /// detect named property
    template<typename Target, typename T>
    struct write_any_t<Target, prop<T>> {
      static void to (Target& out, const prop<T>& t) {
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
        write_tuple(out, persistent::get_members(t));
      }
    };

    /// detect shared
    template<typename Target, typename T>
    struct write_any_t<Target, std::shared_ptr<T>> {
      static void to (Target& out, const std::shared_ptr<T>& t) {
        write_shared(out, t);
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
    struct parser {
      static bool read_list_start (Source&) { return false; }
      static bool read_list_element_init (Source&, int) { return true; }
      static void read_list_element_finish (Source&) {}
      static void read_list_end (Source&) {}

      static void read_property_init (Source&, std::string&) {}
      static void read_property_finish (Source&, const std::string&) {}

      static bool read_next_struct_element (Source&, std::string&) { return true; }
      static void read_struct_element_finish (Source&, const std::string&) {}
    };

    /// read value
    template<typename Source, typename T>
    struct read_value_t {
      static bool from (Source& in, T& t);
    };

    template<typename Source, typename T>
    inline bool read_value (Source& in, T& t) {
      return read_value_t<Source, T>::from(in, t);
    }

    /**
    * read any type
    * type will be detected in later specializations
    */
    template<typename Source, typename T, typename Enable = void>
    struct read_any_t {
      static inline bool from (Source& in, T& t) {
        return read_value(in, t);
      }
    };

    template<typename Source, typename T>
    inline bool read_any (Source& in, T& t) {
      return read_any_t<Source, T>::from(in, t);
    }

    /// read property
    template<typename Source, typename T>
    struct read_property_t {
      static bool from (Source& in, prop<T>& t) {
        std::string name;
        try {
          parser<Source>::read_property_init(in, name);
          const bool found = read_any(in, t());
          parser<Source>::read_property_finish(in, name);
          return found;
        } catch (std::exception& ex) {
          throw std::runtime_error(msg_fmt() << ex.what() << " for property '" << name << "'");
        }
      }
    };

    template<typename Source, typename T>
    inline bool read_property (Source& in, prop<T>& t) {
      return read_property_t<Source, T>::from(in, t);
    }

    /// read vector
    template<typename Source, typename T>
    struct read_vector_t {
      static bool from (Source& in, std::vector<T>& v) {
        if (!parser<Source>::read_list_start(in)) {
          return false;
        }
        v.clear();
        int num = 0;
        bool found = false;
        while (parser<Source>::read_list_element_init(in, num++)) {
          T t;
          found |= read_any(in, t);
          v.push_back(t);
          parser<Source>::read_list_element_finish(in);
        }
        parser<Source>::read_list_end(in);
        return found;
      }
    };

    template<typename Source, typename T>
    inline bool read_vector (Source& in, std::vector<T>& t) {
      return read_vector_t<Source, T>::from(in, t);
    }

    /// read array
    template<typename Source, typename T, std::size_t S>
    struct read_array_t {
      static bool from (Source& in, std::array<T, S>& a) {
        if (!parser<Source>::read_list_start(in)) {
          return false;
        }
        int num = 0;
        bool found = false;
        for (T& e : a) {
          parser<Source>::read_list_element_init(in, num++);
          found |= read_any(in, e);
          parser<Source>::read_list_element_finish(in);
        }
        parser<Source>::read_list_end(in);
        return found;
      }
    };

    template<typename Source, typename T, std::size_t S>
    inline bool read_array (Source& in, std::array<T, S>& t) {
      return read_array_t<Source, T, S>::from(in, t);
    }

    /// read element with name of a tuple
    template<std::size_t I, typename Source, typename ... Types>
    struct read_named {
      static bool property (Source& in, const std::string& name, std::tuple<prop<Types>&...>& t) {
        bool found = read_named<I - 1, Source, Types...>::property(in, name, t);
        if (!found) {
          auto& f = std::get<I - 1>(t);
          if (name == f.name()) {
            found |= read_any(in, f());
          }
        }
        return found;
      }
    };

    /// Stop recoursion at element 0
    template<typename Source, typename ... Types>
    struct read_named<0, Source, Types...> {
      static inline bool property (Source&, const std::string& name, std::tuple<prop<Types>& ...>&) {
        return false;
      }
    };

    /// read tuple
    template<typename Source, typename ... Types>
    struct read_tuple_t {
      static bool from (Source& in, std::tuple<prop<Types>&...>& t) {
        std::string name;
        bool found = false;
        while (parser<Source>::read_next_struct_element(in, name)) {
          found |= read_named<sizeof...(Types), Source, Types...>::property(in, name, t);
          parser<Source>::read_struct_element_finish(in, name);
          name.clear();
        }
        return found;
      }
    };

    template<typename Source, typename ... Types>
    inline bool read_tuple (Source& in, std::tuple<prop<Types>&...>& t) {
      return read_tuple_t<Source, Types...>::from(in, t);
    }

    /// read basic_struct
    template<typename Source, typename ... Types>
    struct read_struct_t {
      static inline bool from (Source& in, basic_struct<Types...>& t) {
        return read_tuple(in, persistent::get_members(t));
      }
    };

    template<typename Source, typename ... Types>
    inline bool read_struct (Source& in, basic_struct<Types...>& t) {
      return read_struct_t<Source, Types...>::from(in, t);
    }

    /// read shared
    template<typename Source, typename T>
    struct read_shared_t {
      static bool from (Source& in, std::shared_ptr<T>& v) {
        if (!v) {
          v = std::make_shared<T>();
        }
        T& t = *v;
        return read_any(in, t);
      }
    };

    template<typename Source, typename T>
    inline bool read_shared (Source& in, std::shared_ptr<T>& t) {
      return read_shared_t<Source, T>::from(in, t);
    }

    /// detect named property
    template<typename Source, typename T>
    struct read_any_t<Source, prop<T>> {
      static inline bool from (Source& in, prop<T>& t) {
        return read_property(in, t);
      }
    };

    /// detect vector
    template<typename Source, typename T>
    struct read_any_t<Source, std::vector<T>> {
      static inline bool from (Source& in, std::vector<T>& t) {
        return read_vector(in, t);
      }
    };

    /// detect array
    template<typename Source, typename T, std::size_t S>
    struct read_any_t<Source, std::array<T, S>> {
      static inline bool from (Source& in, std::array<T, S>& t) {
        return read_array(in, t);
      }
    };

    /// detect struct
    template<typename Source, typename T>
    struct read_any_t<Source, T, typename std::enable_if<std::is_base_of<basic_container, T>::value>::type> {
      static inline bool from (Source& in, T& t) {
        return read_tuple(in, persistent::get_members(t));
      }
    };

    /// detect shared
    template<typename Source, typename T>
    struct read_any_t<Source, std::shared_ptr<T>> {
      static inline bool from (Source& in, std::shared_ptr<T>& t) {
        return read_shared(in, t);
      }
    };

    /// convenience helper
    template<typename Source, typename T>
    inline bool read (Source& in, T& t) {
      return read_any(in, t);
    }

  } // namespace io

} // namespace persistent

// --------------------------------------------------------------------------
