# C++ struct persistence

A header only library to persist c++ structures.

## Motivation

In other languages, like java, you can easily persist structs or classes by using
the reflection of that language. In C++ there is no such mechanism, but with
templates, there is a very powefull mechanism to retrieve similar comfort with
the addition to have high optimized code specialized for your struct.

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
2. The members have to be associated with a name and stored/loaded dpendend of their type.
3. A tuple that hold the information about the members.

# Usage

## Include it to your code

Since it's a header only library you can just copy the whole directory to your
project and include it. For convenience there is also a cmake project, that can 
be included to your cmake file.

## Define a persistent struct

```c++

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

