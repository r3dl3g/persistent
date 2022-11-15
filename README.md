# C++ struct persistence

A header only library to persist c++ structures.

## Motivation

In other languages, like java, you can easily persist structs or classes by using
the reflection of that language. In C++ there is no such mechanism, but with
templates and deducion, there is a even more powefull mechanism to retrieve similar comfort with
the addition to have high optimized specialized code for your structs.

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
  "d": 1.234,
  "i": 4711,
  "s": "Some text",
  "a": [ 1, 2, 3, 4, 5 ],
  "v": [ "One", "Two", "Three" ]
}

```

... or a XML like this

```xml

<?xml version="1.0" encoding="utf-8"?>
<body>
  <d>1.234</d>
  <i>4711</i>
  <s>Some text</s>
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
      <li>One</li>
      <li>Two</li>
      <li>Three</li>
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
members of a struct or class, this is done by deduction and for structs by a 
tuple that hold just the information to the defined members of that struct.
Additional the members get a name information so they can be stored in a
name/key based sink. For that, some changes are necessary to the basic struct.

1. The struct itself must be recognized as a persistent struct.
2. The members have to be associated with a name and stored/loaded dpendend of their type.

# Usage

## Include it to your code

Since it's a header only library you can just copy the whole directory to your
project and include it. For convenience there is also a cmake project, that can 
be included to your cmake file.

## Define a persistent struct

There are 2 ways to declare persistence.

1. Inside your class/struct.

- Make your class/struct a sub-class of persistent_struct. It doesn't matter if you sub class it private, protected or public.
- Declare a method with name 'attributes' that return a tuple with attributes of your member variables to be persist.
  You can see below, how easy this can be done. Just return make_attributes(...) with a list of attribute objects.
  Here you specify the persistent name of each attribute. The type will be recognized automtaically.

With this approach your members can be protected or private. Only the 'attributes' method has to be accessible from outside.

```c++

#include "persistent/persistent.h"

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
    return make_attributes(attribute(d, "d"), attribute(i, "i"), attribute(s, "s"), attribute(a, "a"), attribute(v, "v"));
  }

};

```

2. Outside your class/struct.

For some resons it's not possible sub class your class/struct or to declare this method inside your class/struct.
Or you just don'T want to change it.
But you can receive the same result without modify your class/struct by declare a struct and a 
function for your class/struct inside the namespace 'persistent'.

- Define a specialization of the template struct 'is_persistent' for your class/struct that just define
  the enum value to true (See below).
- Define a specialization of the template function attributes<> for your class/struct that return a 
  tuple with attributes of your member variables to be persist.

With this approach there is no need to modify your class/struct.

- 
```c++

#include "persistent/persistent.h"

// --------------------------------------------------------------------------
struct MyStruct {

  std::string str;
  int i;

};

namespace persistent {

  template <> 
  struct is_persistent<MyStruct> : std::true_type {};

  template<>
  auto attributes<MyStruct> (MyStruct& t) {
    return make_attributes(attribute(t.str, "str"), attribute(t.i, "i"));
  }

}

```

2.1. Outside with hidden members.

If your members are hidden, you can optional define setter and getter methods to read or write the meber values.

```c++

#include "persistent/persistent.h"

// --------------------------------------------------------------------------
struct MyStruct {

  MyStruct (const std::string& str_ = {}, int i_ = {})
    : str(str_)
    , i(i_)
  {}
  
  const std::string& get_string () const {
    return str;
  }
  
  int get_integer () const {
    return i;
  }
  
  void set_string (const std::string& s) {
    str = s;
  }
  
  void set_integer (int i_) {
    i = i_;
  }

private:
  std::string str;
  int i;

};

namespace persistent {

  template <>
  struct is_persistent<MyStruct> : std::true_type {};
  
  template<>
  auto attributes (MyStruct& t) {
    return make_attributes(setter(t, &MyStruct::set_string, "str"), setter(t, &MyStruct::set_integer, "i"));
  }
  
  template<>
  auto attributes (const MyStruct& t) {
    return make_attributes(getter(t.get_string(), "str"), getter(t.get_integer(), "i"));
  }

}

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

  std::istringstream is("{\"d\":1.234,\"i\":4711,\"s\":\"Some text\",\"a\":[1,2,3,4,5],\"v\":[\"One\",\"Two\",\"Three\"]}");
  persistent::io::read_json(is, s);

```

