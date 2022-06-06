# `cobb::reflex::flags_mask`

A reflection-capable flags-mask type relying on templates and compile-time strings. It is a sister type to `cobb::reflex::enumeration`, and is used similarly.

```c++
struct Native {
   Native() = delete;
   enum {
      a,
      b,
      c = 3,
      d,
      e
   };
};
uint32_t my_native = (1 << Native::a) | (1 << Native::c);
// == (1 << 0) | (1 << 3)
// == 9

using Reflex = cobb::reflex::reflex_flags<
   cobb::reflex::member<cobb::cs("a")>,
   cobb::reflex::member<cobb::cs("b")>,
   cobb::reflex::member<cobb::cs("c"), 3>,
   cobb::reflex::member<cobb::cs("d")>,
   cobb::reflex::member<cobb::cs("e")>
>;
Reflex my_reflex = Reflex::from<(cobb::cs("a")), (cobb::cs("c"))>;
// underlying value == 9
```

As with `cobb::reflex::enumeration`, reflex-flags can report information about themselves, though that information is somewhat limited owing to the need to represent multiple values (flags) at a time.

As with `cobb::reflex::enumeration`, range members are supported. Nested enumerations are not, and there is no system for nested flags masks.

## Metadata

You can assign metadata objects to flags. Of course, because metadata is passed as a non-type template parameter, its type must be a [structural type](https://en.cppreference.com/w/cpp/language/template_parameters).

For full metadata functionality to be available, all flags must specify either no metadata, or metadata of the same type.

```c++
struct metadata {
   int id;
};

using Reflex = cobb::reflex::flags_mask<
   cobb::reflex::member<cobb::cs("a"), cobb::reflex::undefined, metadata{ .id = 5 }>,
   cobb::reflex::member<cobb::cs("b")>,
   cobb::reflex::member<cobb::cs("c"), 5, metadata{ .id = 10 }>,
   cobb::reflex::member<cobb::cs("d")>,
   cobb::reflex::member<cobb::cs("e"), 123>
>;

Reflex my_reflex = Reflex::from<cobb::cs("a"), cobb::cs("b"), cobb::cs("c")>;

int total = 0;
my_reflex.for_each_metadata([&total](const metadata& md) {
   total += md.id;
});

// the lambda runs for A and C only, since B has no metadata
// total == 15
```

## API reference

The template parameters for `flags_mask` can be either a list of members, or an underlying type followed by a list of members.

The following constraints apply to reflex enums:

* The underlying type, if specified, must be arithmetic and integral.
* All members must be `cobb::reflex::member_gap` or a specialization of `cobb::reflex::member` or `cobb::reflex::member_range`.
  * The `member_gap` struct can be used as a quick way to insert gaps into a list (say, to match a large list with unused indices defined in some other system outside of your control) without having to manually count up and specify underlying flag indices.
  * The templated `member` struct defines a single flag: one name, one value. You can optionally specify a flag index (such that the flag value is `1 << specified`) and a metadata object. If you wish to specify a metadata object but no manual underlying value, then pass `cobb::reflex::undefined` for the underlying value.
  * The templated `member_range` struct defines a range of flags: one name, multiple values. You must specify a name and a count, and can optionally specify the first underlying flag index and a metadata object.
* All named members must have unique names.
* All defined flags' values must be able to fit in the mask's underlying type, whether or not a flag's underlying index was explicitly specified.

### Constructors

* A `constexpr` default constructor
* A `constexpr` constructor taking the underlying type or any type convertible to it

### Operators

* Operators `==` and `!=` are defined for comparing two of the same reflex enum.
* Operators `|=` and `&=` are defined for use with an operand of the same reflex-flags type, as are `|` and `&`.

### Static members

#### `all_underlying_values`
A `static constexpr` array of value type `underlying_type`, whose entries are the underlying values of all valid flags.

#### `explicit_underlying_type`
A `static constexpr bool` whose value is true if an underlying type was specified for the enum, or false otherwise.

#### `flag_count`
A `static constexpr size_t` indicating the number of defined flags (counting ranges multiple times based on their own counts). This is not the same as the highest flag index; `flag_count` does not count gaps.

#### `flag_index<Name>`
A `static constexpr size_t` templated on a `cobb::cs`, returning the bit index of the flag with the given name. `1 << flag_index<...>` would produce the flag's underlying value.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>,
   cobb::reflex::member<cobb::cs("C")>
>;

constexpr auto b_index = example::flag_index<cobb::cs("B")>;
static_assert(example::underlying_value_of<cobb::cs("B")> == (1 << b_index));
```

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

#### `names_per_bit`
A `static constexpr` array of `const char*`, of size `max_flag_index + 1`. The entry for index `i` is the name of the flag with underlying value `1 << i`. Gaps will have `nullptr` as their entry.

#### `metadata_per_bit`
A `static constexpr` array of metadata objects, of size `max_flag_index + 1`. If the requirements for full metadata functionality are met, then array items will match any metadata specified when defining the flags-mask type.

#### `metadata_presence_per_bit`
A `static constexpr` value of `underlying_type`, functioning as an integral flags mask. If the requirements for full metadata functionality are met, then bits are set if a corresponding metadata object exists for those flags.

#### `min_flag_index` and `max_flag_index`
A `static constexpr` value of `size_t` indicating the bit index of the lowest and highest defined flags, respectively.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>,
   cobb::reflex::member<cobb::cs("B")>,
   cobb::reflex::member<cobb::cs("C")>,
   cobb::reflex::member<cobb::cs("D")>
>;

constexpr auto min = example::underlying_type(1) << example::min_flag_index;
constexpr auto max = example::underlying_type(1) << example::max_flag_index;

// min == the underlying value of A
// max == the underlying value of D
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

#### `underlying_value_range`
A `static constexpr underlying_type` member equal to `max_underlying_value - min_underlying_value + 1`. This value represents the full range of enum members' values, including any gaps between members.

#### `value_count`
A `static constexpr size_t` member indicating the number of defined members and their values within the enumeration.

```c++
using example = cobb::reflex::enumeration<
   cobb::reflex::member<cobb::cs("A")>, // == 0
   cobb::reflex::member<cobb::cs("B")>, // == 1
   cobb::reflex::member_range<cobb::cs("Range of Three"), 3> // == 2, 3, 4
>;

constexpr auto value_count = example::value_count; // == 5

// A (1) + B (1) + Range of Three (3)
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

### Instance members

#### `all_set<auto... Names>`
Templated on one or more flag names (`cobb::cs`). This function checks whether all of the given flags are set, returning a bool.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   member<cobb::cs("C")>, // 4
   member<cobb::cs("D"), 7> // 128
>;

constexpr auto value = example::from_int(130);
static_assert(value.all_set<cobb::cs("A"), cobb::cs("B")>() == false); // A is not set
```

#### `any_set<auto... Names>`
Templated on one or more flag names (`cobb::cs`). This function checks whether any of the given flags are set, returning a bool.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   member<cobb::cs("C")>, // 4
   member<cobb::cs("D"), 7> // 128
>;

constexpr auto value = example::from_int(130);
static_assert(value.any_set<cobb::cs("A"), cobb::cs("B")>() == true); // B is set
```

#### `flag<cobb::cs Name>`
Templated on a member name (`cobb::cs`). This function checks whether the enum has the given flag set, returning a bool.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   member<cobb::cs("C")>, // 4
   member<cobb::cs("D"), 7> // 128
>;

constexpr auto value = example::from_int(130);
static_assert(value.flag<cobb::cs("A")>() == false);
static_assert(value.flag<cobb::cs("B")>() == true);
static_assert(value.flag<cobb::cs("C")>() == false);
static_assert(value.flag<cobb::cs("D")>() == true);
```

#### `flag<cobb::cs Name, size_t Offset>`
Templated on a member name (`cobb::cs`). If the member name matches a range member, this function checks whether the enum has the given flag set, returning a bool.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   cobb::reflex::member_range<cobb::cs("C"), 4> // == 4, 8, 16, 32
>;

constexpr auto value = example::from_int(48);
static_assert(value.flag<cobb::cs("C"), 0>() == false);
static_assert(value.flag<cobb::cs("C"), 1>() == false);
static_assert(value.flag<cobb::cs("C"), 2>() == true);
static_assert(value.flag<cobb::cs("C"), 3>() == true);
```

#### `for_each_metadata`
Takes a lambda as an argument. Loops over each set bit in the value. If there is a metadata object defined for a set bit, this function invokes the lambda with a const reference to that metadata object as the argument.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A"), undefined, metadata{ .id = 5 }>,
   member<cobb::cs("B")>,
   member<cobb::cs("C"), undefined, metadata{ .id = 10 }>,
   member<cobb::cs("D"), 7>
>;

auto value = example::from<
   cobb::cs("A"),
   cobb::cs("B"),
   cobb::cs("C")
>;

int total      = 0;
int exec_count = 0;
value.for_each_metadata([&total](const metadata& md) {
   total += md.id;
});

assert(total == 15); // sum of both metadatas' IDs
assert(exec_count == 2); // the lambda ran only for the bits that have metadata
```

#### `for_each_name`
Takes a lambda as an argument. Loops over each set bit in the value. If there is a name associated with a set bit (i.e. the bit corresponds to a valid flag), then the lambda will be invoked with that name (as `const char*`) as an argument.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("1")>,
   member<cobb::cs("22")>,
   member<cobb::cs("333")>,
   member<cobb::cs("4444")>
>;

auto value = example::from<
   cobb::cs("A"),
   cobb::cs("B"),
   cobb::cs("C")
>;

int total      = 0;
int exec_count = 0;
value.for_each_name([&total](const char* name) {
   total += strlen(name);
});

assert(total == 6);
assert(exec_count == 3);
```

#### `modify_flag<cobb::cs Name>`
Templated on a member name (`cobb::cs`). This function sets or clears the specified flag.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   member<cobb::cs("C")>, // 4
   member<cobb::cs("D"), 7> // 128
>;

auto value = example::from_int(130);
value.modify_flag<cobb::cs("D")>(false); // == 2
```

#### `modify_flags<cobb::cs Name>`
Templated on one or more member names (`cobb::cs`). This function sets or clears the specified flags.

```c++
using example = cobb::reflex::flags_mask<
   member<cobb::cs("A")>, // 1
   member<cobb::cs("B")>, // 2
   member<cobb::cs("C")>, // 4
   member<cobb::cs("D"), 7> // 128
>;

auto value = example::from_int(0);
value.modify_flags<cobb::cs("A"), cobb::cs("D")>(true); // == 129
```

#### `to_int`
Returns the instance's current value in `underlying_type` form.