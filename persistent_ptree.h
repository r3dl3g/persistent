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
// Library includes
//
#include <boost/property_tree/ptree.hpp>

// --------------------------------------------------------------------------
//
// Project includes
//
#include "persistent_io.h"


namespace persistent {

  namespace io {

    typedef boost::property_tree::ptree ptree;

    // --------------------------------------------------------------------------
    //
    // write
    //
    // --------------------------------------------------------------------------

    /// write value
    template<typename T>
    struct write_value_t<ptree, T> {
      static void to (ptree& pt, const T& t) {
        pt.put_value(t);
      }
    };

    /// write property
    template<typename T>
    struct write_property_t<ptree, T> {
      static void to (ptree& p, const prop<T>& t) {
        ptree child;
        write_any(child, t());
        p.add_child(t.name(), child);
      }
    };

    /// write vector and array values
    template<typename T, typename V>
    struct writeList_t<ptree, T, V> {
      static void to (ptree& p, const V& v) {
        for (const T& t : v) {
          ptree pt;
          write_any(pt, t);
          p.push_back(std::make_pair("", pt));
        }
      }
    };

    // --------------------------------------------------------------------------
    //
    // read
    //
    // --------------------------------------------------------------------------

    /// read value
    template<typename T>
    struct read_value_t<ptree, T> {
      static bool from (ptree& p, T& t) {
        t = p.get_value<T>(T());
        return true;
      }
    };

    /// read vector
    template<typename T>
    struct read_vector_t<ptree, T> {
      static bool from (ptree& p, std::vector<T>& v) {
        int i = 0;
        v.resize(p.size());
        for (auto& item : p) {
          read_any(item.second, v[i++]);
        }
        return true;
      }
    };

    /// read array
    template<typename T, size_t S>
    struct read_array_t<ptree, T, S> {
      static bool from (ptree& p, std::array<T, S>& a) {
        int i = 0;
        for (auto& item : p) {
          read_any(item.second, a[i++]);
        }
        return true;
      }
    };

    /// read tuple
    template<typename ... Types>
    struct read_tuple_t<ptree, Types...> {
      static bool from (ptree& p, std::tuple<prop<Types>&...>& t) {
        bool found = true;
        for (auto& item : p) {
          found |= read_named<sizeof...(Types), ptree, Types...>::property(item.second, item.first, t);
        }
        return found;
      }
    };

  } // namespace io

  // --------------------------------------------------------------------------
  template<typename ... Types>
  struct ptree_struct : public basic_struct<Types...> {
    typedef basic_struct<Types...> super;

    ptree_struct (prop<Types>&... properties)
      : super(properties...)
    {}

    void write (io::ptree& pt) const {
      io::write(pt, *this);
    }

    bool read (io::ptree& pt) {
      return io::read(pt, *this);
    }

  };

} // namespace persistent

// --------------------------------------------------------------------------
