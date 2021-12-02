
#pragma once

#include "persistent/basic_struct.h"

// --------------------------------------------------------------------------
struct test_int64 : public persistent::basic_struct<int64_t, int64_t> {
  typedef persistent::basic_struct<int64_t, int64_t> super;
  test_int64 ()
    : super(i, j)
    , i("i")
    , j("j")
  {}

  test_int64 (const test_int64& rhs)
    : test_int64()
  {
    super::operator=(rhs);
  }

  persistent::int64 i;
  persistent::int64 j;
};

// --------------------------------------------------------------------------
struct test2 : public persistent::basic_struct<int64_t, test_int64, int64_t> {
  typedef persistent::basic_struct<int64_t, test_int64, int64_t> super;

  test2 ()
    : super(i1, t1, i2)
    , i1("i1")
    , t1("t1")
    , i2("i2")
  {}

  test2 (const test2& rhs)
    : test2()
  {
    super::operator=(rhs);
  }

  persistent::int64 i1;
  persistent::type<test_int64> t1;
  persistent::int64 i2;
};

// --------------------------------------------------------------------------
struct test3 : public persistent::basic_struct<std::vector<test_int64>> {
  typedef persistent::basic_struct<std::vector<test_int64>> super;

  test3 ()
    : super(v)
    , v("v")
  {}

  test3 (const test3& rhs)
    : test3()
  {
    super::operator=(rhs);
  }

  persistent::vector<test_int64> v;
};

// --------------------------------------------------------------------------

