/*

   PARSER OVERVIEW:
   
   The first parsing "pass" takes all of the script text and generates instances of 
   MBlock, MAlias, MVariableReference, MFunctionCall, and so on, representing each 
   token in the script.
   
   The second parsing "pass" iterates over all of these tokens and both analyzes 
   and validates them:
   
    - MVariableReferences are decoded. Each MVariablePart has its "archetype" and 
      "object" identified (with alias invocations resolved); after this, the 
      variable reference identifies its type and passes itself to a "resolve" 
      function on that type. The ultimate aim is to make it so that we have the 
      data needed to build instances of the correct opcode-arg class in memory: 
      we need to know the value type and, for variables, the "scope," "which," 
      and "index."
   
    - Function calls are validated to ensure that the script doesn't call non-
      existent functions, and to ensure that opcodes are called correctly.
      
       = TODO

*/


/*

   TODO:
   
   Currently, invalid alias names (e.g. "name[1]" or integer constants) fail during 
   first-pass parsing. Consider having them fail during second-pass parsing (i.e. 
   non-fatal errors, so other problems in the script can also be reported at the 
   same time) instead. (Aliases that shadow keywords are an exception and should 
   always fail during first-pass parsing.)
   
    - Just fix this when we port to C++.
   
   Strongly consider renaming all "validate" member functions to "analyze", since we 
   also need to do things like resolving MVariableReferences that include aliases, 
   and tracking what functions are in scope.
   
    - Just fix this when we port to C++.
   
   If an alias targets a variable that doesn't exist, it should fail second-pass 
   parsing. If there is a function by the targeted name, the error message should 
   note that functions can't be aliased.
   
    - TODO: DON'T FORGET THIS
    
   Right now, aliases (and other objects) don't have anything to signal successful 
   validation versus failed validation; invocations can end up matching an invalid 
   alias, leading to cascading failures. AT A MINIMUM aliases need to remember 
   whether they are valid, so that invocations of an invalid alias fail with a 
   SENSIBLE AND INFORMATIVE error message.
   
    - Just fix this when we port to C++.
   
   Keywords, variable parts, and so on need to be case-insensitive.
   
    - Just fix this when we port to C++.
   
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   
   Do we want to allow integer-aliases as forge label indices (i.e. in for-each-
   object-with-label loops)?
   
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   
   See if we can replace our custom Collection with Map. Apparently the latter is 
   supposed to be accessible both by key and by iteration (i.e. for...of).
   
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   
   HOW SHOULD WE HANDLE VECTORS, FORMAT STRINGS AND TOKENS, AND SIMILAR MULTI-PART 
   ARGUMENTS IN FUNCTIONS?
   
    - I think we should use nested parentheses: if we encounter an opening paren 
      while parsing a function call's arguments, then we blindly consume everything 
      up to the next closing paren, and we store all of that as a string inside of 
      an "MUndifferentiatedFunctionCallArg". Then, during second-pass parsing, when 
      we've identified the function and we know what type of argument was supposed 
      to be there, we can choose a functor to parse that string (e.g. parse it as 
      a list of flags, or a format string and tokens, or a vector3, etc.).
      
      We can use a uniform syntax for all cases I can think of: comma-separated 
      values inside of the parens. That works for vector coordinates, for flags in 
      a mask, and for a format string and its tokens.
      
       = WE HAVE CREATED THE MUndifferentiatedFunctionCallArg CLASS AND ADDED 
         SUPPORT FOR IT TO THE FIRST-PASS PARSER.
      
       - The function to parse argument types needs to be able to handle aliases.
       
       - "Shape" and "format string token" arguments can contain/be variable 
         references.
      
       - Do we want a flags-mask to have to be wrapped in parentheses even when 
         only one flag is being used?
      
       - Do we want a format-string-and-tokens to have to be wrapped in parenthe-
         ses even when there are no tokens and it's just the one format string?
      
       - The Alpha Zombies sample script will need to be updated accordingly; it 
         calls some functions that use flags masks.
         
   Bring in the opcode database.
   
    - We need to map the native representation to the script representation. The 
      naive approach is to make it so that every opcode is either a function call 
      or a property; however, some opcodes can be both. For example, the opcode 
      to identify which player killed another player can be accessed as either of:
      
         alias killer = global.player[0]
         alias victim = global.player[1]
         
         -- Function form:
         killer = no_player
         killer = victim.try_get_killer()
         
         -- Property form:
         killer = victim.killer
      
      where in this case, the property form implicitly sets (killer) to (no_player) 
      before running the opcode (i.e. we compile an assignment followed by the 
      opcode, producing identical code to the function form). To effect this, we 
      need to give every opcode:
      
       - Primary function name.
       
       - Secondary function name, or blank.
       
       - A flag that, if set, causes us to default-initialize the out-argument if 
         the opcode is used via the secondary function name. "Default initialization" 
         here means compiling an assign statement before the opcode call, wherein we 
         set the out-argument to the respective "none" value for its type (no_team, 
         no_player, no_object).
         
          - The goal here is to accommodate the "get player killer" opcode. If it's 
            invoked as the primary function name, (player.try_get_killer), then we 
            just call it directly. If it's invoked as the secondary function name, 
            (player.get_killer), then we zero out the out-variable before calling 
            it, as a nice convenience thing for script authors.
      
      Unrelated to that situation, each opcode also needs:
      
       - The native-index of the "this" argument, if any.
      
       - An array containing native-indices; this will map argument indices to the 
         script argument order.
      
       - Potentially, a "no discard" flag. We don't yet know if you can pass a 
         none value as an out-argument (i.e. create object and store it in variable 
         no_object) and we don't know if this varies from opcode to opcode.
   
    - Validate all function calls: ensure that calls are to valid functions, be 
      they built-in or user-defined.
      
       - Validate function call arguments.
       
          - If a function's only argument is a flags mask, if a call to it has 
            multiple arguments, and if any argument is the name of a flag, then 
            have the error message mention that flags masks ALWAYS have to be 
            wrapped in parentheses, i.e. player.killed_by((kill, suicide)).
      
    - Validate all function declarations: do not allow user-defined functions to 
      shadow built-in functions.
    
   MFunctionCall arguments will need special-case handling to account for format 
   string tokens, enum values, and so on. We'll need special-case alias handling 
   here as well; I want it to be possible to alias entries in the built-in enums.
   
   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   
   Once the compiler is implemented, we need to test whether it's possible to call 
   a non-subroutine trigger; this will impact whether we allow function declarations 
   to also be event handlers (see comments in the parser's "function" keyword 
   handler).
   
   (I mean, we need to test whether it works properly in-game.)
   
   ---------------------------------------------------------------------------------
   
   C++ TODO: If the (mpvr) block turns out to exceed the allowed size limit, can we 
   work around that by writing it as a compressed (_cmp) block? (If not, then why 
   the hell did Bungie ever even compress some mpvr blocks?) A quick way to test 
   this would be to examine some variants that use compressed blocks, like Halo 
   Chess, and see if they exceed the 0x5000-odd-byte cap when uncompressed.
   
   ---------------------------------------------------------------------------------
   
   NOTES:
   
   The following fails by design:
      
      alias is_zombie = player.number[0]
      alias indirect  = is_zombie
      
   The reason it fails is because if we define another (is_zombie) alias that is 
   NOT a relative alias on the "player" typename, then it will neither shadow nor 
   be shadowed by the (is_zombie) alias in the sample code above; this means that 
   the only way to be absolutely sure what alias (indirect) is meant to chain is 
   to specify the typename. In other words, this is the correct syntax:
      
      alias is_zombie = player.number[0]
      alias indirect  = player.is_zombie
   
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
   "for_each_object",
   "for_each_object_with_label",
   "for_each_player",
   "for_each_player_randomly",
   "for_each_team",
   "function",
]);
const block_event = _make_enum([
   "none",
   "init",
   "local_init",
   "host_migration",
   "double_host_migration",
   "object_death",
   "local",
   "pregame",
]);

function is_operator_char(c) {
   return ("=<>!+-*/%&|~^").indexOf(c) >= 0;
}
function is_quote_char(c) {
   return ("`'\"").indexOf(c) >= 0;
}
function is_syntax_char(c) {
   return ("(),:").indexOf(c) >= 0;
}
function is_whitespace_char(c) {
   return (" \r\n\t").indexOf(c) >= 0;
}

function is_keyword(word) {
   word = word.toLowerCase();
   switch (word) {
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
      case "on":
      case "or":
      case "then":
         return true;
   }
   return false;
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
      this.target = null; // MVariableReference
      {  // Validate name.
         if (is_keyword(n))
            throw new Error(`Keyword "${n}" cannot be used as the name of an alias.`);
         //
         // TODO: Do we really want these next errors to be fatal parse errors? Seems like stuff we could 
         // just catch during the second-pass parsing.
         //
         if (n.match(/[\[\]\.]/))
            throw new Error(`Invalid alias name "${n}". Alias names cannot contain square brackets or periods.`);
         if (!n.match(/[^0-9]/))
            throw new Error(`Invalid alias name "${n}". You cannot alias an integer constant.`);
         let nl = n.toLowerCase();
         let f  = function(e) { return e.name == nl; }
         if (namespaces.find(f) || types.find(f) || namespaces.unnamed.members.find(f))
            throw new Error(`Invalid alias name "${n}". You cannot shadow a built-in namespace or keyword.`);
      }
      let result = token_to_int_or_var(rhs);
      if (result instanceof MVariableReference) {
         this.target = result;
         result.owner = this;
      } else
         this.value = result;
   }
   serialize() {
      return `alias ${this.name} = ${this.target.serialize()}`;
   }
   validate(validator) {
      if (this.target) {
         this.target.validate(validator, true);
         let typename = this.get_relative_typename();
         if (typename) {
            let n = this.name.toLowerCase();
            for(let property of typename.properties) {
               if (property.name != n)
                  continue;
               validator.report(this, `Invalid alias name "${this.name}". You cannot shadow "${typename.name}.${this.name}" or other built-in properties on built-in types.`);
               return false;
            }
         }
      }
   }
   //
   get_integer_constant() {
      if (this.target.is_integer_constant())
         return this.target.constant;
      return void 0;
   }
   get_target_parts() {
      if (!this.target)
         return [];
      return this.target.parts;
   }
   get_relative_typename() {
      if (!this.is_relative())
         return null;
      if (this.target.is_integer_constant())
         return null;
      let first = this.target.parts[0];
      return first.get_typename();
   }
   is_integer_constant() {
      return this.target.is_integer_constant();
   }
   is_relative() { // requires validation of the RHS
      if (!this.target)
         return false;
      return this.target.is_relative_alias();
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
      this.event       = block_event.none;
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
      validator.trackEventHandler(this);
      //
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
         item.validate(validator);
         if (item instanceof MAlias) { // TODO: only if it validates
            aliases.push(item);
            validator.trackAlias(item);
         }
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
class MVariablePart {
   constructor(name, index) {
      this.name  = name || "";
      this.index = null; // NOTE: string literals allowed, since aliases can be constant ints
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
   clone() {
      let out = new MVariablePart;
      out.name  = this.name;
      out.index = this.index;
      out.analyzed.archetype = this.analyzed.archetype;
      out.analyzed.object    = this.analyzed.object;
      return out;
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
         return true;
      return !!this.index;
   }
   serialize() {
      if (!this.index && this.index !== 0) {
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
   get_typename() {
      let object = this.analyzed.object;
      switch (this.analyzed.archetype) {
         case "namespace":
            return null;
         case "member":
         case "property":
            return object.type;
         case "alias_rel_typename":
         case "static":
         case "var":
            return object;
      }
      return null;
   }
   is_read_only() {
      let type = this.get_typename();
      if (type && type.always_read_only)
         return true;
      let object = this.analyzed.object;
      switch (this.analyzed.archetype) {
         case "member":
         case "property":
            return object.is_read_only;
         case "static":
            return true;
      }
      return false;
   }
   //
   resolve_alias(validator) { // returns: array of MVariablePart OR integer
      let i     = this.locate_me();
      let alias = null;
      let found = false; // found any matching name, even if it was invoked improperly
      if (i == 0) { // must be a fully-qualified name
         //
         // alias or alias.something: The alias must be an absolute alias, and cannot 
         // be indexed.
         //
         validator.forEachTrackedAlias((function(a) {
            if (a.is_relative())
               return;
            if (a.name == this.name) {
               found = true;
               alias = a;
               return true;
            }
         }).bind(this));
         if (!alias)
            return false;
         if (alias.is_integer_constant()) {
            if (this.owner.parts.length > 1) {
               validator.report(this, `Alias ${alias.name} refers to a constant integer and is not valid here.`);
               return false;
            }
            return alias.get_integer_constant();
         }
         if (!alias || this.has_index()) {
            validator.report(this, `Found one or more aliases named ${this.name}, but none were valid here.`);
            return false;
         }
         return alias.get_target_parts();
      }
      //
      // something.alias: The alias must be a relative alias, and cannot be indexed.
      //
      let prev = this.owner.parts[i - 1];
      assert(prev, "There should be a previous part."); // JS only
      if (prev.analyzed.archetype == "property")
         return false;
      let pTyp = prev.get_typename();
      validator.forEachTrackedAlias((function(a) {
         if (a.name == this.name) {
            found = true;
            if (!a.is_relative()) // absolute aliases are not valid here
               return;
            let type = a.get_relative_typename();
            if (type != pTyp)
               return;
            alias = a;
            return true;
         }
      }).bind(this));
      if (!alias || this.has_index()) {
         if (found)
            validator.report(this, `Found one or more aliases named ${this.name}, but none were valid here.`);
         return false;
      }
      return alias.get_target_parts().slice(1);
   }
   resolve_index(validator) { // returns: undefined if there is no index; otherwise: the index as a constant integer, OR null if there is no valid index
      if (!this.has_index())
         return;
      if (+this.index === this.index)
         return this.index;
      let found = false;
      let alias = null;
      let name  = this.index;
      //
      // NOTE: Keywords-as-indices are already checked for during stage-one parsing and are a fatal error. 
      // Don't bother checking for them here.
      //
      // NOTE: Absurd indices e.g. "abc-def-g" are already handled during MVariableReference.extract.
      //
      validator.forEachTrackedAlias((function(a) {
         if (a.is_relative())
            return;
         if (a.name == name) {
            found = true;
            if (a.is_integer_constant())
               alias = a;
            return true;
         }
      }).bind(this));
      if (!alias) {
         if (found)
            validator.report(this, `The alias named "${this.index}" is not an integer constant and thus can't be used as a variable index.`);
         else
            validator.report(this, `Failed to find an alias named "${this.index}".`);
         return null;
      }
      return (this.index = alias.get_integer_constant());
   }
   //
   _extract_as_member(validator, namespace) { // returns true if no errors
      let name  = this.name;
      let match = namespace.members.find(function(e) { return e.name == name });
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
            if (this.index < 0) {
               validator.report(this, `The index cannot be negative.`);
               return false;
            }
            //
            // TODO: Apply (match.soft_max_index).
            //
         }
         return true;
      }
      return true;
   }
   _extract_as_property(validator, typename) {
      let name  = this.name;
      let match = typename.properties.find(function(e) { return e.name == name; });
      if (match) {
         this.analyzed.archetype = "property";
         this.analyzed.object    = match;
         if (match.hard_max_index == 0) {
            if (this.has_index()) {
               validator.report(this, `Property "${match.name}" on ${typename.name} variables cannot be indexed.`);
               return false;
            }
         } else {
            if (!this.has_index()) {
               validator.report(this, `Property "${match.name}" on ${typename.name} variables must be indexed.`);
               return false;
            }
            if (this.index < 0) {
               validator.report(this, `The index cannot be negative.`);
               return false;
            }
            //
            // TODO: Apply (match.soft_max_index).
            //
         }
      }
      return true;
   }
   _extract_as_var(validator) { // returns true if no errors
      let name  = this.name;
      let match = types.find(function(e) { return e.is_variable && e.name == name; });
      if (match) {
         this.analyzed.archetype = "var";
         this.analyzed.object    = match;
         if (!this.has_index()) {
            validator.report(this, `Must specify an index; you need to indicate which ${match.name} variable you want.`);
            return false;
         }
         if (this.index < 0) {
            validator.report(this, `The index cannot be negative; you need to indicate which ${match.name} variable you want.`);
            return false;
         }
         //
         // TODO: Validate the index; ensure it doesn't exceed the upper bound given what 
         // scope we're accessing (or check that later if it's easier).
         //
      }
      return true;
   }
   validate(validator, is_alias_rhs) {
      {
         let result = this.resolve_index(validator);
         if (result === null && this.index !== null) // we have an index but it's not valid
            return false;
      }
      //
      // Validation during stage-two parsing.
      //
      let i     = this.locate_me(); // my index within my containing MVariableReference
      let name  = this.name;
      let match = null;
      if (i == 0) {
         if (is_alias_rhs) {
            //
            // We're resolving an alias declaration. We want to support typename-relative 
            // aliases, such as (alias is_zombie = player.number[0]), where you could 
            // then access (any_player_reference.is_zombie).
            //
            if (!this.has_index()) {
               match = types.find(function(e) { return e.is_nestable && e.name == name; });
               if (match) {
                  this.analyzed.archetype = "alias_rel_typename";
                  this.analyzed.object    = match;
                  return true;
               }
            }
         }
         //
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
            if (this.index < 0) {
               validator.report(this, `Invalid index ${this.index}: negative ${match.name} indices are not allowed.`);
               return false;
            }
            if (this.index >= match.static_limit) {
               validator.report(this, `Invalid index ${this.index}: there are only ${match.static_limit} of this object type, and indices start with zero, so the maximum index is ${match.static_limit - 1}.`);
               return false;
            }
            return true;
         }
         if (!this._extract_as_member(validator, namespaces.unnamed))
            return false;
         if (this.analyzed.archetype) // _extract_as_member did indeed find that we are a namespace member
            return true;
         validator.report(this, `"${name}" is not a namespace or static-type.`);
         return false;
      }
      let prev = this.owner.parts[i - 1];
      assert(prev, "There should be a previous part."); // JS only
      let pTyp = prev.get_typename();
      let pObj = prev.analyzed.object;
      if (pTyp && !pTyp.is_nestable) {
         validator.report(this, `Variables of type ${pTyp.name} cannot contain nested variables.`);
         return false;
      }
      switch (prev.analyzed.archetype) {
         case "namespace": { // we can be a member or a var
            //
            // Check for namespace.member:
            //
            if (!this._extract_as_member(validator, pObj))
               return false;
            if (this.analyzed.archetype) // _extract_as_member did indeed find that we are a namespace member
               return true;
            //
            // Check for namespace.var:
            //
            if (!this._extract_as_var(validator))
               return false;
            if (this.analyzed.archetype) // _extract_as_var did indeed find that we are a variable
               return true;
            validator.report(this, `The "${pObj.name}" namespace does not have a field named "${name}".`);
            return false;
         } break;
         case "member": { // we can be a property or a var
            let type = pObj.type;
            //
            // Check for [namespace.]member.property:
            //
            if (!this._extract_as_property(validator, type))
               return false;
            if (this.analyzed.archetype) // _extract_as_property did indeed find that we are a property
               return true;
            //
            // Check for [namespace.]member.var:
            //
            if (!this._extract_as_var(validator))
               return false;
            if (this.analyzed.archetype) // _extract_as_var did indeed find that we are a variable
               return true;
            validator.report(this, `Objects of type ${type.name} do not have a field named "${name}".`);
            return false;
         } break;
         case "alias_rel_typename":
         case "static": { // we can be a property or a var
            //
            // Check for static.property:
            //
            if (!this._extract_as_property(validator, pObj))
               return false;
            if (this.analyzed.archetype) // _extract_as_property did indeed find that we are a property
               return true;
            //
            // Check for static.var:
            //
            if (!this._extract_as_var(validator))
               return false;
            if (this.analyzed.archetype) // _extract_as_var did indeed find that we are a variable
               return true;
            validator.report(this, `Objects of type ${pObj.name} do not have a field named "${name}".`);
            return false;
         } break;
         case "var": { // we can be a property or, in some cases, a var
            //
            // Most properties are available on (namespace.var) but not (namespace.var.var); moreover, 
            // (namespace.var.var.var) is not allowed so we need to know whether the previous var is 
            // itself preceded by a var.
            //
            let is_second_var = i >= 3;
            //
            // Check for namespace.var.property and namespace.var.var.property:
            //
            if (!this._extract_as_property(validator, pObj))
               return false;
            if (this.analyzed.archetype) { // _extract_as_property did indeed find that we are a property
               if (is_second_var && !this.analyzed.object.allow_from_nested) {
                  validator.report(this, `Constructions of the form (namespace.var.var.${this.analyzed.object.name}) are not allowed; store (namespace.var.var) in a variable and then access through that variable.`);
                  return false;
               }
               return true;
            }
            //
            // Check for namespace.var.var:
            //
            if (!this._extract_as_var(validator))
               return false;
            if (this.analyzed.archetype) { // _extract_as_var did indeed find that we are a variable
               if (is_second_var) {
                  validator.report(this, `Constructions of the form (namespace.var.var.var) are not allowed; store (namespace.var.var) in a variable and then access through that variable.`);
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
      validator.report(this, `Failed to identify part #${i + 1} ("${this.serialize()}") in variable/property.`);
      return false;
   }
}
class MVariableReference { // represents a variable, keyword, or aliased integer
   constructor(arg) {
      this.parts    = []; // std::vector<MVariablePart>
      this.constant = 0;
      if (arg) {
         if (+arg === arg)
            this.constant = arg;
         else
            this.extract(arg);
      }
      this.analyzed = {
         type:  null,
         scope: null, // var_scope OR the _scope enums on the opcode arg value classes
         which: null,
         index: null,
         is_read_only: false,
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
            if (BAD_INDEX_CHARS.indexOf(c) >= 0) {
               if (!index.length && c == "-") { // negative indices are invalid but should not be a fatal parse error
                  index += c;
                  continue;
               }
               throw new Error(`Character ${c} is not valid inside of an index.`);
            }
            if (c == "]") {
               is_index = false;
               if (index === "") // "name[]" is a syntax error
                  throw new Error(`Variables of the form "name[]" are not valid. Specify an index if appropriate, or no square brackets at all otherwise.`);
               continue;
            }
            if (!index.length && is_whitespace_char(c))
               continue;
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
      if (this.is_integer_constant())
         return this.constant + "";
      let list = this.parts.map(function(v) { return v.serialize(); });
      return list.join(".");
   }
   toString() {
      return this.serialize();
   }
   //
   is_integer_constant() {
      return !this.parts.length;
   }
   is_read_only() {
      if (this.is_integer_constant())
         return true;
      if (this.analyzed.type)
         return this.analyzed.is_read_only;
      return void 0; // analysis was not yet conducted, so we don't know
   }
   is_relative_alias() {
      if (this.parts.length) {
         let first = this.parts[0];
         if (first.analyzed.archetype == "alias_rel_typename")
            return true;
      }
      return false;
   }
   is_single() {
      return this.parts.length == 1 && !this.parts[0].has_index();
   }
   //
   validate(validator, is_alias_rhs) {
      if (this.is_integer_constant()) {
         this.analyzed.type  = types.number;
         this.analyzed.scope = number_var_scope.constant;
         this.analyzed.index = this.constant;
         return true;
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
      for(let i = 0; i < this.parts.length; i++) {
         validator.set_errors_suspended(true); // "suspend" the validator so we can discard error messages if appropriate
         let result = this.parts[i].validate(validator, is_alias_rhs);
         if (!result) {
            result = this.parts[i].resolve_alias(validator);
            if (+result === result) {
               validator.set_errors_suspended(false, true); // discard errors caught while suspended
               assert(this.parts.length == 1, "Can't handle integer alias here.");
               this.constant = result;
               this.parts    = [];
               break;
            }
            if (!result) {
               validator.set_errors_suspended(false, false);
               return false;
            }
            {
               let dupes = [];
               for(let i = 0; i < result.length; i++) {
                  let copy = result[i].clone();
                  copy.owner = this;
                  dupes.push(copy);
               }
               result = dupes;
            }
            //
            // Replace the part with the contents of the alias to which it referred:
            //
            Array.prototype.splice.apply(this.parts, ([i, 1]).concat(result)); // Array.splice sucks because it splices in raw args rather than an array
            //
            // And now validate the first of those contents:
            //
            validator.set_errors_suspended(false, true); // discard errors caught while suspended
            result = this.parts[i].validate(validator, is_alias_rhs);
            if (!result) {
               return false;
            }
         }
         validator.set_errors_suspended(false, false); // un-suspend
      }
      if (this.parts.length) {
         let last = this.parts[this.parts.length - 1];
         let type = last.get_typename();
         this.analyzed.type         = type;
         this.analyzed.is_read_only = last.is_read_only();
         if (type && !is_alias_rhs) {
            let func = type.resolve_function;
            if (!func && type.underlying_type) {
               this.analyzed.type = type.underlying_type;
               func = type.underlying_type.resolve_function;
            }
            if (func)
               func(this);
         }
      }
      //
      // TODO: Set (type), (scope), (which), and (index) on (this.analyzed) according to what 
      // conclusion we drew from each part.
      //
      // member                           | type and index depend on the member; scope is global
      // member[a]                        | used for script options. member determines the type and scope; no which; index == a
      // member.var[a]                    | scope and which depend on the member; type depends on the var; index == a
      // namespace.var[a].var[b]          | var[b] determines the type; var[a] determines the scope; which == a; index == b
      // namespace.var[a].property        | property determines the scope and type; no which; index == a
      // namespace.var[a].property[b]     | used for script stats. property determines the scope and type; which == a; index == b
      // namespace.var[a].var[b].property | property determines the scope and type; which == a; index == b
      // static[a]                        | type depends on the static; scope is global; no which; index == a
      // static[a].var[b]                 | var[b] determines the type; static is the scope; which == a; index == b
      // static[a].property               | property determines the scope and type; no which; index == a
      // game.property                    | type and scope depend on the property; no which; no index
      //
      // Don't bother trying to come up with a generic approach; instead, just give each typename 
      // object a handler function that takes the MVariableReference as an argument and sets the 
      // scope, which, and index values on it appropriately.
      //
      // NOTES:
      //
      //  - We need to verify that any indices are in range. Different variable scopes have different 
      //    counts of each variable.
      //
      return true;
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
   identify_root() {
      //
      // Helper function for identifying the (scope), (which), and (index) for certain variables. 
      // This function will properly identify the root of any MVariableReference that begins with:
      //
      //    global.object[n]
      //    global.player[n]
      //    global.team[n]
      //    unnamed_ns_object
      //    unnamed_ns_player
      //    unnamed_ns_team
      //    player[n]
      //    team[n]
      //
      // The sequences above are the "roots" that this function can identify; that is: they are 
      // nestable variables that are (internally considered) globally-scoped. The function returns: 
      // the number of parts that the "root" consists of; the typename for the root; and the root's 
      // identifier within its respective enum (megalo_objects, megalo_players, or megalo_teams).
      //
      // This function should be called by (resolve_function) functions provided to MScriptTypename 
      // instances.
      //
      let first  = this.parts[0] ? this.parts[0].analyzed.object : null;
      let second = this.parts[1] ? this.parts[1].analyzed.object : null;
      if (first instanceof MScriptNamespace) {
         if (!second)
            return { part_count: 0, type: null, id: null };
         if (first != namespaces.global)
            return { part_count: 0, type: null, id: null };
         for(let member of first.members) { // namespace.member
            if (member.name == second.name) {
               let id   = null;
               let type = member.type;
               switch (type) {
                  case types.object: id = member.megalo_object; break;
                  case types.player: id = member.megalo_player; break;
                  case types.team:   id = member.megalo_team;   break;
               }
               return { part_count: 2, type: type, id: id };
            }
         }
         for(let type of types) { // namespace.var
            if (type.name == second.name) {
               let part = this.parts[1];
               let id   = null;
               switch (type) {
                  case types.object: id = megalo_objects["global_object_" + part.index]; break; // C++: enum value + index
                  case types.player: id = megalo_players["global_player_" + part.index]; break; // C++: enum value + index
                  case types.team:   id = megalo_teams  ["global_team_"   + part.index]; break; // C++: enum value + index
               }
               return { part_count: 2, type: type, id: id };
            }
         }
         return { part_count: 0, type: null, id: null };
      } else {
         for(let member of namespaces.unnamed.members) { // member
            if (member.name == first.name) {
               let id   = null;
               let type = member.type;
               switch (type) {
                  case types.object: id = member.megalo_object; break;
                  case types.player: id = member.megalo_player; break;
                  case types.team:   id = member.megalo_team;   break;
               }
               return { part_count: 1, type: type, id: id };
            }
         }
         if (first instanceof MScriptTypename) { // static
            let part  = this.parts[0];
            let index = part.index;
            let id    = null;
            switch (first) {
               case types.player: id = megalo_players["player_" + (index + 1)]; break; // C++: enum value + index
               case types.team:   id = megalo_teams["team_" + (index + 1)];     break; // C++: enum value + index
            }
            return { part_count: 1, type: first, id: id };
         }
      }
      return { part_count: 0, type: null, id: null };
   }
}

class MUndifferentiatedFunctionCallArg extends MParsedItem {
   constructor(text) {
      super();
      this.content  = text || "";
      this.resolved = null;
   }
   serialize() {
      if (!this.resolved)
         return `(${this.content})`;
      //
      // TODO
      //
   }
   validate(validator) {
      //
      // TODO
      //
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
         //
         // TODO: Fail on "1234.func()"
         //
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
         if (arg instanceof MVariableReference || arg instanceof MUndifferentiatedFunctionCallArg || arg instanceof MStringLiteral) {
            result += arg.serialize();
         } else
            throw new Error("unhandled");
      }
      result += ")";
      return result;
   }
   validate(validator) {
      let func = null;
      if (this.context) {
         this.context.validate(validator);
         //
         // TODO: if the context is valid, verify that the member function we're trying 
         // to call exists. Identify the function as (func).
         //
      } else {
         //
         // TODO: Verify that the global function we're trying to call exists (i.e. a 
         // user-defined function or an opcode). Identify the function as (func).
         //
         // User-defined functions cannot have arguments, so in that case, fail if we 
         // have any.
         //
      }
      for(let arg of this.args) {
         if (arg instanceof MVariableReference) {
            if (arg.is_single()) {
               //
               // TODO: Handle the case of this being an enum value, single flag, etc..
               //
            }
            //
            // TODO
            //
         }
         //
         // TODO
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
         return new MVariableReference(+text);
   }
   return new MVariableReference(text);
}

class MAssignment extends MParsedItem {
   constructor() {
      super();
      this.target   = null; // MVariableReference
      this.source   = null; // MVariableReference or MFunctionCall
      this.operator = null; // MOperator
   }
   serialize() {
      let result = "";
      if (this.target instanceof MVariableReference || this.target instanceof MFunctionCall)
         result += this.target.serialize();
      else
         throw new Error("unhandled");
      result = `${result} ${this.operator.serialize()} `;
      if (this.source instanceof MVariableReference || this.source instanceof MFunctionCall)
         result += this.source.serialize();
      else
         throw new Error("unhandled");
      return result;
   }
   validate(validator) {
      if (this.target instanceof MVariableReference) {
         this.target.validate(validator);
         if (this.target.is_integer_constant())
            validator.report(this, "You cannot assign to a constant integer.");
         else if (this.target.is_read_only())
            validator.report(this, "You cannot assign to this variable/property. It is read-only.");
      }
      if (this.source instanceof MParsedItem || this.source instanceof MVariableReference) {
         this.source.validate(validator);
      }
   }
}
class MComparison extends MParsedItem {
   constructor() {
      super();
      this.left     = null; // MVariableReference
      this.right    = null; // MVariableReference
      this.operator = null; // MOperator
   }
   serialize() {
      let result = "";
      if (this.left instanceof MVariableReference)
         result += this.left.serialize();
      else
         throw new Error("unhandled");
      result += ` ${this.operator.serialize()} `;
      if (this.right instanceof MVariableReference)
         result += this.right.serialize();
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
      this.root        = null;  // root block
      this.block       = null;  // block we're currently parsing
      this.next_event  = block_event.none; // event for next block
      this.token       = "";    // current token being parsed (lefthand word / operator / call argument / righthand word)
      this.token_pos   = null;  // stream position of current token (as reported by backup_stream_state)
      this.token_end   = false; // whether we hit whitespace, meaning that the current word is "over." if so, then another word character indicates {word word} which is bad syntax (outside of keywords)
      this.token_brace = false; // whether we're in square brackets; needed to properly handle constructs like "abc[ d]" or "abc[-1]" at the starts of statements
      this.statement   = null;  // current statement
      this.call        = null;  // current call (statement or assignment righthand side)
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
      this.token       = "";
      this.token_pos   = null;
      this.token_end   = false;
      this.token_brace = false;
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
      if (this.next_event != block_event.none) {
         this.throw_error(`The file ended with an "on" keyword but no following block.`);
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
      if (c == "[") {
         this.token_brace = true;
         this.token += c;
         return;
      }
      if (this.token_brace) {
         if (c == "]")
            this.token_brace = false;
         this.token += c;
         return;
      }
      if (is_whitespace_char(c)) {
         this.token_end = true;
         //
         // Handle keywords here, if appropriate.
         //
         let handler = null;
         switch (this.token) {
            case "and":
            case "or":
            case "then":
               this.throw_error(`The "${this.token}" keyword cannot appear here.`);
               break;
            case "alias":    handler = this._handleKeywordAlias;    break;
            case "do":       handler = this._handleKeywordDo;       break;
            case "else":     handler = this._handleKeywordElse;     break;
            case "elseif":   handler = this._handleKeywordElseIf;   break;
            case "end":      handler = this._handleKeywordEnd;      break;
            case "expect":   handler = this._handleKeywordExpect;   break;
            case "for":      handler = this._handleKeywordFor;      break;
            case "function": handler = this._handleKeywordFunction; break;
            case "if":       handler = this._handleKeywordIf;       break;
            case "on":       handler = this._handleKeywordOn;       break;
         }
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
         this.statement.source.owner = this.statement;
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
      if (c == "[") {
         this.token_brace = true;
         this.token += c;
         return;
      }
      if (this.token_brace) {
         if (c == "]")
            this.token_brace = false;
         this.token += c;
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
            case "on":
               this.throw_error(`You cannot mark event handlers inside of conditions.`);
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
      if (this.extractSpecificChar("(")) { // find MUndifferentiatedFunctionCallArg
         let found = "";
         let state = this.backup_stream_state();
         this.scan((function(c) {
            if (c == "(")
               this.throw_error(`Parentheses can only be used to mark the start and end of a function's arguments, or within arguments to mark the start and end of a multi-part argument.`);
            if (c == ")")
               return true;
            found += c;
         }).bind(this));
         if (!found.length) {
            this.restore_stream_state(state);
            this.throw_error(`Unterminated multi-part function argument.`);
         }
         ++this.pos; // move position to after the closing paren
         //
         let arg = new MUndifferentiatedFunctionCallArg(found);
         this.call.args.push(arg);
         arg.owner = this.call;
         arg.set_start(state);
         arg.set_end(this.pos);
         return true;
      }
      //
      let integer = this.extractIntegerLiteral();
      if (integer !== null) {
         let arg = new MVariableReference(integer);
         this.call.args.push(arg);
         arg.owner = this.call;
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
         // if it is found. NOTE: square brackets aren't supported in this branch, because 
         // we should never end up wanting to find a specific parser-mandated variable name 
         // anywhere.
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
      let word  = "";
      let brace = false; // are we in square brackets?
      this.scan((function(c) {
         if (brace) {
            if (c == "]") {
               brace = false;
            }
            word += c;
            return;
         }
         if (is_whitespace_char(c))
            return word.length > 0;
         if ((".[]").indexOf(c) >= 0 && !word.length) { // these chars are allowed in a word, but not at the start of a word
            if (c == "[")
               brace = true;
            return true;
         }
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
      if (this.next_event) {
         created.event = this.next_event;
         this.next_event = block_event.none;
      }
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
      if (this.next_event) {
         block.event = this.next_event;
         this.next_event = block_event.none;
      }
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
               use_label = this.extractIntegerLiteral(); // TODO: Do we want to allow constant integer aliases here?
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
      if (this.next_event) {
         loop.event = this.next_event;
         this.next_event = block_event.none;
      }
      this.block = this.block.insert_item(loop);
   }
   _handleKeywordFunction() { // call after "function "
      let start = this.token_pos;
      //
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`A function must have a name.`);
      for(let c of name) {
         if (("[].").indexOf(c) >= 0)
            this.throw_error(`Unexpected ${c} inside of a function name.`);
         //
         // Other characters can occur inside of square brackets, but we don't need to test for 
         // them, because we're already testing for the brackets themselves.
      }
      if (is_keyword(name))
         this.throw_error(`Keyword "${n}" cannot be used as the name of a function.`);
      if (!this.extractSpecificChar("("))
         this.throw_error(`Expected "(".`);
      if (!this.extractSpecificChar(")"))
         this.throw_error(`Expected ")". User-defined functions cannot have arguments.`);
      let created = new MBlock;
      created.type = block_type.function;
      created.name = name;
      created.set_start(start);
      if (this.next_event) {
         //
         // TODO: Do we want to allow function bodies as events? I mean, I don't think the file 
         // format makes that impossible... Functions are basically going to be implemented as 
         // "subroutine"-entry-type triggers. Event handlers are implemented as a list of trigger 
         // indices stored top-level AND as entry type values on the individual triggers, which 
         // makes event handler status mutually exclusive with subroutine status; however, I don't 
         // yet know whether the game can or can't handle calling a non-subroutine trigger in a 
         // trigger action.
         //
         created.event = this.next_event;
         this.next_event = block_event.none;
      }
      this.block = this.block.insert_item(created);
   }
   _handleKeywordOn() {
      if (this.block != this.root)
         this.throw_error(`Only top-level (non-nested) blocks can be event handlers.`);
      //
      // VALID EVENT NAMES:
      //    double host migration
      //    host migration
      //    init
      //    local
      //    local init
      //    object death
      //    pregame
      //
      let words = [];
      let state = this.backup_stream_state();
      while (!this.extractSpecificChar(":")) {
         let word = this.extractWord();
         if (!word) {
            this.restore_stream_state(state);
            this.throw_error(`No valid event name specified.`);
         }
         words.push(word);
      }
      if (!words.length) {
         this.restore_stream_state(state);
         this.throw_error(`No valid event name specified.`);
      }
      let phrase = words.join(" ").toLowerCase();
      let event  = block_event.none;
      switch (phrase) {
         case "init":                  event = block_event.init;                  break;
         case "local init":            event = block_event.local_init;            break;
         case "host migration":        event = block_event.host_migration;        break;
         case "double host migration": event = block_event.double_host_migration; break;
         case "object death":          event = block_event.object_death;          break;
         case "local":                 event = block_event.local;                 break;
         case "pregame":               event = block_event.pregame;               break;
      }
      if (event == block_event.none) {
         //
         // TODO: Is there any chance we can make this a non-fatal parse error i.e. a second-pass error?
         //
         this.restore_stream_state(state);
         this.throw_error(`Invalid event name: "${words.join(" ")}".`);
      }
      this.next_event = event;
   }
}