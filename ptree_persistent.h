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

// --------------------------------------------------------------------------
//
// Project includes
//
#include "persistent.h"
#include "vector_util.h"


namespace persistent {

  namespace io {

    typedef boost::property_tree::ptree ptree;

    /// read simple type
    template<typename T>
    void read (const ptree& pt, typename std::enable_if<!std::is_base_of<basic_container, T>::value, T>::type& t) {
      t = pt.get_value<T>(T());
    }

    /// write simple type
    template<typename T>
    void write (ptree& pt, const typename std::enable_if<!std::is_base_of<basic_container, T>::value, T>::type& t) {
      pt.put_value(t);
    }

    /// read struct
    template<typename T>
    void read (const ptree& pt, typename std::enable_if<std::is_base_of<basic_container, T>::value, T>::type& t) {
      t.read(pt);
    }

    /// write struct
    template<typename T>
    void write (ptree& pt, const typename std::enable_if<std::is_base_of<basic_container, T>::value, T>::type& t) {
      t.write(pt);
    }

    /// read vector
    template<typename T>
    void read (const std::pair<ptree::const_assoc_iterator, ptree::const_assoc_iterator>& r, std::vector<T>& v) {
      v.resize(std::distance(r.first, r.second));
      int idx = 0;
      for (auto i = r.first, e = r.second; i != e; ++i) {
        read<T>(i->second, v[idx++]);
      }
    }

    /// read array
    template<typename T, size_t S>
    void read (const std::pair<ptree::const_assoc_iterator, ptree::const_assoc_iterator>& r, std::array<T, S>& a) {
      int idx = 0;
      for (auto i = r.first, e = r.second; i != e; ++i) {
        read<T>(i->second, a[idx++]);
      }
    }

    template<typename T>
    struct ptree_io {

      static void get (const ptree& pt, const char* name, T& t) {
        const auto& opt = pt.get_child_optional(name);
        if (opt) {
          read<T>(opt.get(), t);
        }
      }

      static void put (ptree& pt, const char* name, const T& t) {
        write<T>(pt.put_child(name, ptree()), t);
      }

      static void add (ptree& pt, const char* name, const T& t) {
        write<T>(pt.add_child(name, ptree()), t);
      }

    };

    template<typename T>
    struct ptree_io<std::vector<T>> {

      static void get (const ptree& pt, const char* name, std::vector<T>& v) {
        read<T>(pt.equal_range(name), v);
      }

      static void put (ptree& pt, const char* name, const std::vector<T>& v) {
        for (const T& e : v) {
          ptree_io<T>::add(pt, name, e);
        }
      }

      static void add (ptree& pt, const char* name, const std::vector<T>& v) {
        ptree_io<T>::put(pt, name, v);
      }

    };

    template<typename T, size_t S>
    struct ptree_io<std::array<T, S>> {

      static void get (const ptree& pt, const char* name, std::array<T, S>& a) {
        read<T, S>(pt.equal_range(name), a);
      }

      static void put (ptree& pt, const char* name, const std::array<T, S>& v) {
        for (const T& e : v) {
          ptree_io<T>::add(pt, name, e);
        }
      }

      static void add (ptree& pt, const char* name, const std::array<T, S>& v) {
        ptree_io<T>::put(pt, name, v);
      }

    };

    // --------------------------------------------------------------------------
    // write named type to ptree
    template<std::size_t I, typename T>
    struct out<ptree, I, type<T>> {
      static void write (ptree& pt, const type<T>& t) {
        ptree_io<T>::put(pt, t.name(), t());
      }
    };

    template<std::size_t I, typename T>
    struct out<ptree, I, const type<T>> {
      static void write (ptree& pt, const type<T>& t) {
        ptree_io<T>::put(pt, t.name(), t());
      }
    };

    // --------------------------------------------------------------------------
    // read named type from ptree
    template<std::size_t I, typename T>
    struct in<const ptree, I, type<T>> {
      static void read (const ptree& pt, type<T>& t) {
        ptree_io<T>::get(pt, t.name(), type<T>::accessor::get(t));
      }
    };

    template<std::size_t I, typename T>
    struct in<const ptree, I, const type<T>> {
      static void read (const ptree& pt, const type<T>& t) {
        ptree_io<T>::get(pt, t.name(), type<T>::accessor::get(t));
      }
    };

  }

  // --------------------------------------------------------------------------
  template<typename ... Properties>
  struct ptree_struct : public basic_struct<Properties...> {
    typedef basic_struct<Properties...> super;

    ptree_struct (Properties&... properties)
      : super(properties...)
    {}

    void write (io::ptree& pt) const {
      io::out<io::ptree, 0, super>::write(pt, *this);
    }

    void read (const io::ptree& pt) {
      io::in<const io::ptree, 0, super>::read(pt, *this);
    }

  };

} // namespace persistent

// --------------------------------------------------------------------------
