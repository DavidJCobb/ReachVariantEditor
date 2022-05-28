# Megalo variables

What we refer to as "variables" in the Megalo bytecode are encoded using up to three different values: the *register set*, the *which-value*, and the *index*.

## What is a "variable?"

Depending on the value type, a Megalo variable operand can refer to any of the following (with examples provided):

* An immediate constant
  * `5`
* An indexed script variable
  * A variable in the `global` namespace
    * `global.number[i]`
  * A member variable on an `object`, `player`, or `team`
    * `current_object.number[i]`
  * A static object
    * `player[i]`
    * `team[i]`
* An indexed data item in the game variant
  * `script_option[i]`
  * `player.script_stat[i]`
* A property
  * `player[0].biped`
  * `team[0].score`
* An engine-level data value
  * `game.betrayal_booting`
  * `game.grace_period_timer`

## Parts of a variable
### Register set

The first part of a variable reference is a *register set*. Each value type has its own enum of possible register sets. The way the above values are mapped to register sets is typically as follows:

* Immediate constants
  * Number variables only.
  * The constant value is stored in the index.
* Global-namespace variables
* Player-scoped variables
* Object-scoped variables
* Team-scoped variables
* Properties
  * Each property has its own register set.
  * Properties accessible on multiple context types (e.g. `player.score` and `team.score`) have a register set for each one.
  * Properties are only accessible on top-level variables unless a register set exists for nested variable access. The `biped` property is the only such property; it defines additional register sets for `global.player.biped`, `object.player.biped`, `player.player.biped`, and `team.player.biped`.
* Engine-level data values
  * Each such value has its own register set.

A variable reference's register set is what determines whether which-values and indices are present. As such, register set names generally follow the pattern of `context[w].value[i]`, where `w` indicates a which-value and `i` indicates an index.

#### Register set definitions

Each register set has a `register_type`:

<dl>
   <dt><code>engine_data</code></dt>
      <dd>
         The register set refers to engine-controlled data. This can be a static value (such as <code>game.betrayal_booting</code>) or a member of a variable (such as <code>object.spawn_sequence</code> or <code>player.score</code>).
      </dd>
   <dt><code>indexed_data</code></dt>
      <dd>
         The register set refers, by index, to data defined in the game variant. These can be static values (such as <code>script_option[n]</code>) or members on a variable (such as <code>player.script_stat[n]</code>).
      </dd>
   <dt><code>variable</code></dt>
      <dd>
         The register set refers to a script variable, which can be top-level or nested.
      </dd>
   <dt><code>immediate</code></dt>
      <dd>
         The register set refers to an immediate constant. This is only meaningful for numbers, wherein this register set is used to encode integer constants. The variable reference's *index* value is used to store the value.
      </dd>
</dl>

Register sets can optionally have any of the following fields:

<dl>
   <dt>bitcount</dt>
      <dd>
         For <code>immediate</code> register sets, this specifies the bitcount of the value. For all other register sets, this specifies the bitcount of the index, if one is present.
      </dd>
   <dt>flags</dt>
      <dd>
         A flags-mask.
         <dl>
            <dt>readonly</dt>
               <dd>Indicates that script instructions cannot modify the value via the register. It may be possible to modify the value through other means, such as a dedicated opcode.</dd>
         </dl>
      </dd>
   <dt>index_accessor</dt>
      <dd>
         For indexed data, this is a function pointer or non-capturing lambda which takes arguments <code>megalo_variant_data&</code> and <code>size_t index</code>. It should look up the indexed data item from the game variant data, and return a void pointer to the item. Specific variable types will cast and capture the result into a <code>util::refcount_ptr</code> as appropriate.
      </dd>
   <dt>scope</dt>
      <dd>
         An optional <code>variable_scope</code> constant. If present on a <code>variable</code> register set and equal to <code>variable_scope::global</code>, it indicates that the register set defines a global or static variable. If present and equal to any other value, it indicates that the register set accesses a variable or property that is a member of the given variable type. The value <code>variable_scope::global</code> is not valid for non-<code>variable</code> register sets.
      </dd>
   <dt>scope_inner</dt>
      <dd>
         An optional <code>variable_type</code> constant. If present on an <code>engine_data</code> register set, it works in conjunction with <code>scope</code> to define a property accessible on nested variables. This value is not valid for any other register type. If specified, this value is only valid if it has a matching value in the <code>variable_scope</code> enum. If specified, the index bitcount must be left unspecified, and will be computed from the <code>variable_type</code> constant as appropriate.
      </dd>
</dl>

<table>
   <tr>
      <th>Value</th>
      <th>Register set</th>
      <th>Variable scope(s)</th>
      <th>Register type</th>
      <th>Has which</th>
      <th>Has index</th>
   </tr>
   <tr>
      <td><code>game.betrayal_booting</code></td>
      <td><code>game.betrayal_booting</code></td>
      <td></td>
      <td>engine data</td>
      <td>No</td>
      <td>No</td>
   </tr>
   <tr>
      <td><code>global.object[1].spawn_sequence</code></td>
      <td><code>object[w].spawn_sequence</code></td>
      <td>object</td>
      <td>engine data</td>
      <td>No</td>
      <td>No</td>
   </tr>
   <tr>
      <td><code>global.team[3]</code></td>
      <td><code>global.team[i]</code></td>
      <td>global</td>
      <td>variable</td>
      <td>No</td>
      <td>Yes</td>
   </tr>
   <tr>
      <td><code>team[3]</code></td>
      <td><code>team[i]</code></td>
      <td>global</td>
      <td>variable</td>
      <td>No</td>
      <td>Yes</td>
   </tr>
   <tr>
      <td><code>current_object.team[1]</code></td>
      <td><code>object[w].team[i]</code></td>
      <td>object</td>
      <td>variable</td>
      <td>Yes</td>
      <td>Yes</td>
   </tr>
   <tr>
      <td><code>script_option[6]</code></td>
      <td><code>script_option[i]</code></td>
      <td></td>
      <td>indexed data</td>
      <td>No</td>
      <td>Yes</td>
   </tr>
   <tr>
      <td><code>current_object.script_stat[4]</code></td>
      <td><code>object[w].number[i]</code></td>
      <td>object</td>
      <td>indexed data</td>
      <td>Yes</td>
      <td>Yes</td>
   </tr>
   <tr>
      <td><code>current_player.biped</code></td>
      <td><code>player[w].biped</code></td>
      <td>player</td>
      <td>engine data</td>
      <td>Yes</td>
      <td>No</td>
   </tr>
   <tr>
      <td><code>current_object.player[0].biped</code></td>
      <td><code>object[w].player[i].biped</code></td>
      <td>object, player</td>
      <td>engine data</td>
      <td>Yes</td>
      <td>Yes</td>
   </tr>
</table>

### Which

When the register set defines a member on an `object`, `player`, or `team` &mdash; i.e. a value that exists in a `variable_scope` other than `variable_scope::global` &mdash; the *which-value* indicates what we're accessing a member of. As an example, the register set for `player[w].number[i]` has a which-value that indicates what player variable we're accessing a number member variable on. However, `global.number[i]` does not have a which-value, and neither does `script_option[i]`.

The which-value's bitcount depends on the register set's `variable_scope`.

### Index

When the register set defines an immediate constant or a member of some collection, the *index* indicates which variable we are accessing. As an example, `global.number[i]`, `script_option[i]`, `player[w].number[i]` have indices, but `game.teams_enabled` does not.

The index value's bitcount varies from register set to register set.