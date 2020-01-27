function _make_enum(list) {
   let out = {};
   for(let entry of list)
      out[entry] = Symbol(entry);
   return out;
}

/*

   TODO:
   
   Might be worth having an "is keyword" function; there are more than a few places 
   that need to check and disallow that. (Ctrl+F first to see which and how many 
   places.)
   
   ---------------------------------------------------------------------------------
   
   Currently, invalid alias names (e.g. "name[1]" or integer constants) fail during 
   first-pass parsing. Consider having them fail during second-pass parsing (i.e. 
   non-fatal errors, so other problems in the script can also be reported at the 
   same time) instead.
   
   A function name should fail second-pass parsing if it includes square brackets or 
   periods. (I'm referring here to function declarations, not function calls.)
   
   Strongly consider renaming all "validate" member functions to "analyze", since we 
   also need to do things like resolving MVariableReferences that include aliases, 
   and tracking what functions are in scope.
   
   If an alias targets a variable that doesn't exist, it should fail second-pass 
   parsing. If there is a function by the targeted name, the error message should 
   note that functions can't be aliased.
   
   Add an (owner) property to MVariableReference and have everything that contains 
   an instance set that property. We need this so that errors caught in second-stage 
   parsing can actually report line and column numbers; MVariableReference has no 
   positional information but if we can access an instance's owner, then we can know 
   roughly where the script error is.
   
   WE ARE CURRENTLY WORKING ON RESOLVING NON-ALIAS MVariableReferences.
   
    - Make it case-insensitive: (gLoBaL.nUmBeR[0]) should parse properly.
    
    = Can we reduce code duplication on the (non_nested_var.property) handlers?
    
    - Now that this is basically done, we need to work on resolving aliases within 
      MVariableReferences before then handling full resolution/analysis.
      
       - It has occurred to me: relative aliases can't be resolved unless we know 
         the type of the thing using them. Consider:
         
            alias is_zombie = player.number[0]
            global.player[0].is_zombie = 1
         
         We need to be able to tell that (global.player[0]) is a player, and that 
         only becomes known during variable resolution -- *after* we want to have 
         resolved aliases.
         
         Recall that every variable reference involves namespaces, vars, statics, 
         situationals, and properties. I think we should look into handling this 
         via OO: have a class representing namespaces and a class representing 
         types, and use members on these classes. So: MScriptNamespace, 
         MScriptNamespaceMember, and MScriptTypename.
         
          - MScriptNamespace: represents a namespace. There will be three instances: 
            "game", "global", and unnamed.
             
             - A boolean indicating whether this namespace is the "global" namespace 
               (for parsing purposes, this indicates whether the namespace can hold 
               variables).
          
             - A list of MScriptNamespaceMembers.
             
          - MScriptNamespaceMember: represents a built-in variable in a namespace, 
            such as "round_time_limit" in "game" or "current_player" in unnamed.
            
          - MScriptTypename members:
          
             - Is a variable?
             
                 - Yes for variable types; no for script_option, etc..
             
             - Can have nested variables?
             
                - Yes for player, object, and team; no for number and timer, and 
                  for all non-variable typenames.
             
             - Can be a static collection?
             
                - Yes for player and team (e.g. player[15], team[7]); no otherwise.
            
             - Underlying type, scope, and logic for adapting which and index.
             
                - This applies to non-variables. "Script option" for example is a 
                  non-variable with underlying type "number" which uses an index. 
                  A script stat is a non-variable with underlying type "number", 
                  which must exist as a member of a team or player (identified 
                  with which); the stat is identified via the index.
         
         Once we've taken this approach, variable *and* alias resolution become 
         simpler:
         
          - If an alias begins with a typename, then it's a relative alias. If we 
            do it the OO way it becomes easier to allow more qualified relative 
            aliases (e.g. "alias flag_points = player.object[0].number[0]").
         
          - Variable resolution would involve identifying the namespace and then 
            matching all remaining parts to their typenames. When we know the 
            previous part's typename, we can check whether the current part is a 
            valid relative alias on that typename.
            
             - Absolute aliases must be singular; given (alias foo = global.number[0]) 
               you cannot do (foo.bar) nor (bar.foo) nor (foo[0]).
   
   NOTE: We don't have a way to specify event triggers e.g. object death events. I 
   think we should do that with an "on" keyword (only permitted at the top level) 
   and a colon, e.g.
   
      on object death: for each player do
         -- ...
      end
   
   ---------------------------------------------------------------------------------
   
   REMINDER: The second stage of parsing entails:
    
    - Making sure that MVariableReferences all point to valid variables (accounting 
      for aliases).
   
    - Making sure that assignments make sense: do not allow assigning to a read-only 
      variable.
      
       = DONE: Don't allow assigning to a constant.
      
    - Making sure that function calls and property access make sense in the context 
      of all Megalo opcodes and their script syntax. (We'll need to bring in the 
      opcode list from C++ and attach data to normalize it to script.)
      
       - An opcode should be able to map the C++ argument order to script.
       
       - If an opcode uses function syntax, it should be able to specify an argument 
         (by C++ index) as the context (the "this") for the function call.
      
       - If an opcode has an out-argument, then it gets assignment semantics; we 
         need to validate assignment statements with this opcode to ensure that the 
         variable that the script wants to assign to is of the right type.
   
    - Making sure that all function calls are to valid opcodes or user-defined 
      functions.
      
    - Making sure that all "expect" declarations apply to valid variable names, and 
      that if their righthand side is an MVariableReference, it points to an alias 
      of a constant integer.

*/

const block_type = _make_enum([
   "basic",
   "root",
   "if",
   "elseif",
   "else",
   "for",
   "for_each_object",
   "for_each_object_with_label",
   "for_each_player",
   "for_each_player_randomly",
   "for_each_team",
   "function",
]);
const var_scope = _make_enum([
   "global",
   "object",
   "player",
   "team",
]);
const var_type = _make_enum([
   "number",
   "object",
   "player",
   "team",
   "timer",
]);
function var_type_to_var_scope(type) {
   switch (type) {
      case var_type.number: return null;
      case var_type.object: return var_scope.object;
      case var_type.player: return var_scope.player;
      case var_type.team:   return var_scope.team;
      case var_type.timer:  return null;
   }
   return null;
}
//
const number_var_scope = _make_enum([
   "constant",
   "player_number",
   "object_number",
   "team_number",
   "global_number",
   "script_option",
   "spawn_sequence",
   "team_score",
   "player_score",
   "unknown_09", // unused in Reach? used in Halo 4?
   "player_rating",
   "player_stat",
   "team_stat",
   "current_round",
   "symmetry_getter",
   "symmetry",
   "score_to_win",
   "unkF7A6",
   "teams_enabled",
   "round_time_limit",
   "round_limit",
   "misc_unk0_bit3",
   "rounds_to_win",
   "sudden_death_time",
   "grace_period",
   "lives_per_round",
   "team_lives_per_round",
   "respawn_time",
   "suicide_penalty",
   "betrayal_penalty",
   "respawn_growth",
   "loadout_cam_time",
   "respawn_traits_duration",
   "friendly_fire",
   "betrayal_booting",
   "social_flags_bit_2",
   "social_flags_bit_3",
   "social_flags_bit_4",
   "grenades_on_map",
   "indestructible_vehicles",
   "powerup_duration_red",
   "powerup_duration_blue",
   "powerup_duration_yellow",
   "death_event_damage_type",
]);
function var_scope_to_number_scope(vs) {
   switch (vs) {
      case var_scope.global: return number_var_scope.global_number;
      case var_scope.object: return number_var_scope.object_number;
      case var_scope.player: return number_var_scope.player_number;
      case var_scope.team:   return number_var_scope.team_number;
   }
   return null;
}
//
const object_var_scope = _make_enum([
   "global",
   "player",
   "object",
   "team",
   "global_player_biped", // KSoft.Tool calls these "player_slave"
   "player_player_biped",
   "object_player_biped",
   "team_player_biped",
]);
function var_scope_to_object_scope(vs) {
   switch (vs) {
      case var_scope.global: return object_var_scope.global;
      case var_scope.object: return object_var_scope.object;
      case var_scope.player: return object_var_scope.player;
      case var_scope.team:   return object_var_scope.team;
   }
   return null;
}
function var_scope_to_biped_scope(vs) {
   switch (vs) {
      case var_scope.global: return object_var_scope.global_player_biped;
      case var_scope.object: return object_var_scope.object_player_biped;
      case var_scope.player: return object_var_scope.player_player_biped;
      case var_scope.team:   return object_var_scope.team_player_biped;
   }
   return null;
}
//
const team_var_scope = _make_enum([
   "global",
   "player",
   "object",
   "team",
   "object_owner_team",
   "player_owner_team",
]);
function var_scope_to_team_scope(vs) {
   switch (vs) {
      case var_scope.global: return team_var_scope.global;
      case var_scope.object: return team_var_scope.object;
      case var_scope.player: return team_var_scope.player;
      case var_scope.team:   return team_var_scope.team;
   }
   return null;
}
//
const timer_var_scope = _make_enum([
   "global",
   "player",
   "team",
   "object",
   "object_owner_team",
   "player_owner_team",
   "round_timer",
   "sudden_death_timer",
   "grace_period_timer",
]);

// currently in variables_and_scopes.cpp. need to add real enums alongside these -- messy. :(
const megalo_objects = _make_enum([
   "no object",
   "Global.Object[0]",
   "Global.Object[1]",
   "Global.Object[2]",
   "Global.Object[3]",
   "Global.Object[4]",
   "Global.Object[5]",
   "Global.Object[6]",
   "Global.Object[7]",
   "Global.Object[8]",
   "Global.Object[9]",
   "Global.Object[10]",
   "Global.Object[11]",
   "Global.Object[12]",
   "Global.Object[13]",
   "Global.Object[14]",
   "Global.Object[15]",
   "current object",
   "HUD target object",
   "killed object",
   "killer object",
]);
const megalo_players = _make_enum([
   "no player",
   "Player 1",
   "Player 2",
   "Player 3",
   "Player 4",
   "Player 5",
   "Player 6",
   "Player 7",
   "Player 8",
   "Player 9",
   "Player 10",
   "Player 11",
   "Player 12",
   "Player 13",
   "Player 14",
   "Player 15",
   "Player 16",
   "Global.Player[0]",
   "Global.Player[1]",
   "Global.Player[2]",
   "Global.Player[3]",
   "Global.Player[4]",
   "Global.Player[5]",
   "Global.Player[6]",
   "Global.Player[7]",
   "current player",
   "HUD player",
   "HUD target",
   "killer player",
]);
const megalo_teams = _make_enum([
   "no team",
   "Team 1",
   "Team 2",
   "Team 3",
   "Team 4",
   "Team 5",
   "Team 6",
   "Team 7",
   "Team 8",
   "Neutral",
   "Global.Team[0]",
   "Global.Team[1]",
   "Global.Team[2]",
   "Global.Team[3]",
   "Global.Team[4]",
   "Global.Team[5]",
   "Global.Team[6]",
   "Global.Team[7]",
   "current team",
   "HUD player's team",
   "HUD target's team",
   "unk_14 team",
   "unk_15 team",
]);

function is_operator_char(c) {
   return ("=<>!+-*/%&|~^").indexOf(c) >= 0;
}
function is_quote_char(c) {
   return ("`'\"").indexOf(c) >= 0;
}
function is_syntax_char(c) {
   return ("(),").indexOf(c) >= 0;
}
function is_whitespace_char(c) {
   return (" \r\n\t").indexOf(c) >= 0;
}

class MParsedItem {
   constructor() {
      this.parent = null;
      this.owner  = null; // for items that appear in conditions, etc..
      this.line   = -1;
      this.col    = -1;
      this.range  = [-1, -1];
   }
   set_start(state) {
      if (!state) {
         console.warn("MParsedItem::set_start called with no incoming state.");
         return;
      }
      this.line     = state.line;
      this.col      = state.pos - state.last_newline;
      this.range[0] = state.pos;
   }
   set_end(pos) {
      this.range[1] = pos;
   }
   validate(validator) {
   }
}

class MAlias extends MParsedItem {
   constructor(n, rhs) {
      super();
      this.name   = n;
      this.value  = 0;    // integer
      this.target = null; // MVariableReference
      {  // Validate name.
         //
         // TODO: Do we really want this to be a fatal parse error? Seems like something we could 
         // just catch during the second-pass parsing.
         //
         if (n.match(/[\[\]\.]/))
            throw new Error(`Invalid alias name "${n}". Alias names cannot contain square brackets or periods.`);
         if (!n.match(/[^0-9]/))
            throw new Error(`Invalid alias name "${n}". You cannot alias an integer constant.`);
      }
      let result = token_to_int_or_var(rhs);
      if (result instanceof MVariableReference) {
         this.target = result;
         result.owner = this;
      } else
         this.value = result;
   }
   serialize() {
      let result = `alias ${this.name} = `;
      if (this.target) {
         result += this.target.serialize();
      } else {
         result += this.value;
      }
      return result;
   }
   validate(validator) {
      if (this.target)
         this.target.validate(validator, true);
   }
}
class MBlock extends MParsedItem {
   constructor() {
      super();
      this.items       = [];   // statements and blocks
      this.type        = block_type.root;
      this.conditions  = [];   // only for if- and elseif-blocks
      this.forge_label = null; // only for for-each-object-with-label blocks
      this.name        = "";   // only for function blocks
   }
   insert_condition(c) {
      if (this.type != block_type.if && this.type != block_type.elseif)
         throw new Error("This block type cannot have conditions.");
      let is_code = (c instanceof MFunctionCall) || (c instanceof MComparison);
      let is_join = (c instanceof MConditionJoiner);
      if (!is_code && !is_join)
         throw new Error("Not a condition or condition-joiner.");
      if (is_code) {
         if (this.conditions.length) {
            let last = this.conditions[this.conditions.length - 1];
            if (!(last instanceof MConditionJoiner))
               throw new Error(`When using multiple conditions in a single if-block, you must link them with the "or" and "and" keywords.`);
         }
      } else if (is_join) {
         if (!this.conditions.length)
            throw new Error(`The "or" and "and" keywords must come after a condition.`);
         //
         // Check for ("if a == b or or c"). In practice this check shouldn't be necessary -- a 
         // mistake like that would trip a syntax error before reaching this code because code 
         // like "word word" is always invalid -- but I prefer to be explicit about this.
         //
         let last = this.conditions[this.conditions.length - 1];
         if (last instanceof MConditionJoiner)
            throw new Error(`The "or" and "and" keywords must come after a condition.`);
      }
      this.conditions.push(c);
      c.owner = this;
   }
   insert_item(i) {
      this.items.push(i);
      i.parent = this;
      return i;
   }
   item(i) {
      if (i < 0)
         return this.items[this.items.length + i];
      return this.items[i];
   }
   serialize(indent) {
      if (!indent)
         indent = "";
      let text = indent;
      if (this.type != block_type.root) {
         switch (this.type) {
            case block_type.basic:
               text += "do";
               break;
            case block_type.for_each_object:
               text += "for each object do";
               break;
            case block_type.for_each_object_with_label:
               text += "for each object with label ";
               if (this.forge_label + "" === this.forge_label)
                  text += '"' + this.forge_label + '"';
               else
                  text += this.forge_label;
               text += " do";
               break;
            case block_type.for_each_player:
               text += "for each player do";
               break;
            case block_type.for_each_player_randomly:
               text += "for each player randomly do";
               break;
            case block_type.for_each_team:
               text += "for each team do";
               break;
            case block_type.function:
               text += `function ${this.name}()`;
               break;
            case block_type.if:
               text += "if ";
               for(let i = 0; i < this.conditions.length; i++) {
                  if (i > 0)
                     text += " ";
                  text += this.conditions[i].serialize();
               }
               text += " then";
               break;
            case block_type.else:
               text += "else";
               break;
            case block_type.elseif:
               text += "elseif ";
               for(let i = 0; i < this.conditions.length; i++) {
                  if (i > 0)
                     text += " ";
                  text += this.conditions[i].serialize();
               }
               text += " then";
               break;
         }
         text += "\n";
         indent += "   ";
      }
      for(let i = 0; i < this.items.length; i++) {
         let item = this.items[i];
         if (item instanceof MBlock) {
            text += `${item.serialize(indent)}`;
         } else {
            text += `${indent}${item.serialize()}`;
         }
         if (i + 1 < this.items.length)
            text += "\n";
      }
      if (this.type != block_type.root) {
         indent = indent.substring(3);
         text += `\n${indent}end`;
      }
      return text;
   }
   validate(validator) {
      for(let condition of this.conditions)
         condition.validate(validator);
      //
      // TODO: We need to keep track of what aliases are in-scope as we iterate through 
      // items, so that MVariableReferences can track that properly; we also need to 
      // keep track of what functions have been defined. The parser doesn't forbid 
      // nesting functions under blocks at this time, and yeah, I think I'd like to 
      // keep block-scoped functions even if there isn't much use for them.
      //
      // I'm not sure we need to construct that data in advance; instead, the validator 
      // could potentially handle the tracking for that, I think.
      //
      let aliases = [];
      for(let item of this.items) {
         if (item instanceof MAlias) {
            aliases.push(item);
            validator.trackAlias(item);
         }
         item.validate(validator);
      }
      validator.untrackAliases(aliases);
   }
}
class MConditionJoiner extends MParsedItem {
   constructor() {
      super();
      this.is_or = false;
   }
   serialize() {
      return this.is_or ? "or" : "and";
   }
}
class MExpect extends MParsedItem {
   constructor(n, v) {
      super();
      this.name  = n;
      this.value = v;
      //
      let lhs = token_to_int_or_var(n);
      if (!(lhs instanceof MVariableReference))
         throw new Error("Expect declarations must apply to a variable.");
      this.name = lhs;
      lhs.owner = this;
      //
      let rhs = token_to_int_or_var(v);
      this.value = rhs; // integer or MVariableReference
      if (rhs instanceof MParsedItem || rhs instanceof MVariableReference)
         rhs.owner = this;
   }
   serialize() {
      let result = `expect ${this.name.serialize()} == `;
      if (this.value instanceof MVariableReference)
         result += this.value.serialize();
      else if (+this.value === this.value)
         result += this.value;
      else
         throw new Error("unimplemented");
      return result;
   }
   validate(validator) {
      this.name.validate(validator);
      if (this.value instanceof MParsedItem)
         this.value.validate(validator);
   }
}
class MStringLiteral extends MParsedItem {
   constructor(text) {
      super();
      this.text = text;
   }
   serialize() {
      return '"' + this.text + '"';
   }
}

class MSituationalVariableDefinition {
   //
   // Used during variable resolution to quickly handle things like "no_team" and 
   // "current_player".
   //
   constructor(name, type, is_none) {
      this.name    = name;
      this.type    = type;
      this.is_none = is_none;
   }
}
const situational_variables = [
   new MSituationalVariableDefinition("no_object", var_type.object, true),
   new MSituationalVariableDefinition("no_player", var_type.player, true),
   new MSituationalVariableDefinition("no_team",   var_type.team,   true),
   new MSituationalVariableDefinition("current_object", var_type.object, false),
   new MSituationalVariableDefinition("current_player", var_type.player, false),
   new MSituationalVariableDefinition("current_team",   var_type.team,   false),
   new MSituationalVariableDefinition("hud_player",        var_type.player, false),
   new MSituationalVariableDefinition("hud_target_object", var_type.object, false), // biped?
   new MSituationalVariableDefinition("hud_target_player", var_type.player, false),
   new MSituationalVariableDefinition("killed_object", var_type.object, false),
   new MSituationalVariableDefinition("killer_object", var_type.object, false),
   new MSituationalVariableDefinition("killer_player", var_type.player, false),
   new MSituationalVariableDefinition("neutral_team",  var_type.team, false),
];
class MVariablePart {
   constructor(name, index) {
      this.name  = name || "";
      this.index = -1; // NOTE: string literals allowed, since aliases can be constant ints
      if (index !== void 0 && index !== "" && index !== null) {
         if (!isNaN(+index))
            this.index = +index;
         else
            this.index = index;
      }
      this.owner = null;
      this.analyzed = {
         archetype: null, // "namespace", "static", "var", "member", "property", etc.
         object:    null, // namespace: MScriptNamespace; member: MScriptNamespaceMember; else MScriptTypename
      };
   }
   //
   locate_me() {
      if (!this.owner)
         return -1;
      let parts = this.owner.parts;
      let size  = parts.length;
      for(let i = 0; i < size; i++)
         if (parts[i] == this)
            return i;
      return -1;
   }
   //
   has_index() {
      if (+this.index === this.index)
         return this.index >= 0;
      return !!this.index;
   }
   serialize() {
      if (+this.index === this.index && this.index < 0) {
         return this.name;
      }
      return `${this.name}[${this.index}]`;
   }
   validate_syntax() {
      //
      // Validation during stage-one parsing.
      //
      if (!this.name.length)
         throw new Error("Invalid variable name. Did you accidentally type two periods instead of one?");
      switch (this.name) {
         case "alias":
         case "and":
         case "do":
         case "else":
         case "elseif":
         case "end":
         case "expect":
         case "for":
         case "function":
         case "if":
         case "or":
            throw new Error("Keywords cannot be used as variable names.");
      }
      switch (this.index) {
         case "alias":
         case "and":
         case "do":
         case "else":
         case "elseif":
         case "end":
         case "expect":
         case "for":
         case "function":
         case "if":
         case "or":
            throw new Error("Keywords cannot be used as indices in a collection.");
      }
   }
   //
   validate(validator) {
      //
      // Validation during stage-two parsing.
      //
      let i     = this.locate_me();
      let name  = this.name;
      let match = null;
      if (i == 0) { // my index within my containing MVariableReference
         match = namespaces.find(function(e) { return e.name.length && e.name == name; });
         if (match) {
            this.analyzed.archetype = "namespace";
            this.analyzed.object    = match;
            return true;
         }
         match = types.find(function(e) { return e.is_static && e.name == name; });
         if (match) {
            this.analyzed.archetype = "static";
            this.analyzed.object    = match;
            if (!this.has_index()) {
               validator.report(this, `Must specify an index; you need to indicate which static ${match.name} you want.`);
               return false;
            }
            return true;
         }
         match = namespaces.unnamed.members.find(function(e) { return e.name == name });
         if (match) {
            this.analyzed.archetype = "member";
            this.analyzed.object    = match;
            if (match.hard_max_index == 0) {
               if (this.has_index()) {
                  validator.report(this, `An index is not valid here.`);
                  return false;
               }
            } else {
               if (!this.has_index()) {
                  validator.report(this, `Must specify an index.`);
                  return false;
               }
            }
            return true;
         }
         validator.report(this, `"${name}" is not a namespace or static-type.`);
         return false;
      }
      let prev = this.owner.parts[i - 1];
      let pObj = prev.analyzed.object;
      assert(prev, "There should be a previous part."); // JS only
      switch (prev.analyzed.archetype) {
         case "namespace": { // we can be a member or a var
            //
            // Check for namespace.member:
            //
            match = pObj.members.find(function(e) { return e.name == name });
            if (match) {
               this.analyzed.archetype = "member";
               this.analyzed.object    = match;
               if (member.hard_max_index == 0) {
                  if (this.has_index()) {
                     validator.report(this, `An index is not valid here.`);
                     return false;
                  }
               } else {
                  if (!this.has_index()) {
                     validator.report(this, `Must specify an index.`);
                     return false;
                  }
               }
               return true;
            }
            //
            // Check for namespace.var:
            //
            match = types.find(function(e) { return e.is_variable && e.name == name; });
            if (match) {
               this.analyzed.archetype = "var";
               this.analyzed.object    = match;
               if (!this.has_index()) {
                  validator.report(this, `Must specify an index; you need to indicate which ${match.name} variable you want.`);
                  return false;
               }
               return true;
            }
            validator.report(this, `The "${pObj.name}" namespace does not have a field named "${name}".`);
            return false;
         } break;
         case "member": { // TODO: we can be a property or a var
            let type = pObj.type;
            //
            // TODO: check for and handle properties here
            //
            if (type.is_nestable) {
               match = types.find(function(e) { return e.is_variable && e.name == name; });
               if (match) {
                  this.analyzed.archetype = "var";
                  this.analyzed.object    = match;
                  if (!this.has_index()) {
                     validator.report(this, `Must specify an index; you need to indicate which ${match.name} variable you want.`);
                     return false;
                  }
                  return true;
               }
            }
            validator.report(this, `Objects of type ${type.name} do not have a field named "${name}".`);
            return false;
         } break;
         case "static": { // TODO: we can be a property or a var
            //
            // TODO: check for and handle properties here
            //
            match = types.find(function(e) { return e.is_variable && e.name == name; });
            if (match) {
               this.analyzed.archetype = "var";
               this.analyzed.object    = match;
               if (!this.has_index()) {
                  validator.report(this, `Must specify an index; you need to indicate which ${match.name} variable you want.`);
                  return false;
               }
               return true;
            }
            validator.report(this, `Objects of type ${pObj.name} do not have a field named "${name}".`);
            return false;
         } break;
         case "var": { // TODO: we can be a property or, in some cases, a var
            //
            // Most properties are available on (namespace.var) but not (namespace.var.var); moreover, 
            // (namespace.var.var.var) is not allowed so we need to know whether the previous var is 
            // itself preceded by a var.
            //
            let is_second_var = i == 3;
            //
            // TODO: check for and handle properties here
            //
            match = types.find(function(e) { return e.is_variable && e.name == name; });
            if (match) {
               if (is_second_var) {
                  validator.report(this, `Constructions of the form (namespace.var.var.var) are not allowed; store (namespace.var.var) in a variable and then access through that variable.`);
                  return false;
               }
               this.analyzed.archetype = "var";
               this.analyzed.object    = match;
               if (!this.has_index()) {
                  validator.report(this, `Must specify an index; you need to indicate which ${match.name} variable you want.`);
                  return false;
               }
               return true;
            }
         } break;
         case "property": {
            validator.report(this, `You cannot access fields on properties like ${pObj.name}. Copy it to a variable and then access through that.`);
            return false;
         } break;
         default: assert(false, `Bad or unknown archetype ${prev.analyzed.archetype}.`);
      }
      return true;
   }
}
class MVariableReference { // represents a variable, keyword, or aliased integer
   constructor(text) {
      this.parts = []; // std::vector<MVariablePart>
      if (text)
         this.extract(text);
      this.analyzed = {
         type:  null,
         scope: null, // var_scope OR the _scope enums on the opcode arg value classes
         which: null,
         index: null,
         aliased_integer_constant: null, // if we turn out to be an alias
         is_read_only: false,
         //
         aliased_typename: null, // for relative aliases of the form typename.member[index]
         aliased_member:   null,
         aliased_index:    null,
      };
   }
   extract(text) {
      let size     = text.length;
      let name     = "";
      let index    = "";
      let is_index = false;
      //
      const BAD_INDEX_CHARS = "[.=!<>+-*/%&|~^";
      //
      for(let i = 0; i < size; i++) {
         let c = text[i];
         if (is_index) {
            if (BAD_INDEX_CHARS.indexOf(c) >= 0)
               throw new Error(`Character ${c} is not valid inside of an index.`);
            if (c == "]") {
               is_index = false;
               if (index === "") // "name[]" is a syntax error
                  throw new Error(`Variables of the form "name[]" are not valid. Specify an index if appropriate, or no square brackets at all otherwise.`);
               continue;
            }
            index += c;
         } else {
            if (c == "[") {
               is_index = true;
               continue;
            }
            if (c == ".") {
               let part = new MVariablePart(name, index);
               part.owner = this;
               this.parts.push(part);
               name  = "";
               index = "";
               continue;
            }
            name += c;
         }
      }
      if (!name) // (text) ended in "." or was empty string
         throw new Error(`Variables cannot end in ".", and cannot be nameless.`);
      let part = new MVariablePart(name, index);
      part.owner = this;
      this.parts.push(part);
      //
      for(let i = 0; i < this.parts.length; i++)
         this.parts[i].validate_syntax();
      return true;
   }
   serialize() {
      let list = this.parts.map(function(v) { return v.serialize(); });
      return list.join(".");
   }
   toString() {
      return this.serialize();
   }
   //
   is_read_only() {
      if (this.analyzed.aliased_integer_constant !== null)
         return true;
      if (this.analyzed.type)
         return this.analyzed.is_read_only;
      return void 0; // analysis was not yet conducted, so we don't know
   }
   is_single() {
      return this.parts.length == 1 && !this.parts[0].has_index();
   }
   //
   resolve_aliases(validator) {
      //
      // TODO: If there is an alias anywhere in our (parts), convert it to its 
      // "real" value.
      //
      //  - If you alias (scope.variable[index]), then (scope[index].alias) is a 
      //    valid invocation of the alias as long as that scope can be indexed.
      //
      //  - Currently we want to be as broad with aliases as possible, e.g. 
      //    allowing the aliasing of scopes and of built-in property names. It 
      //    could be useful as a localization aid (picture a foreign-language 
      //    developer pasting a bunch of aliases at the top of the script to 
      //    translate as many names as possible to their language).
      //
      // I think the best way to do this is to:
      //
      //  - Resolve the MVariableReferences in MAliases.
      //
      //     - Modify the MVariableReference resolve function to take an argument 
      //       indicating whether it's being called for an alias. If so, have it 
      //       check whether the alias is relative or absolute and if it's relative, 
      //       resolve that with custom logic and return.
      //
      //  - A "relative" alias has as its target (type.member[index]) such that it 
      //    can be invoked as a member of any variable of that type. For example, 
      //    (alias is_zombie = player.number[0]) is a relative alias and can be 
      //    invoked as (current_player.is_zombie), (global.player[0].is_zombie), 
      //    et cetera.
      //
      //    An "absolute" alias refers to a fully-qualified variable.
      //
      //  - When all in-scope aliases have been resolved, resolving variable 
      //    references that may use aliases is simple: simply copy the alias's 
      //    own resolved variable reference (if it's absolute) or construct an 
      //    appropriate new reference from its data (if it's relative).
      //
   }
   //
   validate(validator, is_alias_rhs) {
      for(let i = 0; i < this.parts.length; i++) {
         let result = this.parts[i].validate(validator);
         if (!result)
            return false;
      }
      return true;
      
      
      {  // new logic
         let index     = 0;
         let item      = this.parts[index];
         let namespace = namespaces.unnamed;
         let type      = null;
         let read_only = false;
         {  // Check for named namespaces
            let match = namespaces.find(function(e) { return e.name.length && e.name == item.name; });
            if (match) {
               namespace = match;
               item      = this.parts[++index];
               if (!item) {
                  validator.report(this, `A namespace such as "${namespace.name}" cannot be used as a variable.`);
                  return false;
               }
            }
         }
         if (namespace.can_have_statics) {  // Check for statics and handle them (e.g. player[0], team[1])
            let match = types.find(function(e) { return e.is_static && e.name == item.name; });
            if (match) {
               type = match;
               if (!item.has_index()) {
                  validator.report(this, `You must specify which ${item.name} you mean.`);
                  return false;
               }
               if (type.static_limit && item.index >= type.static_limit) {
                  validator.report(this, `Index out of bounds. There are only ${type.static_limit} ${type.name}s.`);
                  return false;
               }
               item = this.parts[++index];
               //
               // Fall through.
               //
            }
         }
         if (!type) { // we know we're not a static type, so test for namespace members
            let member = namespace.members.find(function(e) { return e.name == item.name; });
            if (member) {
               type      = m.type;
               member    = m;
               read_only = m.is_read_only;
               if (m.type && m.type.always_read_only)
                  read_only = true;
               //
               // TODO
               //
               item = this.parts[++index];
               //
               if (member.is_none && item) {
                  validator.report(this, `Cannot access properties or variables on a none value`);
                  return false;
               }
               if (type && !type.is_nestable && !type.properties.length) {
                  validator.report(this, `Cannot access properties or variables on type "${type.name}".`);
                  return false;
               }
            } else {
               if (namespace != namespaces.unnamed) {
                  if (!namespace.can_have_variables) {
                     validator.report(this, `The "${namespace.name}" namespace does not have a property or variable named "${item.name}".`);
                     return false;
                  }
               }
            }
         }
         //
         // TODO:
         // member.var
         // member.property
         // namespace.var
         // namespace.var.property
         // namespace.var.var
         // namespace.var.var.property ("biped" only)
         // static.var
         // static.property
         //
         return true;
      }
      
      
      
      this.resolve_aliases(validator);
      if (this.analyzed.aliased_integer_constant !== null) // if we're actually an integer
         return true;
      if (is_alias_rhs) {
         //
         // TODO: See comments in (resolve_aliases): we want to use this function (the function 
         // that this very comment is inside of) to use custom logic right ***here*** when the 
         // MVariableReference being resolved is the target for an MAlias.
         //
      }
      //
      // There are sixty-one possible variables not accounting for indices (i.e. counting 
      // player[0] and player[1] as "the same" variable). However, all such variables fall 
      // into one of the following patterns:
      // 
      //    situational                 // e.g. current_player; killed_object    // global-scoped under the hood
      //    situational.var             // e.g. current_player.number[0]         // 
      //    namespace.var               // e.g. global.object[0]                 // the only valid namespace for variables is "player"
      //    namespace.var.var           // e.g. global.object[0].player[0]       // this doesn't nest further, i.e. there is no (namespace.var.var.var), though (namespace.var.var.property) may exist as noted in this list
      //    static                      // e.g. player[0]                        // global-scoped under the hood; you can have static players or teams
      //    static.var                  // e.g. player[0].object[0]              // 
      //    global_object.property      // e.g. current_object.spawn_sequence    // any non-nested object e.g. situational, static, global.object
      //    global_player.property      // e.g. current_player.team              // any non-nested player e.g. situational, static, global.player
      //    any_player.biped            // e.g. global.object[0].player[0].biped // any player e.g. situational, static, global.var, global.var.var
      //    indexed_data                // e.g. script_option[0]                 // options, traits, and stats
      //
      // There are two additional patterns that can appear in code as well:
      //
      //    game.state_value            // e.g. game.round_limit                 // 
      //    function_argument_enum      // e.g. basis.create_object(flag, ...)   // MVariableReferences that are function call args need special handling; MVariableReference::is_single may help
      // 
      // As such, if we handle the above patterns, then we handle all variables. Note that 
      // throughout this function, we will be using the term "property" to refer to non-
      // variable fields accessible on variables: biped, spawn_sequence, team, and so on.
      //
      let index = 0;
      let item  = this.parts[index];
      if (item.name == "global") { // namespace.var or namespace.var.var or namespace.var.property or namespace.var.var.property
         if (item.has_index()) {
            validator.report(this, `Namespaces such as "global" cannot be indexed.`);
            return false;
         }
         item = this.parts[++index];
         if (!item) {
            validator.report(this, `Namespaces such as "global" cannot be accessed directly.`);
            return false;
         }
         switch (item.name) {
            case "number":
            case "timer": // TODO: change syntax (timer.rate = ...) to (timer.set_rate(...)) so that the syntax for timer rate is consistent with other setters
               if (this.parts.length > (index + 1)) {
                  validator.report(this, `Variables of type "${item.name}" cannot contain properties or other variables.`);
                  return false;
               }
            case "object":
            case "player":
            case "team":
               if (!item.has_index()) {
                  validator.report(this, `You must specify which global ${item.name} variable you wish to use, e.g. "global.${item.name}[0]".`);
                  return false;
               }
               break;
            default:
               validator.report(this, `The "global" namespace does not contain variables of type ${item.name}.`);
               return false;
         }
         this.analyzed.scope = var_scope.global;
         switch (item.name) {
            case "number": this.analyzed.type = var_type.number; break;
            case "object": this.analyzed.type = var_type.object; break;
            case "player": this.analyzed.type = var_type.player; break;
            case "team":   this.analyzed.type = var_type.team;   break;
            case "timer":  this.analyzed.type = var_type.timer;  break;
            default: throw new Error("[DEV] You need to keep this switch-case in synch with the one above.");
         }
         this.analyzed.index = item.index;
         item = this.parts[++index];
         if (!item) {
            switch (this.analyzed.type) {
               case var_type.number: this.analyzed.scope = number_var_scope.global_number; break;
               case var_type.object: this.analyzed.scope = object_var_scope.global; break;
               case var_type.team:   this.analyzed.scope = team_var_scope.global;   break;
               case var_type.timer:  // TODO
            }
            return true;
         }
         //
         // And fall through in order to handle namespace.var.property and namespace.var.var[.property].
         //
      } else if (item.name == "game") {
         if (item.has_index()) {
            validator.report(this, `Namespaces such as "game" cannot be indexed.`);
            return false;
         }
         item = this.parts[++index];
         if (!item) {
            validator.report(this, `Namespaces such as "game" cannot be accessed directly.`);
            return false;
         }
         switch (item.name) {
            case "current_round":
            case "symmetry_getter":
            case "symmetry":
            case "score_to_win":
            case "unkF7A6":
            case "teams_enabled":
            case "round_time_limit":
            case "round_limit":
            case "misc_unk0_bit3":
            case "rounds_to_win":
            case "sudden_death_time":
            case "grace_period":
            case "lives_per_round":
            case "team_lives_per_round":
            case "respawn_time":
            case "suicide_penalty":
            case "betrayal_penalty":
            case "respawn_growth":
            case "loadout_cam_time":
            case "respawn_traits_duration":
            case "friendly_fire":
            case "betrayal_booting":
            case "social_flags_bit_2":
            case "social_flags_bit_3":
            case "social_flags_bit_4":
            case "grenades_on_map":
            case "indestructible_vehicles":
            case "powerup_duration_red":
            case "powerup_duration_blue":
            case "powerup_duration_yellow":
            case "death_event_damage_type":
               this.analyzed.type  = var_type.number;
               this.analyzed.scope = number_var_scope[item.name];
               this.analyzed.which = null;
               this.analyzed.index = null;
               this.analyzed.is_read_only = true;
               break;
            case "round_timer":
            case "sudden_death_timer":
            case "grace_period_timer":
               this.analyzed.type  = var_type.timer;
               this.analyzed.scope = timer_var_scope[item.name];
               this.analyzed.which = null;
               this.analyzed.index = null;
               this.analyzed.is_read_only = true;
               break;
            default:
               validator.report(this, `The "game" namespace does not contain a property named "${item.name}".`);
               return false;
         }
         item = this.parts[++index];
         if (item) {
            validator.report(this, `Game state values do not, themselves, have properties.`);
            return false;
         }
         return true;
      } else {
         assert(index == 0, "At this point, we should still be looking at the first item.");
         let found   = false;
         let is_none = false;
         for(let situational of situational_variables) {
            if (item.name == situational.name) {
               found   = true;
               is_none = situational.is_none;
               this.analyzed.scope = var_scope.global;
               this.analyzed.type  = situational.type;
               this.analyzed.which = situational.name; // in C++ we want this to be the megalo_XXXXX enums
               this.analyzed.is_read_only = true;
               break;
            }
         }
         if (found) {
            item = this.parts[++index];
            if (is_none && item) {
               validator.report(this, `Cannot access properties on a "none" variable.`);
               return false;
            }
            //
            // And fall through in order to handle situational.property and situational.var[.property].
            //
         } else {
            assert(index == 0, "At this point, we should still be looking at the first item.");
            //
            // REMAINING, AT THIS POINT: static; indexed_data
            //
            {  // indexed_data
               if (item.name == "script_option") { // indexed_data: script options
                  if (!item.has_index()) {
                     validator.report(this, `If you wish to access a script option, you must specify which one.`);
                     return false;
                  }
                  // TODO: bounds-check the index (we can just do this in C++)
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.script_option;
                  this.analyzed.which = null;
                  this.analyzed.index = item.index;
                  this.analyzed.is_read_only = true;
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Script options don't have properties.`);
                     return false;
                  }
                  return true;
               }
               if (item.name == "script_traits") { // indexed_data: script traits
                  if (!item.has_index()) {
                     validator.report(this, `If you wish to access a set of scripted player traits, you must specify which one.`);
                     return false;
                  }
                  // TODO: bounds-check the index (we can just do this in C++)
                  this.analyzed.scope = var_scope.global;
                  this.analyzed.type  = var_type.not_a_variable;
                  this.analyzed.which = "script_traits"; // in C++ we want this to be an enum
                  this.analyzed.index = item.index;
                  this.analyzed.is_read_only = true;
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Scripted player traits don't have properties.`);
                     return false;
                  }
                  return true;
               }
            }
            //
            // REMAINING, AT THIS POINT: static
            //
            if (item.name == "player") { // static player
               if (!item.has_index()) {
                  validator.report(this, `If you wish to access a static player, you must specify which one, e.g. player[0] for Player 1.`);
                  return false;
               }
               if (item.index > 16) {
                  validator.report(this, `Out-of-bounds static player. Halo: Reach only supports up to 16 players, and static players are zero-indexed.`);
                  return false;
               }
               this.analyzed.scope = var_scope.global;
               this.analyzed.type  = var_type.player;
               this.analyzed.which = "player[" + item.index + "]"; // in C++ we want this to be the megalo_players enum
               //
               // And fall through in order to handle static.property and static.var[.property].
               //
            } else if (item.name == "team") { // static team
               if (!item.has_index()) {
                  validator.report(this, `If you wish to access a static team, you must specify which one, e.g. team[0] for Team 1.`);
                  return false;
               }
               if (item.index > 8) {
                  validator.report(this, `Out-of-bounds static team. Halo: Reach only supports up to 8 players, and static teams are zero-indexed.`);
                  return false;
               }
               this.analyzed.scope = var_scope.global;
               this.analyzed.type  = var_type.team;
               this.analyzed.which = "team[" + item.index + "]"; // in C++ we want this to be the megalo_teams enum
               //
               // And fall through in order to handle static.property and static.var[.property].
               //
            } else {
               if (item.name == "object") {
                  validator.report(this, `Objects must be namespaced; did you mean "global.object[...]"?`);
                  return false;
               }
               validator.report(this, `Unrecognized variable root ${item.name}.`);
               return false;
            }
            item = this.parts[++index];
            //
            // And fall through to handle properties.
            //
         }
      }
      if (!item) // We've reached the end.
         return true;
      this.analyzed.is_read_only = false; // "current_object" would have set this to true, but we want "current_object.number[0]" to be writeable
      //
      // NOW, WE NEED TO HANDLE PROPERTIES:
      //
      //    situational.var
      //    static.var
      //    namespace.var.var
      //    any_object.team
      //    any_player.biped
      //    any_player.team
      //
      if (this.analyzed.type === null) {
         validator.report(this, `Property access on unknown type.`);
         return false;
      }
      switch (this.analyzed.type) {
         //
         // Validate property/nested-variable access on a given type (i.e. guard against 
         // global.number.variable), and handle properties that exist on global variables 
         // (i.e. situational.property, static.property, and global.var.property).
         //
         case var_type.object:
            switch (item.name) {
               case "spawn_sequence":
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (object.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.spawn_sequence;
                  if (this.analyzed.which) { // conform JavaScript data to match C++
                     this.analyzed.index = this.analyzed.which;
                     this.analyzed.which = null;
                  }
                  return true;
               case "team":
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (object.property.anything) are invalid. Store the team in a team variable and then access properties through that.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.team;
                  this.analyzed.scope = team_var_scope.object_owner_team;
                  if (this.analyzed.index) {
                     this.analyzed.which = this.analyzed.index;
                     this.analyzed.index = null;
                  }
                  return true;
            }
            break;
         case var_type.player:
            switch (item.name) { // global.player[n].property
               case "biped":
                  if (item.has_index()) {
                     validator.report(this, `Player bipeds cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Property access of the form (player.biped.anything) is invalid. Store the biped in an object variable and then access properties through that.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.object;
                  this.analyzed.scope = object_var_scope.global_player_biped;
                  this.analyzed.which = this.analyzed.which;
                  this.analyzed.is_read_only = true;
                  return true;
               case "rating":
                  if (item.has_index()) {
                     validator.report(this, `Player ratings cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (player.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.player_rating;
                  if (this.analyzed.which) { // conform JavaScript data to match C++
                     this.analyzed.index = this.analyzed.which;
                     this.analyzed.which = null;
                  }
                  return true;
               case "score":
                  if (item.has_index()) {
                     validator.report(this, `Player scores cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (player.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.player_score;
                  if (this.analyzed.which) { // conform JavaScript data to match C++
                     this.analyzed.index = this.analyzed.which;
                     this.analyzed.which = null;
                  }
                  return true;
               case "script_stat":
                  if (!item.has_index()) {
                     validator.report(this, `You must specify which stat to access.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (player.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.player_stat;
                  this.analyzed.which = this.analyzed.which; // which/index handling will likely differ between JavaScript and C++
                  this.analyzed.index = item.index; // TODO: validate that the index is in bounds
                  return true;
               case "team":
                  if (item.has_index()) {
                     validator.report(this, `Player teams cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Property access of the form (player.team.anything) is invalid. Store the team in a team variable and then access properties through that.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.team;
                  this.analyzed.scope = team_var_scope.player_owner_team;
                  if (this.analyzed.index) {
                     this.analyzed.which = this.analyzed.index;
                     this.analyzed.index = null;
                  }
                  return true;
               case "unknown_09":
                  if (item.has_index()) {
                     validator.report(this, `Player unknown_09s cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (player.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.unknown_09;
                  if (this.analyzed.which) { // conform JavaScript data to match C++
                     this.analyzed.index = this.analyzed.which;
                     this.analyzed.which = null;
                  }
                  return true;
            }
            break;
         case var_type.team:
            switch (item.name) { // global.player[n].property
               case "score":
                  if (item.has_index()) {
                     validator.report(this, `Team scores cannot be indexed.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (team.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.team_score;
                  if (this.analyzed.which) { // conform JavaScript data to match C++
                     this.analyzed.index = this.analyzed.which;
                     this.analyzed.which = null;
                  }
                  return true;
               case "script_stat":
                  if (!item.has_index()) {
                     validator.report(this, `You must specify which stat to access.`);
                     return false;
                  }
                  if (this.parts.length > (index + 1)) {
                     validator.report(this, `Accessors of the form (team.property.anything) are invalid.`);
                     return false;
                  }
                  this.analyzed.type  = var_type.number;
                  this.analyzed.scope = number_var_scope.team_stat;
                  this.analyzed.which = this.analyzed.which; // which/index handling will likely differ between JavaScript and C++
                  this.analyzed.index = item.index; // TODO: validate that the index is in bounds
                  return true;
            }
            break;
         default:
            validator.report(this, `Property access on a variable type that does not support properties.`);
            return false;
      }
      //
      // REMAINING AT THIS POINT: namespace.var.var, situational.var, static.var (all including properties)
      //
      // Right now, (this.analyzed) refers to the containing variable; for example, if we are 
      // accessing (global.object[0].player[1]), then (this.analyzed) refers to (global.object[0]).
      //
      switch (item.name) {
         case "number":
         case "object":
         case "player":
         case "team":
         case "timer":
            break;
         default:
            validator.report(this, `Unrecognized variable "${this.serialize()}".`);
            return false;
      }
      if (!item.has_index()) {
         validator.report(this, `You must specify which ${item.name} variable you mean.`);
         return false;
      }
      this.analyzed.scope = var_type_to_var_scope(this.analyzed.type);
      this.analyzed.which = this.analyzed.index;
      this.analyzed.index = item.index;
      switch (item.name) {
         case "number": this.analyzed.type = var_type.number; this.analyzed.scope = var_scope_to_number_scope(this.analyzed.scope); break;
         case "object": this.analyzed.type = var_type.object; this.analyzed.scope = var_scope_to_object_scope(this.analyzed.scope); break;
         case "player": this.analyzed.type = var_type.player; break; // same scope enum as variable scopes in general
         case "team":   this.analyzed.type = var_type.team;   break;
         case "timer":  this.analyzed.type = var_type.timer;  break;
         default: throw new Error("[DEV] You need to keep this switch-case in synch with the one above.");
      }
      item = this.parts[++index];
      if (!item)
         return true;
      switch (item.name) {
         //
         // Handle properties that can appear on non-global variables (i.e. global.anything.anything.property).
         //
         case "biped": // global.anything.player[n].biped
            if (this.analyzed.type == var_type.player) {
               if (this.parts.length > (index + 1)) {
                  validator.report(this, `Property access of the form (player.biped.anything) is invalid. Store the biped in an object variable and then access properties through that.`);
                  return false;
               }
               this.analyzed.type  = var_type.object;
               this.analyzed.scope = var_scope_to_biped_scope(this.analyzed.scope);
               this.analyzed.is_read_only = true;
               return true;
            }
            break;
      }
      validator.report(this, `Property access of the form (object.var.var.anything) is invalid. Use an intermediate variable.`);
      return false;
      /*//
      //
      // SPECIFIC LIST OF ALL VARIABLES (different indices not listed):
      //
      // no_object
      // no_player
      // no_team
      // player[0]         // static player
      // team[0]           // static team
      // neutral_team      // static team (neutral)
      // current_object    // loop variable
      // current_player    // loop variable
      // current_team      // loop variable
      // hud_player
      // hud_target_player
      // hud_target_object
      // killed_object
      // killer_object
      // killer_player
      // hud_player.team
      // hud_target.team
      // unk_14_team
      // unk_15_team
      // script_option[0]
      // script_stat[0]
      // script_string[0]
      // script_traits[0]
      // global.number[0]  // global.number variable
      // global.object[0]  // global.object variable
      // global.player[0]  // global.player variable
      // global.team[0]    // global.team   variable
      // global.timer[0]   // global.timer  variable
      // global.object[0].number[0] // object.number variable (namespace.var)
      // global.object[0].object[0] // object.object variable (namespace.var)
      // global.object[0].player[0] // object.player variable (namespace.var)
      // global.object[0].team[0]   // object.team   variable (namespace.var)
      // global.object[0].timer[0]  // object.timer  variable (namespace.var)
      // player[0].number[0]        // player.number variable (static.var)
      // global.player[0].number[0] // player.number variable (var.var)
      // player[0].object[0]        // player.object variable (static.var)
      // global.player[0].object[0] // player.object variable (var.var)
      // player[0].player[0]        // player.player variable (static.var)
      // global.player[0].player[0] // player.player variable (var.var)
      // player[0].team[0]          // player.team   variable (static.var)
      // global.player[0].team[0]   // player.team   variable (var.var)
      // player[0].timer[0]         // player.timer  variable (static.var)
      // global.player[0].timer[0]  // player.timer  variable (var.var)
      // team[0].number[0]          // team.number   variable (static.var)
      // global.team[0].number[0]   // team.number   variable (var.var)
      // team[0].object[0]          // team.object   variable (static.var)
      // global.team[0].object[0]   // team.object   variable (var.var)
      // team[0].player[0]          // team.player   variable (static.var)
      // global.team[0].player[0]   // team.player   variable (var.var)
      // team[0].team[0]            // team.team     variable (static.var)
      // global.team[0].team[0]     // team.team     variable (var.var)
      // team[0].timer[0]           // team.timer    variable (static.var)
      // global.team[0].timer[0]    // team.timer    variable (var.var)
      // player[0].biped                  // player.biped        (static.biped)
      // global.object[0].player[0].biped // object.player.biped (var.var.biped)
      // global.player[0].player[0].biped // player.player.biped (var.var.biped)
      // global.team[0].player[0].biped   // team.player.biped   (var.var.biped)
      // current_player.team              // player.team (keyword.team) // also for hud_player, etc.
      // player[0].team                   // player.team (static.team)
      // current_object.team              // object.team (keyword.team) // also for killed_object, etc.
      // global.object[0].team            // object.team (var.team)
      //
      //*/
   }
}

class MFunctionCall extends MParsedItem {
   constructor() {
      super();
      this.context = null; // MVariableReference
      this.name    = "";
      this.args    = []; // constant integers, string literals, or MVariableReferences
   }
   extract_stem(text) {
      let size = text.length;
      let i    = size - 1;
      for(; i >= 0; --i) {
         let c = text[i];
         if (("[]").indexOf(c) >= 0)
            return false;
         if (c == ".") {
            this.name = text.substring(i + 1);
            break;
         }
      }
      if (!this.name) {
         if (i == size - 1) // "name.()"
            return false;
         this.name = text;
      } else {
         this.context = new MVariableReference;
         this.context.owner = this;
         this.context.extract(text.substring(0, i));
      }
      return true;
   }
   serialize() {
      let result = `${this.name}(`;
      if (this.context)
         result = `${this.context.serialize()}.${result}`;
      for(let i = 0; i < this.args.length; i++) {
         if (i > 0)
            result += ", ";
         let arg = this.args[i];
         if (arg instanceof MVariableReference || arg instanceof MStringLiteral) {
            result += arg.serialize();
         } else if (+arg === arg) {
            result += arg;
         } else
            throw new Error("unhandled");
      }
      result += ")";
      return result;
   }
   validate(validator) {
      if (this.context) {
         this.context.validate(validator);
         //
         // TODO: if the context is valid, verify that the member function we're trying 
         // to call exists.
         //
      } else {
         //
         // TODO: Verify that the global function we're trying to call exists (i.e. a 
         // user-defined function or an opcode).
         //
      }
      //
      // TODO: Verify call arguments.
      //
   }
   //
   is_condition() {
      //
      // I kinda don't like this because it isn't based on any intrinsic property of 
      // MFunctionCalls, but rather on the behavior of other classes... but eh.
      //
      if (this.owner)
         return this.owner instanceof MBlock; // if instanceof MAssignment, then action
      return false;
   }
}
class MOperator extends MParsedItem {
   constructor(o) {
      super();
      this.text = o;
   }
   is_assignment() {
      switch (this.text) {
         case "=":
         case "+=":
         case "-=":
         case "*=":
         case "/=":
         case "%=":
         case ">>=":
         case "<<=":
         case ">>>=":
         case "~=":
         case "^=":
         case "&=":
         case "|=":
            return true;
      }
      return false;
   }
   is_comparison() {
      switch (this.text) {
         case "==":
         case "!=":
         case ">=":
         case "<=":
         case ">":
         case "<":
            return true;
      }
      return false;
   }
   serialize() {
      return this.text;
   }
}

function token_to_int_or_var(text) { // converts a token to a constant int or to an MVariableReference
   let size = text.length;
   {  // constant integer?
      let found = false;
      let valid = true;
      for(let i = 0; i < size; i++) {
         let c = text[i];
         if (c == ".")
            throw new Error("Floating-point numbers are not supported. Numbers cannot have a decimal point.");
         if (c == "-") {
            if (i > 0) {
               valid = false;
               break;
            }
         } else {
            if (c >= '0' && c <= '9') {
               found = true;
            } else {
               valid = false;
               break;
            }
         }
      }
      if (found && valid)
         return +text;
   }
   return new MVariableReference(text);
}

class MAssignment extends MParsedItem {
   constructor() {
      super();
      this.target   = null; // MVariableReference
      this.source   = null; // MVariableReference, MFunctionCall, or constant integer
      this.operator = null; // MOperator
   }
   serialize() {
      let result = "";
      if (this.target instanceof MVariableReference || this.target instanceof MFunctionCall)
         result += this.target.serialize();
      else if (+this.target === this.target)
         result += this.target;
      else
         throw new Error("unhandled");
      result = `${result} ${this.operator.serialize()} `;
      if (this.source instanceof MVariableReference || this.source instanceof MFunctionCall)
         result += this.source.serialize();
      else if (+this.source === this.source)
         result += this.source;
      else
         throw new Error("unhandled");
      return result;
   }
   validate(validator) {
      if (+this.target === this.target)
         validator.report(this, "You cannot assign to a constant integer.");
      else if (this.target instanceof MVariableReference) {
         this.target.validate(validator);
         if (this.target.is_read_only())
            validator.report(this, "You cannot assign to this variable/property. It is read-only.");
      }
      //
      // TODO: Fail if assigning to a read-only target.
      //
      if (this.source instanceof MParsedItem || this.source instanceof MVariableReference) {
         this.source.validate(validator);
      }
   }
}
class MComparison extends MParsedItem {
   constructor() {
      super();
      this.left     = null; // MVariableReference or constant integer
      this.right    = null; // MVariableReference or constant integer
      this.operator = null; // MOperator
   }
   serialize() {
      let result = "";
      if (this.left instanceof MVariableReference)
         result += this.left.serialize();
      else if (+this.left === this.left)
         result += this.left;
      else
         throw new Error("unhandled");
      result += ` ${this.operator.serialize()} `;
      if (this.right instanceof MVariableReference)
         result += this.right.serialize();
      else if (+this.right === this.right)
         result += this.right;
      else
         throw new Error("unhandled");
      return result;
   }
   validate(validator) {
      if (this.left instanceof MParsedItem || this.left instanceof MVariableReference)
         this.left.validate(validator);
      if (this.right instanceof MParsedItem || this.right instanceof MVariableReference)
         this.right.validate(validator);
   }
}

//
// ALLOWED STATEMENTS:
//  - assigmnent
//  - block
//  - comparison
//  - call
//
// ASSIGNMENT:
// [variable] [op] [variable OR constant int OR call]
//
// COMPARISON:
// [variable OR constant int] [op] [variable OR constant int]
//
// CALL:
// [variable].[name]([args...])
// [name]([args...])
//
// RAW DATA:
//  - word: anything that isn't an operator, call, etc., including keywords and constant ints
//

class MSimpleParser {
   constructor() {
      this.text = "";
      this.pos  = 0;
      this.line = 0;
      this.last_newline = 0;
      //
      this.root      = null;  // root block
      this.block     = null;  // block we're currently parsing
      this.token     = "";    // current token being parsed (lefthand word / operator / call argument / righthand word)
      this.token_pos = null;  // stream position of current token (as reported by backup_stream_state)
      this.token_end = false; // whether we hit whitespace, meaning that the current word is "over." if so, then another word character indicates {word word} which is bad syntax (outside of keywords)
      this.statement = null;  // current statement
      this.call      = null;  // current call (statement or assignment righthand side)
   }
   throw_error(text) {
      throw new Error(`Error on or near line ${this.line + 1} col ${this.pos - this.last_newline + 1}: ` + text);
   }
   //
   backup_stream_state() {
      //
      // If you need to rewind the stream, use this along with restore_stream_state, 
      // so that our line/col tracking doesn't break.
      //
      return {
         pos:          this.pos,
         line:         this.line,
         last_newline: this.last_newline,
      };
   }
   restore_stream_state(s) {
      if (s.pos !== void 0)
         this.pos = s.pos;
      if (s.line !== void 0)
         this.line = s.line;
      if (s.last_newline !== void 0)
         this.last_newline = s.last_newline;
   }
   reset_token() {
      this.token     = "";
      this.token_pos = null;
      this.token_end = false;
   }
   //
   scan(functor) { // process the input stream one character at a time, passing the character to some functor
      let text    = this.text;
      let length  = text.length;
      let comment = false; // are we inside of a line comment?
      for(; this.pos < length + 1; ++this.pos) {
         //
         // A lot of our parser logic relies on reacting to the character after something, 
         // e.g. processing a keyword when we encounter the non-word character after it. 
         // However, this means that if a token ends at EOF (such that there's no character 
         // after the token), then parsing code can't react to it.
         //
         // We solve this problem by just pretending that the input stream is one character 
         // longer than it really is: we supply a trailing space character in order to be 
         // sure that any given token's end is processed.
         //
         let c = " ";
         if (this.pos < length)
            c = text[this.pos];
         //
         if (c == "\n") {
            if (this.pos != this.last_newline) {
               //
               // Before we increment the line counter, we want to double-check that we 
               // haven't seen this specific line break before. This is because if a 
               // functor chooses to stop scanning on a newline, the next call to (scan) 
               // will see that same newline again.
               //
               ++this.line;
               this.last_newline = this.pos;
            }
            if (comment) {
               comment = false;
               continue;
            }
         }
         if (comment)
            continue;
         if (c == "-" && this.pos < length - 1 && text[this.pos + 1] == "-") { // handle line comments
            comment = true;
            continue;
         }
         this.last_scanned = this.pos;
         if (functor(c))
            break;
      }
   }
   //
   parse(text) {
      this.text = text;
      if (!this.root) {
         this.root = new MBlock;
         this.root.set_start(this.backup_stream_state());
      }
      this.block = this.root;
      //
      this.statement = null;
      this.call      = null;
      this.reset_token();
      this.scan((function(c) {
         //
         // If we're not in a statement, then the next token must be a word. If that word is a 
         // keyword, then we handle it accordingly. If it is not a keyword, then it must be 
         // followed either by an operator (in which case we're in an assign statement) or by 
         // an opening parentheses (in which case the statement is a function call).
         //
         if (!this.statement) {
            this._parseActionStart(c)
            return;
         }
         //
         // If, on the other hand, we're in a statement, then we need to finish that up.
         //
         if (this.statement instanceof MAssignment) {
            this._parseAssignment(c);
            return;
         }
      }).bind(this));
      //
      if (this.block != this.root)
         this.throw_error(`Unclosed block.`);
      if (this.statement) {
         if (this.statement instanceof MAssignment)
            this.throw_error(`An assignment statement is missing its righthand side.`);
         if (this.statement instanceof MFunctionCall)
            this.throw_error(`A function call statement is unterminated.`);
         this.throw_error(`The file ended before a statement could be fully processed.`);
      }
      //
      this.root.set_end(this.pos);
      return this.root;
   }
   _parseActionStart(c) {
      if (!this.token.length) {
         if (c != "-" && is_operator_char(c)) // minus-as-numeric-sign must be special-cased
            this.throw_error(`Unexpected ${c}. Statements cannot begin with an operator.`);
         if (is_syntax_char(c))
            this.throw_error(`Unexpected ${c}.`);
         if (is_quote_char(c))
            this.throw_error(`Unexpected ${c}. Statements cannot begin with a string literal.`);
         if (is_whitespace_char(c))
            return;
         this.token += c;
         this.token_pos = this.backup_stream_state();
         return;
      }
      if (is_whitespace_char(c)) {
         this.token_end = true;
         //
         // Handle keywords here, if appropriate.
         //
         switch (this.token) {
            case "and":
            case "or":
            case "then":
               this.throw_error(`The "${this.token}" keyword cannot appear here.`);
         }
         let handler = this.getHandlerForKeyword(this.token);
         if (handler) {
            let prior = this.pos;
            handler.call(this);
            this.reset_token();
            if (prior < this.pos) {
               //
               // The handler code advanced the position to the end of the keyword's relevant 
               // content (e.g. the end of a block declaration). However, our containing loop 
               // will increment the position one more time, so we need to rewind by one.
               //
               --this.pos;
            }
         }
         //
         // If (handler) is null, then the word wasn't a keyword. Move to the next iteration 
         // of the parsing loop; we'll eventually feed the word to a new statement.
         //
         return;
      }
      if (is_quote_char(c))
         this.throw_error(`Unexpected ${c}. Statements of the form {word "string" ...} are not valid.`);
      if (c == "(") {
         this.statement = this.call = new MFunctionCall;
         this.call.set_start(this.token_pos);
         if (!this.call.extract_stem(this.token))
            this.throw_error(`Invalid function context and/or name: "${this.token}".`);
         this.reset_token();
         ++this.pos; // advance past the open-parentheses
         this._parseFunctionCall(false);
         --this.pos; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ")" || c == ",")
         this.throw_error(`Unexpected ${c}.`);
      if (is_operator_char(c)) {
         this.statement = new MAssignment;
         this.statement.set_start(this.token_pos);
         try {
            let lhs = token_to_int_or_var(this.token);
            this.statement.target = lhs;
            if (lhs instanceof MParsedItem)
               lhs.owner = this.statement;
         } catch (e) {
            this.throw_error(e.message);
         }
         this.reset_token();
         this.token = c;
         this.token_pos = this.backup_stream_state();
         return;
      }
      if (this.token_end)
         this.throw_error(`Statements of the form {word word} are not valid.`);
      this.token += c;
      if (this.token[0] == "-" && isNaN(+c)) {
         //
         // We allowed the word to start with "-" in case it was a number, but it 
         // has turned out not to be a number. That means that the "-" was an 
         // operator, not a numeric sign. Wait, that's illegal.
         //
         this.throw_error(`Unexpected -. Statements cannot begin with an operator.`);
      }
   }
   _parseAssignment(c) {
      if (!this.statement.operator) {
         //
         // If the statement doesn't have an operator stored, then the operator is currently 
         // being parsed and exists in (token).
         //
         if (is_operator_char(c)) {
            this.token += c;
            return;
         }
         this.statement.operator = new MOperator(this.token);
         this.statement.operator.set_start(this.token_pos);
         this.statement.operator.set_end(this.pos);
         if (!this.statement.operator.is_assignment())
            this.throw_error(`Operator ${this.token} is not a valid assignment operator.`);
         this.reset_token();
         //
         // Fall through to righthand-side handling so we don't miss the first character 
         // after the operator in cases like {a=b} where there's no whitespace.
         //
      }
      if ((this.token.length || c != "-") && is_operator_char(c))
         this.throw_error(`Unexpected ${c} on the righthand side of an assignment statement.`);
      if (is_quote_char(c))
         this.throw_error(`Unexpected ${c}. You cannot assign strings to variables.`);
      if (!this.token.length) {
         if (is_whitespace_char(c))
            return;
         if (c == "(")
            this.throw_error(`Unexpected ${c}. Parentheses are only allowed as delimiters for function arguments.`);
      } else {
         if (c == "(") {
            //
            // Handle function call as righthand side.
            //
            this.call = new MFunctionCall;
            this.call.set_start(this.token_pos);
            if (!this.call.extract_stem(this.token))
               this.throw_error(`Invalid function context and/or name: "${this.token}".`);
            this.statement.source = this.call;
            this.call.owner = this.statement;
            this.reset_token();
            ++this.pos; // advance past the open-parentheses
            this._parseFunctionCall(false);
            --this.pos; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
            return;
            //
            // From here on out, the code for parsing function calls will handle what 
            // remains. The end of the function call is also the end of this statement, 
            // so the code for parsing function calls will "close" this statement just 
            // fine.
            //
         }
      }
      if (c == ")" || c == ",")
         this.throw_error(`Unexpected ${c}.`);
      if (!is_whitespace_char(c)) {
         if (!this.token.length)
            this.token_pos = this.backup_stream_state();
         this.token += c;
         return;
      }
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      this.statement.set_end(this.pos);
      try {
         this.statement.source = token_to_int_or_var(this.token);
      } catch (e) {
         this.throw_error(e.message);
      }
      this.block.insert_item(this.statement);
      this.statement = null;
      this.reset_token();
   }
   //
   _parseBlockConditions() {
      this.statement = null;
      this.call      = null;
      this.reset_token();
      this.scan((function(c) {
         if (!this.statement) {
            if (this._parseConditionStart(c) == "stop")
               return true; // stop the loop; we found the "then" keyword
            return;
         }
         // If, on the other hand, we're in a statement, then we need to finish that up.
         //
         if (this.statement instanceof MComparison) {
            this._parseComparison(c);
            return;
         }
      }).bind(this));
   }
   _parseConditionStart(c) {
      if (!this.token.length) {
         if (c != "-" && is_operator_char(c)) // minus-as-numeric-sign must be special-cased
            this.throw_error(`Unexpected ${c}. Conditions cannot begin with an operator.`);
         if (is_syntax_char(c))
            this.throw_error(`Unexpected ${c}.`);
         if (is_quote_char(c))
            this.throw_error(`Unexpected ${c}. Conditions cannot begin with a string literal.`);
         if (is_whitespace_char(c))
            return;
         this.token += c;
         this.token_pos = this.backup_stream_state();
         return;
      }
      if (is_whitespace_char(c)) {
         this.token_end = true;
         //
         // Handle keywords here, if appropriate.
         //
         if (this.token == "then") {
            return "stop";
         }
         switch (this.token) {
            case "alias":
            case "expect":
               this.throw_error(`You cannot place ${this.token} declarations inside of conditions.`);
            case "do":
               this.throw_error(`You cannot open or close blocks inside of conditions. (If the "do" was meant to mark the end of conditions, use "then" instead.)`);
            case "else":
            case "elseif":
            case "end":
            case "for":
            case "function":
            case "if":
               this.throw_error(`You cannot open or close blocks inside of conditions.`);
         }
         if (this.token == "and" || this.token == "or") {
            let joiner = new MConditionJoiner;
            joiner.range = [this.pos - this.token.length, this.pos];
            joiner.is_or = (this.token == "or");
            try {
               this.block.insert_condition(joiner);
            } catch (e) {
               this.throw_error(e.message);
            }
            this.reset_token();
         }
         return;
      }
      if (is_quote_char(c))
         this.throw_error(`Unexpected ${c}. Statements of the form {word "string" ...} are not valid.`);
      if (c == "(") {
         this.statement = this.call = new MFunctionCall;
         this.call.set_start(this.token_pos);
         if (!this.call.extract_stem(this.token))
            this.throw_error(`Invalid function context and/or name: "${this.token}".`);
         this.reset_token();
         ++this.pos; // advance past the open-parentheses
         this._parseFunctionCall(true);
         --this.pos; // _parseFunctionCall moved us to the end of the call, but we're being run from inside of a scan-functor -- effectively a loop -- so we're going to advance one more
         return;
      }
      if (c == ")" || c == ",")
         this.throw_error(`Unexpected ${c}.`);
      if (is_operator_char(c)) {
         this.statement = new MComparison;
         this.statement.set_start(this.token_pos);
         try {
            let lhs = token_to_int_or_var(this.token);
            this.statement.left = lhs;
            if (lhs instanceof MParsedItem)
               lhs.owner = this.statement;
         } catch (e) {
            this.throw_error(e.message);
         }
         this.reset_token();
         this.token = c;
         this.token_pos = this.backup_stream_state();
         return;
      }
      if (this.token_end)
         this.throw_error(`Statements of the form {word word} are not valid.`);
      this.token += c;
      if (this.token[0] == "-" && isNaN(+c)) {
         //
         // We allowed the word to start with "-" in case it was a number, but it 
         // has turned out not to be a number. That means that the "-" was an 
         // operator, not a numeric sign. Wait, that's illegal.
         //
         this.throw_error(`Unexpected -. Conditions cannot begin with an operator.`);
      }
   }
   _parseComparison(c) {
      if (!this.statement.operator) {
         //
         // If the statement doesn't have an operator stored, then the operator is currently 
         // being parsed and exists in (token).
         //
         if (is_operator_char(c)) {
            this.token += c;
            return;
         }
         this.statement.operator = new MOperator(this.token);
         this.statement.operator.set_start(this.token_pos);
         this.statement.operator.set_end(this.pos);
         if (!this.statement.operator.is_comparison())
            this.throw_error(`Operator ${this.token} is not a valid comparison operator.`);
         this.reset_token();
         //
         // Fall through to righthand-side handling so we don't miss the first character 
         // after the operator in cases like {a=b} where there's no whitespace.
         //
      }
      //
      // Handle righthand side.
      //
      if ((this.token.length || c != "-") && is_operator_char(c))
         this.throw_error(`Unexpected ${c} on the righthand side of a comparison statement.`);
      if (is_quote_char(c))
         this.throw_error(`Unexpected ${c}. You cannot compare variables to strings.`);
      if (!this.token.length && is_whitespace_char(c))
         return;
      if (c == "(") {
         if (this.token.length)
            this.throw_error(`Unexpected ${c}. You cannot compare variables to the result of a function call.`);
         this.throw_error(`Unexpected ${c}. Parentheses are only allowed as delimiters for function arguments.`);
      }
      if (c == ")" || c == ",")
         this.throw_error(`Unexpected ${c}.`);
      if (!is_whitespace_char(c)) {
         this.token += c;
         if (!this.token.length)
            this.token_pos = this.backup_stream_state();
         return;
      }
      //
      // If we get here, then we've encountered the end of the statement's righthand side.
      //
      this.statement.set_end(this.pos);
      try {
         this.statement.right = token_to_int_or_var(this.token);
      } catch (e) {
         this.throw_error(e.message);
      }
      try {
         this.block.insert_condition(this.statement);
      } catch (e) {
         this.throw_error(e.message);
      }
      this.statement = null;
      this.reset_token();
   }
   //
   _parseFunctionCallArg() {
      let integer = this.extractIntegerLiteral();
      if (integer !== null) {
         this.call.args.push(integer);
         return true;
      }
      let string = this.extractStringLiteral();
      if (string !== null) {
         let arg = new MStringLiteral(string);
         this.call.args.push(arg);
         arg.owner = this.call;
         return true;
      }
      let word = this.extractWord();
      if (word) {
         let arg = token_to_int_or_var(word);
         this.call.args.push(arg);
         if (arg instanceof MParsedItem)
            arg.owner = this.call;
         return true;
      }
      return false;
   }
   _parseFunctionCall(is_condition) {
      //
      // When this function is called, the stream position should be just after the 
      // opening parentheses for the call arguments. Assuming no syntax errors are 
      // encountered, this function advances the stream position to just after the 
      // ")" glyph that marks the end of the call arguments; if you are calling 
      // this function from inside a functor being run by MSimpleParser.scan, then 
      // you will need to rewind the stream position by one to avoid skipping the 
      // glyph after the ")", because the functor is being run in a loop and the 
      // loop will advance the stream by one more character.
      //
      // Called from _parseActionStart, _parseConditionStart, and _parseAssignment.
      //
      let start = this.backup_stream_state();
      while (!this.extractSpecificChar(")")) {
         if (this.call.args.length) {
            if (!this.extractSpecificChar(","))
               this.throw_error("Function call arguments must be separated by commas.");
         }
         if (this.pos >= this.text.length) {
            this.restore_stream_state(start);
            this.throw_error(`Unterminated function call.`);
         }
         if (!this._parseFunctionCallArg()) {
            {  // Check if we have a trailing comma e.g. func(a, )
               let state = this.backup_stream_state();
               if (this.extractSpecificChar(")")) {
                  this.restore_stream_state(state);
                  this.throw_error(`Commas in function calls can only appear between arguments; func(a,) is not valid.`);
               }
               if (this.extractSpecificChar("(")) {
                  this.restore_stream_state(state);
                  this.throw_error(`Parentheses may only be used to mark the start and end of list of function arguments.`);
               }
            }
            this.throw_error(`Failed to extract function argument.`);
         }
      }
      //
      // Function calls can exist as standalone statements or as the righthand side of 
      // assignment statements. In both cases, the end of the function call is the end 
      // of the statement. (You can assign the result of a function call to a variable 
      // (or rather, that's the syntax we're going with for specific trigger opcodes), 
      // but we don't support arbitrary expressions so things like (a = b() + c) won't 
      // be valid.)
      //
      this.reset_token();
      if (is_condition) {
         try {
            this.block.insert_condition(this.statement);
         } catch (e) {
            this.throw_error(e.message);
         }
      } else
         this.block.insert_item(this.statement);
      this.statement.set_end(this.pos + 1);
      this.call.set_end(this.pos + 1);
      this.call      = null;
      this.statement = null;
      return;
   }
   //
   extractIntegerLiteral() {
      let state = this.backup_stream_state();
      let sign  = "";
      let found = "";
      this.scan((function(c) {
         if (c == "-") { // handle numeric sign
            if (sign || found.length)
               return true;
            sign = c;
            return;
         }
         if (c >= "0" && c <= "9") {
            found += c;
            return;
         }
         if (is_whitespace_char(c))
            return found.length > 0; // stop if we have any digits
         if (c == "." && found.length)
            this.throw_error(`Floating-point numbers are not supported. Numbers cannot have a decimal point.`);
         return true;
      }).bind(this));
      if (!found.length) {
         this.restore_stream_state(state);
         return null;
      }
      return +(sign + found);
   }
   extractSpecificChar(required) { // advances the stream past the char if it is found
      let state = this.backup_stream_state();
      let found = false;
      this.scan(function(c) {
         if (is_whitespace_char(c))
            return;
         if (c == required)
            found = true;
         return true;
      });
      if (!found) {
         this.restore_stream_state(state);
         return false;
      }
      ++this.pos; // move position to after the char
      return true;
   }
   extractStringLiteral() { // returns contents of string literal or null if none is found; advances the stream past the end-quote if a string literal is found
      let state  = this.backup_stream_state();
      let inside = null; // char
      let result = "";
      this.scan((function(c) {
         if (inside) {
            if (c == inside) {
               if (this.text[this.pos - 1] == "\\") { // allow backslash-escaping
                  result += c;
                  return;
               }
               return true; // stop
            }
            result += c;
            return;
         }
         if (is_quote_char(c)) {
            inside = c;
            return;
         }
         if (!is_whitespace_char(c))
            return true; // stop
      }).bind(this));
      if (!inside) {
         this.restore_stream_state(state);
         return null;
      }
      ++this.pos; // move position to after the closing-quote
      return result;
   }
   extractWord(desired) {
      //
      // For the purposes of the parser, a "word" is any sequence of characters that are 
      // not whitespace, string delimiters, function syntax characters (i.e. parentheses 
      // and commas), or operator characters. This includes keywords, variables with 
      // square brackets and periods, and integer literals.
      //
      if (desired) {
         //
         // If searching for a specific word, advances the stream to the end of that word 
         // if it is found.
         //
         let state = this.backup_stream_state();
         let index = 0;
         let valid = true;
         this.scan((function(c) {
            if (is_whitespace_char(c))
               return index > 0; // abort... unless we haven't started yet
            c = c.toLowerCase();
            if (desired[index] != c) {
               valid = false;
               return true; // abort
            }
            ++index;
         }).bind(this));
         if (!valid) {
            this.restore_stream_state(state);
            return null;
         }
         return desired;
      }
      //
      // If searching for any word, advances the stream to the end of the found word or 
      // to the first non-word character.
      //
      let word = "";
      this.scan((function(c) {
         if (is_whitespace_char(c))
            return word.length > 0;
         if ((".[]").indexOf(c) >= 0 && !word.length) // these chars are allowed in a word, but not at the start of a word
            return true;
         if (is_syntax_char(c))
            return true;
         if (is_quote_char(c))
            return true;
         if (is_operator_char(c))
            return true;
         word += c;
      }).bind(this));
      return word;
   }
   //
   getHandlerForKeyword(word) { // for keywords outside of conditions only
      word = word.toLowerCase();
      switch (word) {
         case "alias":    return this._handleKeywordAlias;
         case "and":      return null; // should be rejected by caller
         case "do":       return this._handleKeywordDo;
         case "else":     return this._handleKeywordElse;
         case "elseif":   return this._handleKeywordElseIf;
         case "end":      return this._handleKeywordEnd;
         case "expect":   return this._handleKeywordExpect;
         case "for":      return this._handleKeywordFor;
         case "function": return this._handleKeywordFunction;
         case "if":       return this._handleKeywordIf;
         case "or":       return null; // should be rejected by caller
         case "then":     return null; // should be rejected by caller
      }
   }
   //
   _handleKeywordAlias() { // call after "alias "
      let start = this.token_pos;
      //
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`An alias declaration must supply a name.`);
      if (!this.extractSpecificChar("="))
         this.throw_error(`Expected "=".`);
      let target = this.extractWord();
      if (!target.length)
         this.throw_error(`An alias declaration must supply a target.`);
      //
      try {
         let item = new MAlias(name, target);
         item.set_start(start);
         item.set_end(this.pos);
         this.block.insert_item(item);
      } catch (e) {
         this.throw_error(e.message);
      }
   }
   _handleKeywordDo() {
      let created = new MBlock;
      created.type = block_type.basic;
      created.set_start(this.token_pos);
      this.block = this.block.insert_item(created);
   }
   _handleKeywordElse() {
      if (this.block.type != block_type.if && this.block.type != block_type.elseif) {
         let item = this.block.item(-1);
         if (item instanceof MBlock) {
            if (item.type == block_type.if || item.type == block_type.elseif)
               this.throw_error(`Unexpected "else". This keyword should not be preceded by the "end" keyword.`);
         }
         this.throw_error(`Unexpected "else".`);
      }
      this.block = this.block.parent;
      if (!this.block)
         this.throw_error(`Unexpected "else".`);
      let block = new MBlock;
      block.type = block_type.else;
      block.set_start(this.token_pos);
      this.block = this.block.insert_item(block);
   }
   _handleKeywordElseIf() {
      if (this.block.type != block_type.if && this.block.type != block_type.elseif) {
         let item = this.block.item(-1);
         if (item instanceof MBlock) {
            if (item.type == block_type.if || item.type == block_type.elseif)
               this.throw_error(`Unexpected "elseif". This keyword should not be preceded by the "end" keyword.`);
         }
         this.throw_error(`Unexpected "elseif".`);
      }
      this.block = this.block.parent;
      if (!this.block)
         this.throw_error(`Unexpected "elseif".`);
      let block = new MBlock;
      block.type = block_type.elseif;
      block.set_start(this.token_pos);
      this.block = this.block.insert_item(block);
      this._parseBlockConditions();
      this.reset_token();
   }
   _handleKeywordEnd() { // call after "end "
      this.block.set_end(this.pos - 1);
      this.block = this.block.parent;
      if (!this.block)
         this.throw_error(`Unexpected "end".`);
   }
   _handleKeywordExpect() { // call after "expect "
      let start = this.token_pos;
      //
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`An expect declaration must supply a name.`);
      if (!this.extractSpecificChar("="))
         this.throw_error(`Expected "==".`);
      if (!this.extractSpecificChar("="))
         this.throw_error(`Expected "==".`);
      let target = this.extractWord();
      if (!target.length)
         this.throw_error(`An expect declaration must supply a target.`);
      //
      let item = new MExpect(name, target);
      item.set_start(start);
      item.set_end(this.pos);
      this.block.insert_item(item);
   }
   _handleKeywordIf() { // call after "if "
      let block = new MBlock;
      block.type = block_type.if;
      block.set_start(this.token_pos);
      this.block = this.block.insert_item(block);
      this._parseBlockConditions();
      this.reset_token();
   }
   _handleKeywordFor() { // call after "for "
      let start = this.token_pos;
      //
      if (!this.extractWord("each"))
         this.throw_error(`The "for" keyword must be followed by "each".`);
      let word = this.extractWord();
      if (!word)
         this.throw_error(`Invalid for-loop.`);
      let use_type  = null;
      let use_label = null;
      switch (word) {
         case "team":
            use_type = block_type.for_each_team;
            if (!this.extractWord("do"))
               this.throw_error(`Invalid for-each-team loop: expected the word "do".`);
            break;
         case "player": // for each player; for each player randomly
            use_type = block_type.for_each_player;
            word     = this.extractWord();
            if (word == "randomly") {
               use_type = block_type.for_each_player_randomly;
               word       = this.extractWord();
            }
            if (word != "do") {
               if (use_type == block_type.for_each_player_randomly)
                  this.throw_error(`Invalid for-each-player-randomly loop: expected the word "do".`);
               this.throw_error(`Invalid for-each-player loop: expected the word "randomly" or the word "do".`);
            }
            break;
         case "object": // for each object; for each object with label [string literal or integer]
            use_type = block_type.for_each_object;
            word     = this.extractWord();
            if (word == "do")
               break;
            if (word != "with")
               this.throw_error(`Invalid for-each-object loop: expected the word "with" or the word "do".`);
            use_type = block_type.for_each_object_with_label;
            if (!this.extractWord("label"))
               this.throw_error(`Invalid for-each-object-with-label loop: expected the word "label".`);
            use_label = this.extractStringLiteral();
            if (use_label === null) {
               use_label = this.extractIntegerLiteral();
               if (use_label === null)
                  this.throw_error(`Invalid for-each-object-with-label loop: the label must be a string literal or integer literal.`);
            }
            if (!this.extractWord("do"))
               this.throw_error(`Invalid for-each-object-with-label loop: expected the word "do".`);
            break;
         default:
            this.throw_error(`Invalid for-loop.`);
      }
      let loop = new MBlock;
      loop.type = use_type;
      if (use_label !== null)
         loop.forge_label = use_label;
      loop.set_start(start);
      this.block = this.block.insert_item(loop);
   }
   _handleKeywordFunction() { // call after "function "
      let start = this.token_pos;
      //
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`A function must have a name.`);
      if (!this.extractSpecificChar("("))
         this.throw_error(`Expected "(".`);
      if (!this.extractSpecificChar(")"))
         this.throw_error(`Expected ")". User-defined functions cannot have arguments.`);
      let created = new MBlock;
      created.type = block_type.function;
      created.name = name;
      created.set_start(start);
      this.block = this.block.insert_item(created);
   }
}