/**
* @copyright (c) 2015-2018 Ing. Buero Rothfuss
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
// Library includes
//
#include <boost/property_tree/ptree.hpp>
#include <boost/optional/optional.hpp>
#include <util/vector_util.h>

#include <iterator>

// --------------------------------------------------------------------------
//
// Project includes
//
#include "persistent.h"


namespace persistent {

  namespace io {

    typedef boost::property_tree::ptree ptree;

    // --------------------------------------------------------------------------
    template<>
    struct write_traits<ptree> {
      static void list_delemiter (ptree& p) {}
      static void members_delemiter (ptree& p) {}
      static void list_start (ptree& p) {}
      static void list_end (ptree& p) {}
      static void object_key (ptree& p, const std::string& key) {}
      static void object_start (ptree& p) {}
      static void object_end (ptree& p) {}
    };
    /// write value
    template<typename T>
    struct write<ptree, T> {
      static void to (ptree& pt, const T& t) {
        pt.put_value(t);
      }
    };

    /// write property
    template<typename T>
    struct write<ptree, type<T>> {
      static void to (ptree& p, const type<T>& t) {
        //TODO: This will not work, it T is an struct
        p.put(t.name(), t());
      }
    };

    /// write vector and array values
    template<typename T, typename V>
    struct writeList<ptree, T, V> {
      static void to (ptree& p, const std::string& name, const V& v) {
        ptree list;
        for (const T& t : v) {
          ptree pt;
          write<ptree, const T>::to(pt, t);
          list.push_back(std::make_pair("", pt));
        }
        p.add_child(name, list);
      }
    };

    /// write vector values
    template<typename T>
    struct write<ptree, type<std::vector<T>>> {
      static void to (ptree& p, const type<std::vector<T>>& t) {
        writeList<ptree, T, std::vector<T>>::to(p, t.name(), t());
      }
    };

    /// write array values
    template<typename T, size_t S>
    struct write<ptree, type<std::array<T, S>>> {
      static void to (ptree& p, const type<std::array<T, S>>& a) {
        writeList<ptree, T, std::array<T, S>>::to(p, a.name(), a());
      }
    };

    // --------------------------------------------------------------------------
    //
    // read
    //
    // --------------------------------------------------------------------------

    template<>
    struct read_traits<ptree const> {
      static inline char list_start (ptree const&) {
        return ' ';
      }

      static inline char list_delemiter (ptree const&) {
        return ' ';
      }

      static inline bool list_continue (ptree const& p, char) {
        return !p.empty();
      }

      static inline void list_end (ptree const&, char) {}

      static inline char object_key (ptree const&, std::string&) {
        return ' ';
      }

      static inline char object_start (ptree const&) {
        return ' ';
      }

      static inline char object_delemiter (ptree const&) {
        return ' ';
      }

      static inline bool object_continue (ptree const& p, char) {
        return !p.empty();
      }

      static inline void object_end (ptree const&, char) {}
    };

    template<>
    struct read_traits<ptree> : public read_traits<ptree const> {};

    /// read value
    template<typename T>
    struct read_value<ptree, T> {
      static void from (ptree& p, T& t) {
        t = p.get_value<T>(T());
      }
    };

    /// read vector
    template<typename T>
    struct read_vector<ptree, T> {
      static void from (ptree& p, std::vector<T>& v) {
        int i = 0;
        v.resize(p.size());
        for (auto& item : p) {
          read<ptree, T>::from(item.second, v[i++]);
        }
      }
    };

    /// read array
    template<typename T, size_t S>
    struct read_array<ptree, T, S> {
      static void from (ptree& p, std::array<T, S>& a) {
        int i = 0;
        for (auto& item : p) {
          read<ptree, T>::from(item.second, a[i++]);
        }
      }
    };

    /// read tuple
    template<typename ... Properties>
    struct read_tuple<ptree, Properties...> {
      static void from (ptree& p, std::tuple<type<Properties>&...>& t) {
        for (auto& item : p) {
          read_named<sizeof...(Properties), ptree, Properties...>::property(item.second, item.first, t);
        }
      }
    };

  } // namespace io

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  struct ptree_struct : public basic_struct<Properties...> {
    typedef basic_struct<Properties...> super;

    ptree_struct (type<Properties>&... properties)
      : super(properties...)
    {}

    void write (io::ptree& pt) const {
      io::write<io::ptree, super>::to(pt, *this);
    }

    void read (io::ptree& pt) {
      io::read<io::ptree, super>::from(pt, *this);
    }

  };

} // namespace persistent

// --------------------------------------------------------------------------
