
# Handling of predefined strings ("variant string IDs")

Halo's \*.map files contain a tag named `msit`, which lists all of the *variant names* that an object can have. These allow a map developer to define a single object that comes in multiple forms. For example, standard Warthogs, Rocket Warthogs, and Gauss Warthogs all use the same baseline tag, and different variant names allow you to select which permutation of Warthog you wish to spawn.

Not all of the names in `msit` are used for object variant names, however. It's worth understanding that `msit` stands for "Megalo string ID table;" the collection of values available serves as a list of predefined strings referenceable by any Megalo operand that needs one. In particular, these **predefined strings** can be used as...

* Object variant names
* Animation names, to be provided to "device machines"
* HaloScript function names, now callable via the `hs_function_call` opcode backported from H4 to MCC!Reach

However, when I first built ReachVariantTool, I was unfamiliar with \*.map modding, and so I didn't understand the second use case; and at the time, the third use case didn't even exist. As such, I named the operand type "variant string ID" both under the hood and in documentation. This, in itself, is a minor mistake. However, I also made an additional mistake: I used identifier syntax.


## Problems with identifier syntax

This issue arose early: because I used identifier syntax, some predefined strings, like `35_spire_fp`, were impossible to reference in code: the parser would see `35` and treat that like an integer literal. The workaround I went with was to allow certain operand types to specify that their imported names (see DF-002) can begin with numbers, and to have the compiler backtrack as necessary to cope with this.

There is an additional problem: `none` is used in ReachVariantTool as an imported name to indicate the *absence* of a predefined string. However, the mid-July 2023 update to Halo: The Master Chief Collection added a predefined string whose contents actually are `"none"`. ReachVariantTool has to call it `none_variant`.

Using identifier syntax was a bad move. The better approach would've been to define a syntax like this: `P"35_spire_fp"`. This syntax imitates the typed prefixes available on string literals in C++, like `L"wide-character string"`; here, the `P` stands for "predefined." It also means that `none` and `P"none"` aren't (as) ambiguous.

Sadly, I'd had few occasions to ever use the C++ syntax at the time, and so I didn't think of this during the design phase. (If that sounds strange, it's worth considering my background: I started with JavaScript, and from there jumped directly to learning x86, disassembly, software reverse-engineering, and code injection. I learned the hardest parts of C++ programming first; then the easier parts; then C++ metaprogramming. I believe that's essentially the opposite order from everyone else, and I think I was roughly in the middle of that process when building ReachVariantTool.)


## Also, a note on object variant naming

The mid-July 2023 update to Halo: The Master Chief Collection added several new predefined strings. Several of them are used to configure specific vehicles, but have ambiguous names:

* auto
* flak_cannon
* grenade
* multiplayer
* no_side_turrets
* no_turrets
* plasma_cannon

If I were still handling "variant string IDs" the way I did in the past, then I might have given these some alternate names, for accessibility:

* shade_shielded
* shade_fuel_rod
* falcon_grenade
* falcon_mp
* no_side_turrets
* no_turrets
* shade_plasma

Here's the problem: if I rename them for one thing, then I rename them for everything. Remember that predefined strings can have three possible uses, and now consider: there's nothing stopping the same predefined string from being used in multiple ways. If there's a HaloScript function named `grenade`, and I renamed that value, then you'd have to use `falcon_grenade` to call it.