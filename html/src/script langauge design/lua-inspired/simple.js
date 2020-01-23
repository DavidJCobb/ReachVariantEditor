function _make_enum(list) {
   let out = {};
   for(let entry of list)
      out[entry] = Symbol(entry);
   return out;
}

/*

   TODO:
   
   Expect declarations need to decode both sides to MVariableReferences if possible. 
   (Aliases can wrap integer constants, so you can have an alias on the righthand 
   side; and of course the lefthand side must be a variable.)
   
   All tokens should store their start and end positions in the stream, along with 
   their line numbers. We will need this for error reporting during the second stage 
   of parsing.
   
    - I *think* I have this properly implemented for most things. I should double-
      check function calls as the righthand side of assignments.
   
    - I should also set this up for the root block and not just the blocks and 
      statements inside of it.
   
   ---------------------------------------------------------------------------------
   
   REMINDER: The second stage of parsing entails:
    
    - Making sure that MVariableReferences all point to valid variables (accounting 
      for aliases).
   
    - Making sure that assignments make sense: do not allow assigning to a constant 
      or to a read-only variable.
      
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
function is_word_char(c) {
   if (is_whitespace_char(c))
      return false;
   if (is_operator_char(c))
      return false;
   if (is_quote_char(c))
      return false;
   if (is_syntax_char(c))
      return false;
   return true;
}

function string_is_numeric(c) {
   return (!isNaN(+c) && c !== null);
}

class MParsedItem {
   constructor() {
      this.parent = null;
      this.owner  = null; // for items that appear in conditions
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
}

class MAlias extends MParsedItem {
   constructor(n, rhs) {
      super();
      this.name   = n;
      this.value  = null;
      this.target = null;
      let result = token_to_int_or_var(rhs);
      if (result instanceof MVariableReference)
         this.target = result;
      else
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
   }
   serialize() {
      let result = `expect ${this.name} == ${this.value}`;
      return result;
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
      this.index = -1; // NOTE: string literals allowed, since aliases can be constant ints
      if (index !== void 0 && index !== "" && index !== null) {
         if (!isNaN(+index))
            this.index = +index;
         else
            this.index = index;
      }
   }
   serialize() {
      if (+this.index === this.index && this.index < 0) {
         return this.name;
      }
      return `${this.name}[${this.index}]`;
   }
   validate_syntax() {
      if (!this.name.length)
         throw new Error("Part is nameless.");
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
}
class MVariableReference {
   constructor(text) {
      this.parts = []; // std::vector<MVariablePart>
      if (text)
         this.extract(text);
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
               this.parts.push(new MVariablePart(name, index));
               name  = "";
               index = "";
               continue;
            }
            name += c;
         }
      }
      if (!name) // (text) ended in "." or was empty string
         throw new Error(`Variables cannot end in ".", and cannot be nameless.`);
      this.parts.push(new MVariablePart(name, index));
      //
      for(let i = 0; i < this.parts.length; i++)
         this.parts[i].validate_syntax();
      return true;
   }
   serialize() {
      let list = this.parts.map(function(v) { return v.serialize(); });
      return list.join(".");
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
         this.name = text;
      } else {
         this.context = new MVariableReference;
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
      let result = `${this.target.serialize()} ${this.operator.serialize()} `;
      if (this.source instanceof MVariableReference || this.source instanceof MFunctionCall)
         result += this.source.serialize();
      else if (+this.source === this.source)
         result += this.source;
      else
         throw new Error("unhandled");
      return result;
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
            this.statement.target = token_to_int_or_var(this.token);
         } catch (e) {
            this.throw_error(e.message);
         }
         this.reset_token();
         this.token = c;
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
            if (!this.call.extract_stem(this.token))
               this.throw_error(`Invalid function context and/or name: "${this.token}".`);
            this.statement.source = this.call;
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
            this.statement.left = token_to_int_or_var(this.token);
         } catch (e) {
            this.throw_error(e.message);
         }
         this.reset_token();
         this.token = c;
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
         return true;
      }
      let word = this.extractWord();
      if (word) {
         this.call.args.push(token_to_int_or_var(word));
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