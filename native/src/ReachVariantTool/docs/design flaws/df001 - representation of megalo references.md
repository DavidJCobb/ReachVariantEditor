
# Representation of Megalo references

## Background

There are five kinds of values to which one can define a reference: integers, objects, players, teams, and timers. These can be split into two categories: numeric references (though MegaloEdit uses the term "custom") and handle references, the latter so named because Halo: Reach's engine generally uses handles rather than pointers for working with objects, players, and teams.


**Explicit handle references** are single values &mdash; enums, which indicate specific, globally-accessible handles; for example, `variables in Megalo.

```c++
enum class explicit_object_handle : int8_t;
enum class explicit_player_handle : int8_t;
enum class explicit_team_handle   : int8_t;
         
union explicit_handle_union {
   int8_t untyped = -1; // this is "none" across all types
            
   explicit_object_handle object;
   explicit_player_handle player;
   explicit_team_handle   team;
};
```

Handle reference operands in Megalo, then, resemble the following, using object handles as an example:

```c++
enum class object_reference_kind : uint8_t;
         
class object_reference {
   public:
      object_reference_kind  kind; // like a tagged union's tag
      explicit_handle_union  top_level;
      std::optional<uint8_t> member_index;
};
```

If the operand refers directly to an explicit object handle, then the <var>kind</var> enum will indicate this, and the <var>top_level</var> field will specify which handle is being referred to, and the <var>member_index</var> field will be absent. If the operand refers to an object that is the member of some other handle, then the <var>kind</var> enum will indicate that; the <var>top_level</var> field will specify the handle, and the <var>member_index</var> field will specify which object handle member is being referred to.

 Complicating matters slightly is the fact that properties are also represented in this system. If the operand is a reference to a player's biped, then this is indicated via the <var>kind</var>; the <var>member_of</var> and <var>target_index</var> fields hold the same values that they would for a `player_reference` referring to that same player.

<figure>
   <table>
      <thead>
         <tr>
            <th>RVT-syntax reference</th>
            <th><var>top_level</var></th>
            <th><var>member_index</var></th>
         </tr>
      </thead>
      <tbody>
         <tr>
            <td><code>no_object</code></td>
            <td>-1</td>
            <td></td>
         </tr>
         <tr>
            <td><code>global.object[<var>T</var>]</code></td>
            <td><var>T</var></td>
            <td></td>
         </tr>
         <tr>
            <td><code>global.object[<var>T</var>].object[<var>M</var>]</code></td>
            <td><var>T</var></td>
            <td><var>M</var></td>
         </tr>
         <tr>
            <td><code>current_player.object[<var>M</var>]</code></td>
            <td>24<sup>[note 1]</sup></td>
            <td><var>M</var></td>
         </tr>
         <tr>
            <td><code>temporaries.object[<var>T</var>]</code></td>
            <td>21 + <var>T</var></td>
            <td></td>
         </tr>
         <tr>
            <td><code>current_player.biped</code></td>
            <td>24<sup>[note 1]</sup></td>
            <td></td>
         </tr>
         <tr>
            <td><code>current_player.player[<var>M</var>].biped</code></td>
            <td>24<sup>[note 1]</sup></td>
            <td><var>M</var></td>
         </tr>
      </tbody>
   </table>
   <figcaption>
      <sup>[note 1:]</sup>  In this case, 24 is a value in the <code>explicit_player_handle</code> 
      enum, which corresponds to <code>current_player</code>.
   </figcaption>
</figure>


Numeric references are slightly different. First, I'll illustrate this with timers:

```c++
enum class timer_reference_kind : uint8_t;
         
class timer_reference {
   public:
      timer_reference_kind  kind; // like a tagged union's tag
      explicit_handle_union member_of;
      uint8_t               target_index;
};
```

If the operand is a reference to a built-in timer, like the game's round timer, then this is indicated via the <var>kind</var>; neither of the other fields are present in the serialized data. If the operand is a reference to a globally-scoped timer, then this is indicated via the <var>kind</var>, the <var>member_of</var> is absent from the serialized data, and the <var>target_index</var> specifies which timer is being used. If the timer is a member of some other object, then all three fields are used.

<figure>
   <table>
      <thead>
         <tr>
            <th>RVT-syntax reference</th>
            <th><var>member_of</var></th>
            <th><var>target_index</var></th>
         </tr>
      </thead>
      <tbody>
         <tr>
            <td><code>game.round_timer</code></td>
            <td></td>
            <td></td>
         </tr>
         <tr>
            <td><code>global.timer[<var>T</var>]</code></td>
            <td></td>
            <td><var>T</var></td>
         </tr>
         <tr>
            <td><code>global.object[<var>M</var>].object[<var>T</var>]</code></td>
            <td><var>M</var></td>
            <td><var>T</var></td>
         </tr>
         <tr>
            <td><code>current_player.timer[<var>T</var>]</code></td>
            <td>24<sup>[note 1]</sup> </td>
            <td><var>T</var></td>
         </tr>
      </tbody>
   </table>
   <figcaption>
      <sup>[note 1:]</sup>  In this case, 24 is a value in the <code>explicit_player_handle</code> 
      enum, which corresponds to <code>current_player</code>.
   </figcaption>
</figure>

Integer references are essentially the same, except that they can also refer to an integer constant &mdash; a signed 16-bit value:

```c++
enum class integer_reference_kind : uint8_t;
         
class integer_reference {
   public:
      integer_reference_kind kind; // like a tagged union's tag
      union {
         int16_t constant;
         struct {
            explicit_handle_union member_of;
            uint8_t               target_index;
         } variable;
      };
};
```

As with handles, integer property references on handle references (e.g. read-access to a player or team's score) are indicated by the <var>kind</var> enum; <var>member_of</var> and <var>target_index</var> hold the same values that their corresponding fields on handle references, <var>top_level</var> and <var>member_index</var>, would use to refer to the handle on which the property is being accessed. (Perhaps, then, it might even be a good idea to separate out the latter two fields into a struct, and add that as a union member in `integer_reference`?)


## Design flaw

ReachVariantTool does not make any distinction between numeric and handle references, because at the time I didn't realize there was any such distinction to make. All five reference types are handled uniformly. The components of a reference operand (called a "variable" operand in RVT) are classified based solely on their types, and not on their meanings. This is an unfortunate design flaw arising from an incomplete understanding of Megalo, and it makes ReachVariantTool's code for references harder to follow and reason about.

The <var>kind</var> field is in ReachVariantTool called the operand's "scope" value. The <var>top_level</var> field on handle references and the <var>member_of</var> field on numeric references are treated uniformly and called the "which" value. The <var>member_index</var> field on handle references and the <var>target_index</var> field on target references are treated uniformly and called the "index."

Scope-values are implemented as `VariableScopeIndicatorValue` instances stored in arrays (one for each reference type); instances effectively serve as annotations for the values of the <var>..._reference_kind</var> enums. The enum values are annotated with:

<table>
   <thead>
      <tr>
         <th>Field name</th>
         <th>Purpose</th>
      </tr>
   </thead>
   <tbody>
      <tr>
         <td>flags</td>
         <td>Flags, to track whether this "scope" represents read-only values like player bipeds or the values of Custom Game options, and whether the "scope" is used for references to a variable. (The latter flag is used by the compiler for things like not allowing you to write a variable declaration wherein the declared variable's initial value is another variable.)</td>
      </tr>
      <tr>
         <td>base</td>
         <td>The explicit handle reference's type, if any.</td>
      </tr>
      <tr>
         <td>index_type</td>
         <td>The index's type, if any; or an indicator that this is the "scope" value for an integer constant (represented as <code>enum index_type::generic</code>).</td>
      </tr>
      <tr>
         <td>index_bitcount</td>
         <td>The bitcount for the index. The index's bitcount can be inferred when the reference is to a Megalo number variable; however, when the reference is to some indexed data object, such as a Megalo-defined scoreboard stat or Custom Game option, we use this field to indicate the bitcount needed for the index. We also use this field to indicate the bitcount of integer constants.</td>
      </tr>
      <tr>
         <td>indexed_list_accessor</td>
         <td>
            <p>
               For references to indexed data objects, like Megalo-defined scoreboard stats or Custom Game options, this is a functor that can be used to get a type-erased pointer to the data object in memory, i.e. a pointer to the scoreboard stat definition or Custom Game option definition.
            </p>
            <p>
               ReachVariantTool allows users to reorder most indexed data options: Custom Game options, scoreboard stats, and similar are defined and edited through the GUI and can be rearranged. The naive approach to this would be to have all script operands store raw indices, and iterate through the entire script to update every operand whenever the user reorders something. The approach ReachVariantTool takes is to have these indexed data objects all inherit from a common base class that tracks its own index within its containing list (i.e. the <var>n</var>-th scoreboard stat <em>knows</em> that it is the <var>n</var>-th scoreboard stat), and have script operands store pointers (typed to the base class) to the indexed data objects. When we go to save the gametype to a file, the operand can simply ask the indexed data object for its index and then serialize that index.
            </p>
         </td>
      </tr>
   </tbody>
</table>

Which-values are implemented as `VariableScopeWhichValue` instances stored in arrays, with the instances having names and flags. Reference operands in memory don't hold pointers to these, however; only indices.

Indexes are implemented as simple integers. When the reference is to an indexed dataobject, they are paired with smart pointers that manage refcounts on the indexed data objects (another feature of the base class described above, used to allow end users to delete indexed data objects if they aren't in use by the script).