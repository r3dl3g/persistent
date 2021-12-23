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

#include "persistent/persistent_ios.h"
#include <testing/testing.h>

#include "test_structs.h"

// --------------------------------------------------------------------------
void test_read_empty () {

  test_int64 t1;
  std::istringstream is("{}");
  persistent::io::read_stream(is, t1);

  EXPECT_EQUAL(t1.i(), 0);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_TRUE(is.good());
  is.get();
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}
// --------------------------------------------------------------------------
template<typename T>
constexpr std::pair<char const*, T> get_ios_test_data () {
  return get_test_data<T>();
}

template<> inline std::pair<char const*, std::string>  get_ios_test_data<std::string> ()  {
  return { "\"Some text\"", "Some text" };
}

// --------------------------------------------------------------------------
template<typename T>
void test_read_prop_type () {
  const auto expected = get_ios_test_data<T>();
  const std::string str = persistent::io::msg_fmt() << "i:" << expected.first;

  persistent::prop::type<T> i("i");
  std::istringstream is(str);
  persistent::io::read_stream(is, i);

  EXPECT_EQUAL(i(), expected.second, " for type ", typeid(T).name(), " with source ", str);
}
// --------------------------------------------------------------------------
template<typename T>
void test_read_prop_t_type () {
  const auto expected = get_ios_test_data<T>();
  const std::string str = persistent::io::msg_fmt() << "i:" << expected.first;

  static constexpr char i_n[] = "i";
  persistent::prop_t::type<T, i_n> i;
  std::istringstream is(str);
  persistent::io::read_stream(is, i);

  EXPECT_EQUAL(i(), expected.second, " for type ", typeid(T).name(), " with source ", str);
}
// --------------------------------------------------------------------------
template<typename T, typename... Types>
struct Test {
  static void test () {
    test_read_prop_type<T>();
    test_read_prop_t_type<T>();
    Test<Types...>::test();
  }
};

template<typename T>
struct Test<T> {
  static void test () {
    test_read_prop_type<T>();
    test_read_prop_t_type<T>();
  }
};
// --------------------------------------------------------------------------
void test_read_all_basic_types () {
  Test<bool, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double,
       std::string, char, short, int, long, unsigned char, unsigned short, unsigned int, unsigned long>::test();
}

// --------------------------------------------------------------------------
void test_read_array () {

  persistent::prop::fix_list<int64_t, 5> a("a");
  std::istringstream is("a:[1,2,3,4,5]");
  persistent::io::read_stream(is, a);

  std::array<int64_t, 5> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(a(), expected);
}
// --------------------------------------------------------------------------
void test_read_vector () {

  persistent::prop::list<int64_t> v("v");
  std::istringstream is("v:[1,2,3,4,5]");
  persistent::io::read_stream(is, v);

  std::vector<int64_t> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(v(), expected);
}

// --------------------------------------------------------------------------
void test_read_prop_t_array () {

  static char constexpr n_a[] = "a";
  persistent::prop_t::fix_list<int64_t, 5, n_a> a;
  std::istringstream is("a:[1,2,3,4,5]");
  persistent::io::read_stream(is, a);

  std::array<int64_t, 5> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(a(), expected);
}
// --------------------------------------------------------------------------
void test_read_prop_t_vector () {

  static char constexpr n_v[] = "v";
  persistent::prop_t::list<int64_t, n_v> v;
  std::istringstream is("v:[1,2,3,4,5]");
  persistent::io::read_stream(is, v);

  std::vector<int64_t> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(v(), expected);
}

// --------------------------------------------------------------------------
void test_read_1 () {

  test_int64 t1;
  std::istringstream is("{i:4711}");
  persistent::io::read_stream(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_2 () {

  test_int64 t1;
  std::istringstream is("{i:4711,j:815}");
  persistent::io::read_stream(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 815);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_3 () {

  persistent::prop::int64 i("i");
  std::istringstream is("i:4711");
  persistent::io::read_stream(is, i);

  EXPECT_EQUAL(i(), 4711);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_prop_t_3 () {

  static char constexpr n_i[] = "i";
  persistent::prop_t::int64<n_i> i;
  std::istringstream is("i:4711");
  persistent::io::read_stream(is, i);

  EXPECT_EQUAL(i(), 4711);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_4 () {

  test_int64 t1;
  std::istringstream is(" \n \t { \n \t i \n \t : \n \t 4711 \n \t } \n \t ");
  persistent::io::read_stream(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_5 () {
  test2 t2;
  std::istringstream is("{i1:815, t1:{i:911, j:203}, i2: 4711}");
  persistent::io::read_stream(is, t2);

  EXPECT_EQUAL(t2.i1(), 815);
  EXPECT_EQUAL(t2.t1().i(), 911);
  EXPECT_EQUAL(t2.t1().j(), 203);
  EXPECT_EQUAL(t2.i2(), 4711);
}

// --------------------------------------------------------------------------
void test_read_6 () {

  test_int64 t1;
  std::istringstream is("{i:4711,k:815}");
  try {
    persistent::io::read_stream(is, t1);
    EXPECT_FALSE("Exception expected");
  } catch (std::exception& ex) {
    EXPECT_TRUE("Exception expected");
  }

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_TRUE(is.good());
}

// --------------------------------------------------------------------------
void test_read_7 () {

  test3 t3;
  std::istringstream is("{v:[{i:1,j:2},{i:3,j:4},{i:5,j:6}]}");
  persistent::io::read_stream(is, t3);

  EXPECT_EQUAL(t3.v().size(), 3);
  EXPECT_EQUAL(t3.v()[0].i(), 1);
  EXPECT_EQUAL(t3.v()[0].j(), 2);
  EXPECT_EQUAL(t3.v()[1].i(), 3);
  EXPECT_EQUAL(t3.v()[1].j(), 4);
  EXPECT_EQUAL(t3.v()[2].i(), 5);
  EXPECT_EQUAL(t3.v()[2].j(), 6);
}

// --------------------------------------------------------------------------
void test_read_8 () {
  test5 t;
  std::istringstream is("{i:\"Text 5\",i:[\"List item 1\",\"List item 2\"]}");

  persistent::io::read_stream(is, t);

  std::vector<std::string> expected({"List item 1", "List item 2"});

  EXPECT_EQUAL(t.i(), "Text 5");
  EXPECT_EQUAL(t.l(), expected);

  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_9 () {
  test6 t;
  std::istringstream is("{i:\"Text 6\",i:[\"List item 1\",\"List item 2\"]}");

  persistent::io::read_stream(is, t);

  std::vector<std::string> expected({"List item 1", "List item 2"});

  EXPECT_EQUAL(t.i(), "Text 6");
  EXPECT_EQUAL(t.l(), expected);

  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_10 () {
  test5 t;
  std::istringstream is("{i:[\"List item 1\",\"List item 2\"],i:\"Text 7\"}");

  persistent::io::read_stream(is, t);

  std::vector<std::string> expected({"List item 1", "List item 2"});

  EXPECT_EQUAL(t.i(), "Text 7");
  EXPECT_EQUAL(t.l(), expected);

  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_11 () {
  test6 t;
  std::istringstream is("{i:[\"List item 1\",\"List item 2\"],i:\"Text 8\"}");

  persistent::io::read_stream(is, t);

  std::vector<std::string> expected({"List item 1", "List item 2"});

  EXPECT_EQUAL(t.i(), "Text 8");
  EXPECT_EQUAL(t.l(), expected);

  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_write_1 () {
  persistent::prop::int64 i("i", 4711);
  std::ostringstream os;
  persistent::io::write_stream(os, i);

  EXPECT_EQUAL(os.str(), "i:4711");
}

// --------------------------------------------------------------------------
void test_write_2 () {
  test_int64 t1;
  std::ostringstream os;
  persistent::io::write_stream(os, t1);

  EXPECT_EQUAL(os.str(), "{i:0,j:0}");
}

// --------------------------------------------------------------------------
void test_write_3 () {
  test2 t2;
  std::ostringstream os;
  persistent::io::write_stream(os, t2);

  EXPECT_EQUAL(os.str(), "{i1:0,t1:{i:0,j:0},i2:0}");
}

// --------------------------------------------------------------------------
void test_write_4 () {
  test3 t3;
  t3.v().push_back(test_int64());
  std::ostringstream os;
  persistent::io::write_stream(os, t3);

  EXPECT_EQUAL(os.str(), "{v:[{i:0,j:0}]}");
}

// --------------------------------------------------------------------------
void test_write_5 () {
  test5 t("Text 5", {"List item 1", "List item 2"});
  std::ostringstream os;
  persistent::io::write_stream(os, t);

  EXPECT_EQUAL(os.str(),"{i:\"Text 5\",i:[\"List item 1\",\"List item 2\"]}");
}

// --------------------------------------------------------------------------
void test_write_6 () {
  test6 t("Text 6", {"List item 1", "List item 2"});
  std::ostringstream os;
  persistent::io::write_stream(os, t);

  EXPECT_EQUAL(os.str(),"{i:[\"List item 1\",\"List item 2\"],i:\"Text 6\"}");
}

// --------------------------------------------------------------------------
void test_write_array () {

  persistent::prop::fix_list<int64_t, 5> a("a", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_stream(os, a);
  EXPECT_EQUAL(os.str(), "a:[1,2,3,4,5]");
}
// --------------------------------------------------------------------------
void test_write_vector () {

  persistent::prop::list<int64_t> v("v", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_stream(os, v);
  EXPECT_EQUAL(os.str(), "v:[1,2,3,4,5]");
}

// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);
  run_test(test_read_empty);
  run_test(test_read_all_basic_types);
  run_test(test_read_array);
  run_test(test_read_vector);
  run_test(test_read_prop_t_array);
  run_test(test_read_prop_t_vector);
  run_test(test_read_1);
  run_test(test_read_2);
  run_test(test_read_3);
  run_test(test_read_prop_t_3);
  run_test(test_read_4);
  run_test(test_read_5);
  run_test(test_read_6);
  run_test(test_read_7);
  run_test(test_read_8);
  run_test(test_read_9);
  run_test(test_read_10);
  run_test(test_read_11);

  run_test(test_write_array);
  run_test(test_write_vector);
  run_test(test_write_1);
  run_test(test_write_2);
  run_test(test_write_3);
  run_test(test_write_4);
  run_test(test_write_5);
  run_test(test_write_6);
}

// --------------------------------------------------------------------------

