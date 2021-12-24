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

#include "persistent/persistent_json.h"
#include "persistent/persistent_xml.h"
#include <testing/testing.h>

using namespace persistent;

struct MyStruct : private persistent_struct {  // persistent structs must be subclasses of a persistent_struct

  MyStruct (double d_ = {}, int64_t i_ = {}, const std::string& s_ = {}, const std::array<int, 5>& a_ = {}, const std::vector<std::string>& v_ = {})
    : d(d_)                       // A standard initializer list
    , i(i_)
    , s(s_)
    , a(a_)
    , v(v_)
  {}

  double d;                       // simple primitiv members
  int i;
  std::string s;
  std::array<int, 5> a;
  std::vector<std::string> v;

  auto attributes () {            // Each persistent structs must provide an attributes method that returns a tuple with attributes of its members.
    return std::make_tuple(attribute(d, "d"), attribute(i, "i"), attribute(s, "s"), attribute(a, "a"), attribute(v, "v"));
  }

  const auto attributes () const {// For convenience we use a const cast. Optional we can use the identical code as in the non const method.
    return (const_cast<MyStruct*>(this))->attributes();
  }

};

// --------------------------------------------------------------------------
void test_write () {

  MyStruct s{1.234, 4711, "Some text", {1, 2, 3, 4, 5}, {"One", "Two", "Three"}};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}");

  os.str({});
  persistent::io::write_json(os, s, true);
  testing::log_info(os.str());

  os.str({});
  persistent::io::write_xml(os, s, true);
  testing::log_info(os.str());
}

// --------------------------------------------------------------------------
void test_read () {

  MyStruct s;

  std::istringstream is("{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.d, 1.234);
  EXPECT_EQUAL(s.i, 4711);
  EXPECT_EQUAL(s.s, "Some text");
  std::array<int, 5> expected_a{1, 2, 3, 4, 5};
  EXPECT_EQUAL(s.a, expected_a);
  std::vector<std::string> expected_v{"One", "Two", "Three"};
  EXPECT_EQUAL(s.v, expected_v);
}

// --------------------------------------------------------------------------
struct MyStruct2 : private persistent_struct {   // make your struct a subclass of persistent_struct.

  MyStruct2 (const MyStruct s_ = {},
             const std::vector<int> v1_ = {},
             const std::vector<MyStruct> v2_ = {},
             const std::shared_ptr<MyStruct> p_ = {})
    : s(s_)
    , v1(v1_)
    , v2(v2_)
    , p(p_)
  {}

  MyStruct s;                                    // A complex member
  std::vector<int> v1;                           // A vector of primitiv types
  std::vector<MyStruct> v2;                      // A vector of complex types
  std::shared_ptr<MyStruct> p;                   // A shared pointer

  auto attributes () {                           // give your properties a name
    return std::make_tuple(attribute(s, "s"), attribute(v1, "v1"), attribute(v2, "v2"), attribute(p, "p"));
  }

  const auto attributes () const {
    return (const_cast<MyStruct2*>(this))->attributes();
  }

};

// --------------------------------------------------------------------------
void test_write2 () {

  MyStruct2 s{{1.234, 4711, "Some text", {1, 2, 3, 4, 5}, {"One", "Two", "Three"}},
              {3, 4, 5},
              {{1.1, 2, "A", {1, 2, 3}, {"A", "B", "C"}}}};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"s\":{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}"
                         ",\"v1\":[\"3\",\"4\",\"5\"]"
                         ",\"v2\":[{\"d\":\"1.1\",\"i\":\"2\",\"s\":\"A\",\"a\":[\"1\",\"2\",\"3\",\"0\",\"0\"]"
                         ",\"v\":[\"A\",\"B\",\"C\"]}]"
                         ",\"p\":null}");
}

// --------------------------------------------------------------------------
void test_read2 () {

  MyStruct2 s;

  std::istringstream is("{\"s\":{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}"
                        ",\"v1\":[\"3\",\"4\",\"5\"]"
                        ",\"v2\":[{\"d\":\"1.1\",\"i\":\"2\",\"s\":\"A\",\"a\":[\"1\",\"2\",\"3\",\"0\",\"0\"]"
                        ",\"v\":[\"A\",\"B\",\"C\"]}]"
                        ",\"p\":{}}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.s.d, 1.234);
  EXPECT_EQUAL(s.s.i, 4711);
  EXPECT_EQUAL(s.s.s, "Some text");
  std::array<int, 5> expected_a{1, 2, 3, 4, 5};
  EXPECT_EQUAL(s.s.a, expected_a);
  std::vector<std::string> expected_v{"One", "Two", "Three"};
  EXPECT_EQUAL(s.s.v, expected_v);

  std::vector<int> expected_v1{3, 4, 5};
  EXPECT_EQUAL(s.v1, expected_v1);

  EXPECT_EQUAL(s.v2.size(), 1);
  EXPECT_EQUAL(s.v2[0].d, 1.1);
  EXPECT_EQUAL(s.v2[0].i, 2);
  EXPECT_EQUAL(s.v2[0].s, "A");
  std::array<int, 5> expected_a2{1, 2, 3, 0, 0};
  EXPECT_EQUAL(s.v2[0].a, expected_a2);
  std::vector<std::string> expected_v2{"A", "B", "C"};
  EXPECT_EQUAL(s.v2[0].v, expected_v2);

  EXPECT_TRUE((bool)s.p);
  EXPECT_EQUAL(s.p->d, 0);
  EXPECT_EQUAL(s.p->i, 0);
  EXPECT_TRUE(s.p->s.empty());
  std::array<int, 5> expected_a3{0, 0, 0, 0, 0};
  EXPECT_EQUAL(s.p->a, expected_a3);
  EXPECT_TRUE(s.p->v.empty());
}

// --------------------------------------------------------------------------
void test_read3 () {

  MyStruct2 s;

  std::istringstream is("{\"s\":{}"
                        ",\"v1\":[]"
                        ",\"v2\":[]"
                        ",\"p\":null}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.s.d, 0);
  EXPECT_EQUAL(s.s.i, 0);
  EXPECT_TRUE(s.s.s.empty());
  std::array<int, 5> expected_a{0, 0, 0, 0, 0};
  EXPECT_EQUAL(s.s.a, expected_a);
  EXPECT_TRUE(s.s.v.empty());
  EXPECT_TRUE(s.v1.empty());
  EXPECT_TRUE(s.v2.empty());
  EXPECT_FALSE((bool)s.p);

}

// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);
  run_test(test_write);
  run_test(test_read);

  run_test(test_write2);
  run_test(test_read2);
  run_test(test_read3);
}

// --------------------------------------------------------------------------

