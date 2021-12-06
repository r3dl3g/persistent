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
* @brief     C++ struct persistence test
*
* @license   MIT license. See accompanying file LICENSE.
*/

#pragma once

#include "persistent/basic_struct.h"

using namespace persistent;

// --------------------------------------------------------------------------
struct test_int64 : public basic_struct<int64, int64> {
  typedef basic_struct<int64, int64> super;

  test_int64 ()
    : super(i, j)
    , i("i")
    , j("j")
  {}

  test_int64 (const test_int64& rhs)
    : test_int64 () {
    operator=(rhs);
  }

  int64 i;
  int64 j;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------
struct test2 : public basic_container {

  test2 ()
    : members(i1, t1, i2)
    , i1("i1")
    , t1("t1")
    , i2("i2")
  {}

  test2 (const test2& rhs)
    : test2() {
    members = rhs.members;
  }

  prop<int64_t> i1;
  prop<test_int64> t1;
  prop<int64_t> i2;

  typedef member_variables_t<decltype(i1), decltype(t1), decltype(i2)> member_variables;

    member_variables members;

};

// --------------------------------------------------------------------------
struct test3 : public basic_container {
private:
  prop<std::vector<test_int64>> m_v;

public:
  typedef member_variables_t<decltype(m_v)> member_variables;

  test3 ()
    : members(m_v)
    , m_v("v")
  {}

  test3 (const test3& rhs)
    : test3()
  {
    members=rhs.members;
  }

  member_variables& get_members () {
    return members;
  }

  const member_variables& get_members () const {
    return members;
  }

  const std::vector<test_int64>& v () const {
    return m_v();
  }

  std::vector<test_int64>& v () {
    return m_v();
  }

private:
  member_variables members;
};

// --------------------------------------------------------------------------
struct test4 : public basic_struct<int64, list<int>> {
  typedef basic_struct<int64, list<int>> super;

  test4 (int64_t i_ = {}, const std::vector<int>& l_ = {})
    : super(i, l)
    , i("i", i_)
    , l("i", l_)
  {}

  test4 (const test4& rhs)
    : test4 () {
    operator=(rhs);
  }

  int64 i;
  list<int> l;
};


namespace persistent {

  template<>
  auto get_members<test2> (test2& t) -> test2::member_variables& {
    return t.members;
  }

  template<>
  auto get_members<test2> (const test2& t) -> const test2::member_variables& {
    return t.members;
  }

}

// --------------------------------------------------------------------------

