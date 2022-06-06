# `cobb::reflex::enumeration`

A reflection-capable enumeration type relying on templates and compile-time strings. The following two enums are equivalent:

```c++
enum class Native {
   a,
   b,
   c = 5,
   d,
   e = 123,
};
Native my_native = Native::c;

using Reflex = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("a")>,
   cobb::reflex::member<cobb::cs("b")>,
   cobb::reflex::member<cobb::cs("c"), 5>,
   cobb::reflex::member<cobb::cs("d")>,
   cobb::reflex::member<cobb::cs("e"), 123>
>;
Reflex my_reflex = Reflex::value_of<cobb::cs("c")>;
```

The difference is that reflex enums can report information about themselves:

```c++
constexpr auto value_count = Reflex::value_count;
constexpr auto bits_needed = std::bit_width(value_count);

constexpr Reflex min_value = Reflex::min_value();
constexpr Reflex max_value = Reflex::max_value();

constexpr auto min_underlying = Reflex::min_underlying_value;
constexpr auto max_underlying = Reflex::max_underlying_value;

constexpr auto value_name = Reflex::value_of<cobb::cs("a")>.to_c_str(); // "a"

const auto value_name = Reflex::value_of<cobb::cs("a")>.to_string(); // std::string("a")
```

## Ranges and nested enums

Additionally, reflex enums support two additional types: *range members* and *nested enums.*

```c++
using Reflex = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // 0
   cobb::reflex::member<cobb::cs("B")>, // 1
   cobb::reflex::member_range<cobb::cs("C"), 3>, // 2, 3, 4, 5
   cobb::reflex::member<cobb::cs("D")> // 6
>;

constexpr Reflex c_base = Reflex::value_of<cobb::cs("C")>;    // == 2
constexpr Reflex c_0    = Reflex::value_of<cobb::cs("C"), 0>; // == 2
constexpr Reflex c_1    = Reflex::value_of<cobb::cs("C"), 1>; // == 3
constexpr Reflex c_2    = Reflex::value_of<cobb::cs("C"), 2>; // == 4
constexpr Reflex c_3    = Reflex::value_of<cobb::cs("C"), 3>; // == 5

// ---

using Inner = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("X")>,
   cobb::reflex::member<cobb::cs("Y")>,
   cobb::reflex::member<cobb::cs("Z")>
>;
using Outer = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>,
   cobb::reflex::nested_enum<
      cobb::cs("C"),
      Inner
   >,
   cobb::reflex::member<cobb::cs("D")>
>;

constexpr Outer o_a      = Outer::value_of<cobb::cs("A")>; // == 0
constexpr Outer o_b      = Outer::value_of<cobb::cs("B")>; // == 1
constexpr Outer o_c_base = Outer::value_of<cobb::cs("C")>; // == 2
constexpr Outer o_c_x    = Outer::value_of<cobb::cs("C"), cobb::cs("X")>; // == 2
constexpr Outer o_c_y    = Outer::value_of<cobb::cs("C"), cobb::cs("Y")>; // == 3
constexpr Outer o_c_z    = Outer::value_of<cobb::cs("C"), cobb::cs("Z")>; // == 4
constexpr Outer o_d      = Outer::value_of<cobb::cs("D")>; // == 5

// You can cast from a subset enum to a superset enum:
constexpr Inner z   = Inner::value_of<cobb::cs("Z")>; // == 2
constexpr Outer o_z = z; // == 4
```

## Metadata

You can assign metadata objects to enum members (except for nested enums, though the members of a nested enum can have their own metadata). Of course, because metadata is passed as a non-type template parameter, its type must be a [structural type](https://en.cppreference.com/w/cpp/language/template_parameters).

Metadata are only usable in any practical sense if all enum members specify either no metadata, or metadata of the same type. These are the only conditions under which the `to_metadata` instance member function can work.

```c++
struct metadata {
   int id;
};

using Reflex = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("a"), cobb::reflex::undefined, metadata{ .id = 5 }>,
   cobb::reflex::member<cobb::cs("b")>,
   cobb::reflex::member<cobb::cs("c"), 5, metadata{ .id = 10 }>,
   cobb::reflex::member<cobb::cs("d")>,
   cobb::reflex::member<cobb::cs("e"), 123>
>;

Reflex a = Reflex::value_of<cobb::cs("a")>;
Reflex b = Reflex::value_of<cobb::cs("b")>;
Reflex c = Reflex::value_of<cobb::cs("c")>;

constexpr auto* a_meta = a.to_metadata(); // ->id == 5
constexpr auto* b_meta = b.to_metadata(); // == nullptr
constexpr auto* c_meta = c.to_metadata(); // ->id == 10
```

## API reference

The template parameters for `enumration` can be either a list of members, or an underlying type followed by a list of members.

The following constraints apply to reflex enums:

* The underlying type, if specified, must be arithmetic and integral.
* All members must be `cobb::reflex::member_gap` or a specialization of `cobb::reflex::member`, `cobb::reflex::member_range`, or `cobb::reflex::nested_enum`.
  * The `member_gap` struct can be used as a quick way to insert gaps into a list (say, to match a large list with unused indices defined in some other system outside of your control) without having to manually count up and specify underlying values.
  * The templated `member` struct defines a single enum member: one name, one value. You can optionally specify an underlying value and a metadata object. If you wish to specify a metadata object but no manual underlying value, then pass `cobb::reflex::undefined` for the underlying value.
  * The templated `member_range` struct defines a range enum member: one name, multiple values. You must specify a name and a count, and can optionally specify the first underlying value and a metadata object.
  * The templated `nested_enum` struct allows you to nest another enum. You must specify a name and an enum type. Given an enum `outer` which nests some enum `inner`, you can cast from `inner` to `outer`, and can compare the two as well.
* All named members must have unique names.
* Members that manually specify an underlying value must specify one that can fit within the enum's underlying type.

### Constructors

* A `constexpr` default constructor
* A `constexpr` constructor taking the underlying type or any type convertible to it
* A `constexpr` constructor templated on any `cobb::reflex::enumeration` specialization; the constructor is valid if the other enumeration is a member of this one via a `nested_enum`.

### Operators

* Operators `==` and `!=` are defined for comparing two of the same reflex enum.
* Operators `==` and `!=` also support comparing two different reflex enum specializations, if one includes the other as a member via a `nested_enum`.

### Static members

#### `explicit_underlying_type`
A `static constexpr bool` whose value is true if an underlying type was specified for the enum, or false otherwise.

#### `from_int`
A `static` member function which accepts as an argument a single value that `is_convertible` to the `underlying_type`. It creates an instance of the enum with the specified value.

```c++
enum class Native {
   A = 5,
   B = 10,
   C = 15,
};
constexpr Native my_native = (Native)10;
static_assert(my_native == Native::B);

using Reflex = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A"), 5>,
   cobb::reflex::member<cobb::cs("B"), 10>,
   cobb::reflex::member<cobb::cs("C"), 15>
>;
constexpr Reflex my_reflex = Reflex::from_int(10);
static_assert(my_reflex == Reflex::value_of<cobb::cs("B")>);
```

#### `has<Name>`
A `static constexpr bool` templated on a `cobb::cs`, indicating whether the enum contains a member with the given name.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>
>;

static_assert(example::has<cobb::cs("A")> == true);
static_assert(example::has<cobb::cs("B")> == true);
static_assert(example::has<cobb::cs("Walrus")> == false);
```

#### `has_subset<T>`
A `static constexpr bool` templated on another `cobb::reflex::enumeration` specialization, indicating whether the context enum contains the parameter enum as a subset.

```c++
using inner = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("X")>,
   cobb::reflex::member<cobb::cs("Y")>,
   cobb::reflex::member<cobb::cs("Z")>
>;
using outer = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>,
   cobb::reflex::nested_enum<cobb::cs("C"), inner>
>;

static_assert(outer::has_subset<inner> == true);

using banana = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("rotate")>,
   cobb::reflex::member<cobb::cs("faster")>
>;

static_assert(outer::has_subset<banana> == false);
```

#### `min_value()` and `max_value()`
Two `static constexpr` member functions which return the minimum and maximum defined values in the enumeration. These have to be member functions due to `constexpr`-related language limitations as of C++20.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A"), -5>,
   cobb::reflex::member<cobb::cs("B"),  0>,
   cobb::reflex::member<cobb::cs("C"),  5>
>;

constexpr example min = example::min_value(); // == example::from_int(-5)
constexpr example max = example::max_value(); // == example::from_int( 5)
```

#### `min_underlying_value` and `max_underlying_value`
Two `static constexpr underlying_type` members representing the minimum and maximum defined values in the enumeration.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A"), -5>,
   cobb::reflex::member<cobb::cs("B"),  0>,
   cobb::reflex::member<cobb::cs("C"),  5>
>;

constexpr auto min = example::min_underlying_value; // == -5
constexpr auto max = example::max_underlying_value; //  5
```

#### `name_of_subset<T>`
A `static constexpr` member templated on another `cobb::reflex::enumeration` specialization. This template requires `has_subset<T>`, and, if the requirement is met, will return the name (`cobb::cs`) that the subset has been paired to.

```c++
using inner = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("X")>,
   cobb::reflex::member<cobb::cs("Y")>,
   cobb::reflex::member<cobb::cs("Z")>
>;
using outer = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>,
   cobb::reflex::nested_enum<cobb::cs("C"), inner>
>;

constexpr auto name = outer::name_of_subset<inner>; // == cobb::cs("C")
```

#### `underlying_type`
A `using`-declaration indicating the enum's underlying type. The default underlying type is `int`.

#### `underlying_value_of<cobb::cs Name>`
A `static constexpr underlying_type` member templated on a member name (`cobb::cs`), and equivalent to the underlying value for that member. This constant `requires` that the enum `has<Name>`.

#### `underlying_value_of<cobb::cs Name, size_t Index>`
A `static constexpr underlying_type` member templated on a member name (`cobb::cs`) and an index, and equivalent to the underlying value for that member. This constant `requires` that the enum `has<Name>` and that the name refer to a `member_range`.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::member_range<cobb::cs("C"), 4> // == 2, 3, 4, 5
>;

constexpr auto c_3 = example::underlying_value_of<cobb::cs("C"), 3>; // == 5
```

#### `underlying_value_of<cobb::cs Name, cobb::cs InnerName>`
A `static constexpr underlying_type` member which allows you to get the underlying value of a member of a nested enum. It's templated on the name of the nested enum and the name of its value. This constant `requires` that the enum `has<Name>` and that the name refer to a `nested_enum`.

```c++
using inner = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("X")>,
   cobb::reflex::member<cobb::cs("Y")>,
   cobb::reflex::member<cobb::cs("Z")>
>;
using outer = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::nested_enum<cobb::cs("C"), inner> // == 2, 3, 4
>;

constexpr auto c_z = outer::underlying_value_of<cobb::cs("C"), cobb::cs("Z")>; // == 4
```

#### `underlying_value_range`
A `static constexpr underlying_type` member equal to `max_underlying_value - min_underlying_value + 1`. This value represents the full range of enum members' values, including any gaps between members.

#### `value_count`
A `static constexpr size_t` member indicating the number of defined members and their values within the enumeration.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::member_range<cobb::cs("Range of Three"), 3>, // == 2, 3, 4
   cobb::reflex::nested_enum< // == 5, 6, 7, 8, 9
      cobb::cs("Enum of Five"),
      cobb::reflex::enumeration<
         cobb::reflex::member<cobb::cs("V")>,
         cobb::reflex::member<cobb::cs("W")>,
         cobb::reflex::member<cobb::cs("X")>,
         cobb::reflex::member<cobb::cs("Y")>,
         cobb::reflex::member<cobb::cs("Z")>
      >
   >
>;

constexpr auto value_count = example::value_count; // == 10

// A (1) + B (1) + Range of Three (3) + Enum of Five (5)
```

#### `value_of<cobb::cs Name>`
A `static constexpr enumeration<...>` member templated on a member name (`cobb::cs`), and equivalent to the value for that member (i.e. a `from_int` call). This constant `requires` that the enum `has<Name>`.

#### `value_of<cobb::cs Name, size_t Index>`
A `static constexpr enumeration<...>` member templated on a member name (`cobb::cs`) and an index, and equivalent to the value for that member. This constant `requires` that the enum `has<Name>` and that the name refer to a `member_range`.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::member_range<cobb::cs("C"), 4> // == 2, 3, 4, 5
>;

constexpr outer c_3 = example::value_of<cobb::cs("C"), 3>; // == example::from_int(5)
```

#### `value_of<cobb::cs Name, cobb::cs InnerName>`
A `static constexpr enumeration<...>` member which allows you to get the value (within an outer enum) of a member of a nested enum. It's templated on the name of the nested enum and the name of its value. This constant `requires` that the enum `has<Name>` and that the name refer to a `nested_enum`.

```c++
using inner = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("X")>,
   cobb::reflex::member<cobb::cs("Y")>,
   cobb::reflex::member<cobb::cs("Z")>
>;
using outer = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::nested_enum<cobb::cs("C"), inner> // == 2, 3, 4
>;

constexpr outer c_z = outer::value_of<cobb::cs("C"), cobb::cs("Z")>; // == outer::from_int(4)
```

### Instance members

#### `to_c_str`
If the instance has a `valid` value, returns a `const char*` pointer representing the value's assigned name.

#### `to_int`
Returns the instance's current value in `underlying_type` form.

#### `to_metadata`
If all members of the enum have the same metadata type (save for those which define no metadata), then this function returns a pointer to the metadata for this instance's value (if the value is `valid`), or `nullptr` otherwise.

#### `to_string`
If the instance has a `valid` value, returns a `std::string` representing the value's assigned name. Otherwise, returns an empty `std::string`.