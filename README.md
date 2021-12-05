# Struct persistence

A header only library to persist structures.

## Motivation

In other languages like java, you can easily persist structs or classes by using
the reflection of that language. In C++ there is no such machanismn, but with
templates, there is a very powefull mechanismn to retrieve similar comfort with
the addition to have an high optimized code specialized for your struct.

## Expected usage:

Let's have a simple structure,

```c++

  struct MyStruct {
    double d;
    int i;
    std::string s;
    std::array<int, 5> a;
    std::vector<std::string> v;
  };

  MyStruct s;

```

And we want to store it in json format:

```c++

  std::ofstream f("file.json");
  write_json(f, s);

```

We expect to have a json like this:

```json

{
  "d": "1.234",
  "i": "4711",
  "s": "Some text",
  "a": [ "1", "2", "3", "4", "5" ],
  "v": [ "One", "Two", "Three" ]
}

```

... or a XML like this

```xml

<?xml version="1.0" encoding="utf-8"?>
<body>
  <d>1.234</d>
  <i>4711</i>
  <s>"Some text"</s>
  <a>
    <ol>
      <li>1</li>
      <li>2</li>
      <li>3</li>
      <li>4</li>
      <li>5</li>
    </ol>
  </a>
  <v>
    <ol>
      <li>"One"</li>
      <li>"Two"</li>
      <li>"Three"</li>
    </ol>
  </v>
</body>

```

And, of course, we expect to read it back

```c++

  MyStruct s2;
  std::ifstream f("file.json");
  read_json(f, s2);

```

## Implementation

Since c++ has no build in mechanismn to get reflection information about the
members of a struct or class, this is done by a tuple that hold just the
information to the defined members of a struct.
Additional the members get a name information so they can be stored in a
name/key based sink. For that, some changes are necessary to the basic struct.

1. The struct itself must be recognized as a persistent struct.
2. The members have to be a type that holds the value itself with a get/Set
   interface and the name of the member.
3. A tuple that hold the information about the members.

# Usage

## Include it to your code

Since it's a header only library you can just copy the whole directory to your
project and include it. For convenience there is also a cmake project, that can
 be included to your 
cmake file.

## Define a persistent struct

```c++

using namespace persistent;

struct MyStruct : public basic_container {  // persistent structs must be subclasses of a basic_container

  MyStruct (double d_ = 0.0, int64_t i_ = 0, const std::string& s_ = {}, const std::array<int, 5>& a_ = {}, const std::vector<std::string>& v_ = {})
    : members(d, i, s, a, v)          // each member has to be registered in the members suple.
    , d("d", d_)                      // each member has to be initialized with a name and optional with a value.
    , i("i", i_)
    , s("s", s_)
    , a("a", a_)
    , v("v", v_)
  {}

  MyStruct (const MyStruct& rhs)      // The copy operator hast to be rewritten that way,
    : MyStruct () {
    members = rhs.members;
  }

  prop<double> d;                     // Each member is now a prop<Type> memeber.
  prop<int> i;
  prop<std::string> s;
  prop<std::array<int, 5>> a;
  prop<std::vector<std::string>> v;

  // A typedef for the typesafe tuple that holds the member information.
  typedef member_variables_t<decltype(d), decltype(i), decltype(s), decltype(a), decltype(v)> member_variables;

  member_variables& get_members () {  // Accessors for the member information
    return members;
  }

  const member_variables& get_members () const {  // If only this exists, the struct can only be written, but not readen.
    return members;
  }

private:
  member_variables members;           // the tuple, that holds the member information

};

```

## Write it to json

To write the struct to a specific format, you just have to call write_XYZ(ostream, MyStruct-object)
For example to write to a json format:

```c++

  MyStruct s{1.234, 4711, "Some text", {1, 2, 3, 4, 5}, {"One", "Two", "Three"}};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

```

## Write it to xml

Write it to a xml format:


```c++

  MyStruct s{1.234, 4711, "Some text", {1, 2, 3, 4, 5}, {"One", "Two", "Three"}};

  std::ostringstream os;
  persistent::io::write_xml(os, s, false);

```

## Read it from json

```c++
  MyStruct s;

  std::istringstream is("{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}");
  persistent::io::read_json(is, s);

```

# Convenience

For convenience there is a second way to declare a persistent struct.

```c++

  struct MyStruct2 : public basic_struct<int64, text> {   // make your struct a subclass of basic_struct and tell it your property types.
    typedef basic_struct<int64, text> super;              // just for more convenience...
  
    MyStruct2 (int64_t i_ = {}, const std::string s_ = {})
      : super(i, s)                                       // register your properties
      , i("i", i_)                                        // give your properties a name
      , s("s", s_)
    {}
  
    MyStruct2 (const MyStruct2& rhs)                      // The copy operator also has to be redefined
      : MyStruct2() {
      super::operator=(rhs);
    }
  
    int64 i;                                              // predefined properties for build in types.
    text s;                                               // A string is now a text to avoid nameing collisions
  };

```
