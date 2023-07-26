
# Mapping between script identifiers and compiled references

## Background

As described in DF-001, reference operands are represented in a suboptimal manner, with "scope," "which," and "index" values. There exists in the program lists of objects (the kinds of things that should've been `constexpr` data) containing metadata for the "scope" and "which" values for each type that has them. These are relied on for properly loading and saving compiled reference operands.

The compiler relies both on those "scope" and "which" metadata objects, and on metadata objects specific to the scripting language: these objects include:

* `Script::Namespace` serves as a container for `Script::NamespaceMember`, and also contains limited additional metadata.

* `Script::NamespaceMember` can wrap a scope value, a which value without a scope value, or a built-in enum definition. Scope values are used for "dead-end" references, like `game.round_timer`, while which values are used for explicit object handle references, like `current_player`.

* `Script::Enum` is the underlying type for built-in enums, and is also wrapped by user-defined enums.

* `Script::Property` is used to track properties available on handle references, like `player.score`.

* Operand types can, in general, "import names" that refer to their values. For example, the `_create_object_flags` type imports `never_garbage_collect`, `suppress_effect`, and `absolute_orientation` into the global scope as identifiers. The same name can be imported by multiple types without conflict; they act like context-dependent overloads.

Hardcoded logic is used to identify anything not in the above list. For example, when parsing `global.number[0]` as a reference operand, we first identify `global` as a namespace; checking its metadata, we see that it's allowed to contain variables, so we check whether `number` is a variable typename; seeing that it is, and seeing the index, we successfully identify the data being referenced. The behavior is similar for `current_player.player[0].biped`: once we've identified `current_player` as a "top-level" reference, we check whether `player` is a typename; seeing that it is, we resolve it as a nested variable and check whether `biped` is a property or an accessor. For `current_player.biped`, `biped` also undergoes the typename check, fails it, and is then tested as a property, and were it to fail that test as well, it'd be tested as an accessor.

Because we need to both compile and decompile, a sensible approach might have been to have a `constexpr` tree of identifiers defined for use by the compiler, and, for use by both the compiler and decompiler, a direct `constexpr` mapping between identifiers and reference operand data (possibly stored in some alternate manner to allow "fill-in-the-blank" logic, so that we don't need to manually predefine all of `global.number[0]`, `global.number[1]`, `global.number[2]`, etc.). The design would need to be a bit more advanced than this to account for nested references, but the basic idea would allow us to have a <a href="https://en.wikipedia.org/wiki/Single_source_of_truth" target="_blank">single source of truth</a> for controlling how identifiers map to compiled reference operands; the compiler and decompiler could work off of the same information.

## Design flaw

ReachVariantTool does not use a direct mapping between identifiers and reference operand data.

Script compiling was added onto the program after its initial release; originally, the goal was just to be able to load and save gametypes at all. In order to verify proper loading, all operands were given the ability to stringify their contents; for this purpose, the "scope" metadata objects contain format strings, and the "which" metadata objects contain names.

This system was not improved or replaced when the compiler was added. I suspect that at the time, I was tunnel-visioned on building the parser and compiler.

As such, the compiler uses a tree of identifiers (of varying types) in combination with hardcoded logic, to identify what a variable should compile to; this is stored in an intermediate form as a `Script::VariableReference`. When it comes time to actually compile the variable, an opcode of the right type is instantiated and is passed that `VariableReference`. The decompiler just uses the stringification functions already on the operand type classes &mdash; the format strings. This means that new reference operands (e.g. temporary variables) and renamed reference operands (e.g. `hud_player` to `local_player` in v2.2.0) must be specified in at least two places, and they must be kept in sync with each other:

* Somewhere in the compiler. This could be a `Namespace`, a `NamespaceMember`, or perhaps even both.

* The format strings for the "scope" and/or "which" metadata.