
#include "persistent/persistent_xml.h"
#include <testing/testing.h>

#include "test_structs.h"

std::string build_xml (const std::string& t) {
  std::ostringstream os;
  os << persistent::io::xml::s_header;
  os << persistent::io::xml::s_body;
  os << t;
  os << persistent::io::xml::s_nbody;
  return os.str();
}

// --------------------------------------------------------------------------
void test_read_empty () {

  persistent::prop<test_int64> t1("t");
  std::istringstream is(build_xml("<t></t>"));
  persistent::io::read_xml(is, t1);

  EXPECT_EQUAL(t1().i(), 0);
  EXPECT_EQUAL(t1().j(), 0);
  EXPECT_TRUE(is.good());
  is >> std::ws;
  EXPECT_FALSE(is.good());
  EXPECT_TRUE(is.eof());
}

// --------------------------------------------------------------------------
void test_read_array () {

  persistent::fix_list<int64_t, 5> a("a");
  std::istringstream is(build_xml("<a><ol><li>1</li><li>2</li><li>3</li><li>4</li><li>5</li></ol></a>"));
  persistent::io::read_xml(is, a);

  std::array<int64_t, 5> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(a(), expected);
}
// --------------------------------------------------------------------------
void test_read_vector () {

  persistent::list<int64_t> v("v");
  std::istringstream is(build_xml("<v><ol><li>1</li><li>2</li><li>3</li><li>4</li><li>5</li></ol></v>"));
  persistent::io::read_xml(is, v);

  std::vector<int64_t> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(v(), expected);
}

// --------------------------------------------------------------------------
void test_read_1 () {
  persistent::int64 i("i");
  std::istringstream is(build_xml("<i>4711</i>"));
  persistent::io::read_xml(is, i);

  EXPECT_EQUAL(i(), 4711);
  is >> std::ws;
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_2 () {

  persistent::prop<test_int64> t1("t");
  std::istringstream is(build_xml("<t><i>4711</i><j>815</j></t>"));
  persistent::io::read_xml(is, t1);

  EXPECT_EQUAL(t1().i(), 4711);
  EXPECT_EQUAL(t1().j(), 815);
  is >> std::ws;
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_3 () {
  persistent::prop<test_int64> t1("t");
  std::istringstream is(build_xml("<t><i>4711</i></t>"));
  persistent::io::read_xml(is, t1);

  EXPECT_EQUAL(t1().i(), 4711);
  EXPECT_EQUAL(t1().j(), 0);
  is >> std::ws;
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_4 () {
  persistent::prop<test_int64> t1("t");
  std::istringstream is(build_xml(" \n \t <t> \n \t <i> \n \t \n \t 4711 \n \t </i> \n \t </t> \n \t "));
  persistent::io::read_xml(is, t1);

  EXPECT_EQUAL(t1().i(), 4711);
  EXPECT_EQUAL(t1().j(), 0);
  is >> std::ws;
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_5 () {
  persistent::prop<test2> t2("t2");
  std::istringstream is(build_xml("<t2><i1>815</i1><t1><i>911</i><j>203</j></t1><i2>4711</i2></t2>"));
  persistent::io::read_xml(is, t2);

  EXPECT_EQUAL(t2().i1(), 815);
  EXPECT_EQUAL(t2().t1().i(), 911);
  EXPECT_EQUAL(t2().t1().j(), 203);
  EXPECT_EQUAL(t2().i2(), 4711);
}

// --------------------------------------------------------------------------
void test_read_6 () {
  persistent::prop<test_int64> t1("t1");
  std::istringstream is(build_xml("<t1><i>4711</i><k>815</k></t1>"));
  try {
    persistent::io::read_xml(is, t1);
    EXPECT_FALSE("Exception expected");
  } catch (std::exception& ex) {
    EXPECT_TRUE("Exception expected");
  }

  EXPECT_EQUAL(t1().i(), 4711);
  EXPECT_EQUAL(t1().j(), 0);
  EXPECT_TRUE(is.good());
}

// --------------------------------------------------------------------------
void test_read_7 () {
  persistent::prop<test3> t3("t3");
  std::istringstream is(build_xml("<t2><v><ol><li><i>1</i><j>2</j></li><li><i>3</i><j>4</j></li><li><i>5</i><j>6</j></li></ol></v></t2>"));
  persistent::io::read_xml(is, t3);

  EXPECT_EQUAL(t3().v().size(), 3);
  EXPECT_EQUAL(t3().v()[0].i(), 1);
  EXPECT_EQUAL(t3().v()[0].j(), 2);
  EXPECT_EQUAL(t3().v()[1].i(), 3);
  EXPECT_EQUAL(t3().v()[1].j(), 4);
  EXPECT_EQUAL(t3().v()[2].i(), 5);
  EXPECT_EQUAL(t3().v()[2].j(), 6);
}

// --------------------------------------------------------------------------
void test_write_1 () {
  persistent::int64 i("i", 4711);
  std::ostringstream os;
  persistent::io::write_xml(os, i, false);

  auto expected = build_xml("<i>4711</i>");
  EXPECT_EQUAL(os.str(), expected);
}

// --------------------------------------------------------------------------
void test_write_2 () {
  test_int64 t1;
  std::ostringstream os;
  persistent::io::write_xml(os, t1, false);

  auto expected = build_xml("<i>0</i><j>0</j>");
  EXPECT_EQUAL(os.str(), expected);
}

// --------------------------------------------------------------------------
void test_write_3 () {
  test2 t2;
  std::ostringstream os;
  persistent::io::write_xml(os, t2, false);

  auto expected = build_xml("<i1>0</i1><t1><i>0</i><j>0</j></t1><i2>0</i2>");
  EXPECT_EQUAL(os.str(), expected);
}

// --------------------------------------------------------------------------
void test_write_4 () {
  test3 t3;
  t3.v().push_back(test_int64());
  std::ostringstream os;
  persistent::io::write_xml(os, t3, false);

  auto expected = build_xml("<v><ol><li><i>0</i><j>0</j></li></ol></v>");
  EXPECT_EQUAL(os.str(), expected);
}

// --------------------------------------------------------------------------
void test_write_array () {

  persistent::fix_list<int64_t, 5> a("a", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_xml(os, a, false);

  auto expected = build_xml("<a><ol><li>1</li><li>2</li><li>3</li><li>4</li><li>5</li></ol></a>");
  EXPECT_EQUAL(os.str(), expected);
}
// --------------------------------------------------------------------------
void test_write_vector () {

  persistent::list<int64_t> v("v", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_xml(os, v, false);
  auto expected = build_xml("<v><ol><li>1</li><li>2</li><li>3</li><li>4</li><li>5</li></ol></v>");
  EXPECT_EQUAL(os.str(), expected);
}

// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);
  run_test(test_read_empty);
  run_test(test_read_array);
  run_test(test_read_vector);
  run_test(test_read_1);
  run_test(test_read_2);
  run_test(test_read_3);
  run_test(test_read_4);
  run_test(test_read_5);
  run_test(test_read_6);
  run_test(test_read_7);

  run_test(test_write_array);
  run_test(test_write_vector);
  run_test(test_write_1);
  run_test(test_write_2);
  run_test(test_write_3);
  run_test(test_write_4);
}

// --------------------------------------------------------------------------

