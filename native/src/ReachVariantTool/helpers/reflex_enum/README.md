# `reflex_enum`

A reimplementation of enums using templates, to allow reflection. The following two definitions are analogous to each other:

```c++
enum class bare_enum {
   a,
   b,
   c = 3,
   d = 1234,
};

using alternate_enum = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_member<cobb::cs("c")>,
   cobb::reflex_enum_member<cobb::cs("d"), 1234>,
>;
```

The first template parameter can optionally be the enum's underlying type; otherwise, the underlying type is `int`. An explicitly specified underlying type must be an integer type (`is_arithmetic` and `is_integral`).

```c++
using example = cobb::reflex_enum<
   int16_t,
   cobb::reflex_enum_member<cobb::cs("ABC"), -1>,
   cobb::reflex_enum_member<cobb::cs("DEF"),  0>
>;
```

All parameters besides the underlying type (if specified) must be a specialization of the `reflex_enum_member` type, or must be the `reflex_enum_gap` type. Multiple members of the same reflex enum cannot have the same name.

## Defining an enum's contents

### `reflex_enum_member`

Defines a named enum member and optionally its underlying value. If the underlying value is not specified, then it is computed automatically in a manner consistent with "real" enums in C++: the previous member's value plus one, or if there is no previous member, 0.

When manually specifying an underlying value for an enum member, the value type must be convertible to the `intmax_t` type.

### `reflex_enum_gap`

This type allows you to add gaps in the middle of the list, without having to assign explicit underlying values to the members after the gaps. The following two enums are equivalent:

```c++
using example_a = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("value 0")>,
   cobb::reflex_enum_member<cobb::cs("value 1")>,
   cobb::reflex_enum_member<cobb::cs("value 5"), 5>
>;

using example_b = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("value 0")>,
   cobb::reflex_enum_member<cobb::cs("value 1")>,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_member<cobb::cs("value 5")>
>;
```

The main use case for this is large enums that are meant to represent the values in some system outside of your control, where there may be gaps in that list. As an example, *Halo: Reach* has a massive list of "incidents" used as a common identifier type for game engine events, and there are unused indices in this list. Given the sheer size of the list, manually assigning underlying values is less practical than just being able to insert gaps.

## Notes on IntelliSense

As of this writing (May 27, 2022), IntelliSense has the following issues with reflex enums:

* Strings can only be used as template parameters if they rely on some sort of fixed-length storage. The `cobb::cs` type can map a string literal to an appropriately sized `std::array` of `char` to comply with this limitation; however, IntelliSense tooltips will display a `cobb::cs` instance by listing out its character codes, not its actual glyphs. This extends to the `reflex_enum` specialization as a whole; tooltips for these types are illegible.

  As an example, `cobb::cs("hello")` is shown in IntelliSense tooltips as somthing like `{ (char)104, (char)101, (char)108, (char)108, (char)111, (char)0 }` rather than a more sensible `{ 'h', 'e', 'l', 'l', 'o', '\0' }`.

* Large `reflex_enum`s (hundreds of members) can crash IntelliSense outright, preventing syntax highlighting and other IntelliSense features for the containing file. Visual Studio itself does not crash, and remains usable.

The following are not issues with IntelliSense per se, but are weaknesses of reflex enums:

* Auto-completion for enum member names is not possible. That said, template constraints are used so that things like `value_of` will show up as errors (including in IntelliSense) if you use a name that doesn't match any member in a reflex enum specialization.

## Static members

### `explicit_underlying_type`

A `static constexpr bool` whose value is `true` if an underlying type was provided when specializing `reflex_enum`.

```c++
using example_a = cobb::reflex_enum<
   int,
   cobb::reflex_enum_member<cobb::cs("a")>
>;

using example_b = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>
>;

static_assert(std::is_same_v<example_a::underlying_type, example_b::underlying_type>); // does not fail

constexpr bool a_is_explicit = example_a::explicit_underlying_type; // true
constexpr bool b_is_explicit = example_b::explicit_underlying_type; // false
```

### `for_each_member`

Executes a templated lambda once per enum member.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c")>,
   cobb::reflex_enum_member<cobb::cs("d"), 5>,
   cobb::reflex_enum_member<cobb::cs("e")>
>;

example::for_each_member([]<typename Current>() {
   printf("member %s == %d", Current::name.c_str(), example::underlying_value_of<Current::name>);
   //
   // Note: we don't pull Current::value because that only gets explicitly-set values, 
   //       i.e. it would only work for "d" here.
   //
});

// prints:
// "member a == 0"
// "member b == 1"
// "member c == 2"
// "member d == 5"
// "member e == 6"
```

### `from_int`

Constructs an instance given any value that `is_convertible` to the underlying type. This is equivalent to assigning an integer value to an `enum class` variable by casting the value to the enum type.

```c++
using example = cobb::reflex_enum<
   uint8_t,
   cobb::reflex_enum_member<cobb::cs("a")>
>;

example my_value = example::from_int(5);
```

### `has`

A `static constexpr bool` templated on an enum name; allows you to check if a reflex enum specialization defines a member with a given name.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>
>;

constexpr bool has_a = example::has<cobb::cs("a")>; // true
constexpr bool has_b = example::has<cobb::cs("b")>; // false
```

### `has_negative_members`

A `static constexpr bool` whose value is `true` if the enum's underlying type is signed and if any enum member has an underlying value below zero.

```c++
using example_a = cobb::reflex_enum<
   int16_t,
   cobb::reflex_enum_member<cobb::cs("x"), 5>
>;
using example_b = cobb::reflex_enum<
   uint16_t,
   cobb::reflex_enum_member<cobb::cs("y"), -1>
>;
using example_c = cobb::reflex_enum<
   int16_t,
   cobb::reflex_enum_member<cobb::cs("z"), -1>
>;

constexpr bool a_negative = example_a::has_negative_members; // false
constexpr bool b_negative = example_b::has_negative_members; // false (underlying type is unsigned)
constexpr bool c_negative = example_c::has_negative_members; // true
```

### `member_count`

A `static constexpr size_t` holding the number of members in the enum. This does not include gaps.

```c++
using example_a = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c")>
>;

using example_b = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_member<cobb::cs("e"), 5>
>;

using example_b = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_gap,
   cobb::reflex_enum_member<cobb::cs("e"), 5>
>;

constexpr size_t a_count = example_a::member_count; // == 3
constexpr size_t b_count = example_b::member_count; // == 3
constexpr size_t c_count = example_c::member_count; // == 3
```

### `min_underlying_value` and `max_underlying_value`

Static constexpr members holding the lowest and highest (respectively) underlying values of all members.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c"), 7575>,
   cobb::reflex_enum_member<cobb::cs("d"), -5000>
>;

constexpr int min = example::min_underlying_value; // == -5000
constexpr int max = example::max_underlying_value; // ==  7575
```

### `min_value` and `max_value`

Static constexpr getter functions which return the members with the lowest and highest (respectively) underlying values.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c"), 7575>,
   cobb::reflex_enum_member<cobb::cs("d"), -5000>
>;

constexpr example min = example::min_value(); // == example::from_int(-5000)
constexpr example max = example::max_value(); // == example::from_int( 7575)
```

These have to be getters rather than constexpr members due to C++20 language limitations: a type `Foo` cannot have a `static constexpr Foo` member because constexpr values cannot be forward-declared, and their types must be fully defined at the time their values are defined.

### `underlying_type`

A `using`-declaration indicating the enum's underlying type.

```c++
using example_a = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>
>;

using example_b = cobb::reflex_enum<
   uint16_t,
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>
>;

static_assert(std::is_same_v<example_a, int>);
static_assert(std::is_same_v<example_b, uint16_t>);
```

### `underlying_value_of`

Templated on a member name; returns the member's underlying value. This template `requires` that `has<Name>` be satisfied.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_member<cobb::cs("c"), 2>
>;

constexpr auto int_a = example::underlying_value_of<cobb::cs("a")>; // == 0

constexpr auto int_b = example::underlying_value_of<cobb::cs("b")>; // == 1

constexpr auto int_c = example::underlying_value_of<cobb::cs("c")>; // -- 2
```

### `value_of`

Templated on a member name; returns an instance with that value. This template `requires` that `has<Name>` be satisfied.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_member<cobb::cs("c"), 2>
>;

example value_a = example::value_of<cobb::cs("a")>;
auto    int_a   = value_a.to_int(); // == 0

example value_b = example::value_of<cobb::cs("b")>;
auto    int_b   = value_b.to_int(); // == 1

example value_c = example::value_of<cobb::cs("c")>;
auto    int_c   = value_c.to_int(); // == 2
```

## Instance members

### `assign`

Templated on a member name; assigns that value to the instance.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c")>
>;

example foo;
foo.assign<cobb::cs("c")>();
foo.to_int(); // == 2
```

### `is`

Templated on a member name; returns `true` if the instance holds the corresponding value.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c")>
>;

example foo = example::value_of<cobb::cs("c")>;

foo.is<cobb::cs("a")>(); // false
foo.is<cobb::cs("c")>(); // true
foo.is<cobb::cs("banana")>(); // false (name not in enum)
```

### `to_int`

Returns the instance's underlying value, as the `underlying_type`.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a")>,
   cobb::reflex_enum_member<cobb::cs("b")>,
   cobb::reflex_enum_member<cobb::cs("c")>
>;

example foo = example::value_of<cobb::cs("b")>;
foo.to_int(); // == 1
```

### `to_string`

Returns the name of the member whose value matches the instance's value, as a `const char*`. If the instance isn't `valid`, this returns `nullptr`.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_member<cobb::cs("c"), 2>
>;

example value_0 = example::from_int(0);
example value_7 = example::from_int(7);

value_0.to_string(); // == "a"
value_7.to_string(); // == nullptr
```

### `valid`

Returns `true` if the instance's underlying value matches any of the enum's members.

```c++
using example = cobb::reflex_enum<
   cobb::reflex_enum_member<cobb::cs("a"), 0>,
   cobb::reflex_enum_member<cobb::cs("b"), 1>,
   cobb::reflex_enum_member<cobb::cs("c"), 2>
>;

example value_0 = example::from_int(0);
example value_7 = example::from_int(7);

value_0.valid(); // == true (matches "a")
value_7.valid(); // == false (no matching enum members)
```