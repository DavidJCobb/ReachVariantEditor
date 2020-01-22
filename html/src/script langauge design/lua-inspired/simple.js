function _make_enum(list) {
   let out = {};
   for(let entry of list)
      out[entry] = Symbol(entry);
   return out;
}

/*

   TODO:
   
   Finish implementing for-each-object-with-label loops: we need code to search for 
   and read either an integer literal or a string literal, which we then need to 
   commit as the forge label for the new block.
   
   We need to improve our parsing of functions so that func(a, ) is a syntax error 
   (comma not followed by an argument).
   
   If an assignment or comparison statement ends at EOF, we never end up saving it 
   to the block being parsed; the statement is lost. This needs to be fixed.
   
   If we hit EOF and the current block isn't the root block, then we should fail with 
   an error (unclosed block(s)).
   
   Consider splitting the parsing of conditions into a separate function away from 
   (parse). Condition parsing should forbid blocks, forbid assignments, permit the 
   "and" and "or" keywords, and stop upon encountering the "then" keyword. The "do" 
   keyword should cause a unique syntax error reminding authors to use "then" to 
   open the body of the if-block instead.
   
    - Once we have code for this, we can implement the "and", "or", and if-block 
      keywords. Those are the last remaining keywords.
      
    - When handling "elseif" and "else," we should check whether the block we're about 
      to close is an "if" or "elseif" block; if not, fail with an error. (If the last 
      element in that block is an if- or elseif block, then the error message should 
      clarify that you do not use "end elseif" or "end else".)
   
   When an MAssignment or MComparison is committed to a block, the lefthand side 
   and (when not a function call) the righthand side should be "decoded," i.e. 
   converted from string literals to the appropriate object (an instance of a class 
   representing an integer constant, or an instance of MVariableReference).
   
   All tokens should store their start and end positions in the stream, along with 
   their line numbers. We will need this for error reporting during the second stage 
   of parsing.
   
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

const token_type = _make_enum([
   "none",
   "constant_int",
   "constant_string",
   "word",
   "variable",
]);

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
      this.range  = [-1, -1];
   }
}

class MAlias extends MParsedItem {
   constructor(n, v) {
      super();
      this.name  = n;
      this.value = v;
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
   insert_item(i) {
      this.items.push(i);
      i.parent = this;
      return i;
   }
}
class MExpect extends MParsedItem {
   constructor(n, v) {
      super();
      this.name  = n;
      this.value = v;
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
}
class MVariableReference {
   constructor() {
      this.parts = []; // std::vector<MVariablePart>
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
               return false;
            if (c == "]") {
               is_index = false;
               if (index === "") // "name[]" is a syntax error
                  return false;
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
         return false;
      this.parts.push(new MVariablePart(name, index));
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
}

class MAssignment extends MParsedItem {
   constructor() {
      super();
      this.target   = null; // MVariableReference
      this.source   = null; // MVariableReference, MFunctionCall, or constant integer
      this.operator = null; // MOperator
   }
}
class MComparison extends MParsedItem {
   constructor() {
      super();
      this.left     = null; // MVariableReference
      this.right    = null; // MVariableReference
      this.operator = null; // MOperator
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
      this.token_end = false; // whether we hit whitespace, meaning that the current word is "over." if so, then another word character indicates {word word} which is bad syntax (outside of keywords)
      this.statement = null;  // current statement
      this.call      = null;  // current call (statement or assignment righthand side)
   }
   throw_error(text) {
      throw new Error(`Error on or near line ${this.line + 1} col ${this.pos - this.last_newline + 1}: ` + text);
   }
   //
   backup_stream_state() {
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
   //
   scan(functor) {
      let text    = this.text;
      let length  = text.length;
      let comment = false;
      for(; this.pos < length; ++this.pos) {
         let c = text[this.pos];
         if (c == "\n") {
            ++this.line;
            this.last_newline = this.pos;
            if (comment) {
               comment = false;
               continue;
            }
         }
         if (comment)
            continue;
         if (c == "-" && this.pos < length - 1 && text[this.pos + 1] == "-") {
            comment = true;
            continue;
         }
         if (functor(c))
            break;
      }
   }
   //
   parse(text, is_condition) {
      this.text = text;
      if (!this.root)
         this.root = new MBlock;
      this.block = this.root;
      //
      this.scan((function(c) {
         //
         // If we're not in a statement, then the next token must be a word. If that word is a 
         // keyword, then we handle it accordingly. If it is not a keyword, then it must be 
         // followed either by an operator (in which case we're in a compare or assign statement) 
         // or by an opening parentheses (in which case the statement is a function call).
         //
         if (!this.statement) {
            this._parseStatementStart(c, is_condition)
            return;
         }
         //
         // If, on the other hand, we're in a statement, then we can finish things up based on 
         // what kind of statement we're in.
         //
         if (this.call) {
            //
            // Function calls can exist as standalone statements or as the righthand side of 
            // assignment statements. In both cases, the end of the function call is the end 
            // of the statement. (You can assign the result of a function call to a variable 
            // (or rather, that's the syntax we're going with for specific trigger opcodes), 
            // but we don't support arbitrary expressions so things like (a = b() + c) won't 
            // be valid.)
            //
            this._parseFunctionCall(c);
            return;
         }
         if (!this.call && this.statement instanceof MAssignment) {
            this._parseAssignment(c);
            return;
         }
         if (this.statement instanceof MComparison) {
            this._parseComparison(c);
            return;
         }
      }).bind(this));
      //
      return this.root;
   }
   _parseStatementStart(c, is_condition) {
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
         return;
      }
      if (is_whitespace_char(c)) {
         this.token_end = true;
         //
         // Handle keywords here, if appropriate.
         //
         let handler = this.getHandlerForKeyword(this.token);
         if (handler) {
            if (is_condition)
               //
               // TODO: revise; we need to allow (or) and (and)
               //
               this.throw_error(`Keyword ${this.token} cannot appear inside of a condition.`);
            let prior = this.pos;
            handler.call(this);
            this.token     = "";
            this.token_end = false;
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
         if (!this.call.extract_stem(this.token))
            this.throw_error(`Invalid function context and/or name: "${this.token}".`);
         this.token     = "";
         this.token_end = false;
         return;
      }
      if (c == ")" || c == ",")
         this.throw_error(`Unexpected ${c}.`);
      if (is_operator_char(c)) {
         if (is_condition) {
            this.statement = new MComparison;
            this.statement.left = this.token;
         } else {
            this.statement = new MAssignment;
            this.statement.target = this.token;
         }
         this.token     = c;
         this.token_end = false;
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
   _parseFunctionCall(c) {
      //
      // Function calls can exist as standalone statements or as the righthand side of 
      // assignment statements. In both cases, the end of the function call is the end 
      // of the statement. (You can assign the result of a function call to a variable 
      // (or rather, that's the syntax we're going with for specific trigger opcodes), 
      // but we don't support arbitrary expressions so things like (a = b() + c) won't 
      // be valid.)
      //
      if (c == ",") {
         if (!this.token)
            this.throw_error(`Unexpected ${c}.`); // func(a, , b) is not valid
         this.call.args.push(this.token);
         this.token     = "";
         this.token_end = false;
         return;
      }
      if (c == ")") {
         //
         // TODO: detect and handle syntax error: func(a, )
         //
         if (this.token)
            //
            // The (token) variable is, here, the raw argument, e.g. "3" or "\"abc\"". 
            // At some point we should give MFunctionCall an "append argument" method 
            // which takes the token and normalizes it into some object (e.g. a class 
            // for constant integers, a class for string literals, a class for variables, 
            // a class for built-in terms like MP object types, and so on).
            //
            this.call.args.push(this.token);
         this.token     = "";
         this.token_end = false;
         //
         // End the statement.
         //
         this.block.insert_item(this.statement);
         this.call      = null;
         this.statement = null;
         return;
      }
      if (is_whitespace_char(c)) {
         if (this.token.length)
            this.token_end = true;
         return;
      }
      if (this.token_end)
         this.throw_error(`Statements of the form {word word} are not valid.`);
      this.token += c;
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
         this.token = "";
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
            this.token     = "";
            this.token_end = false;
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
      // TODO: We'll end up "losing" this statement if we hit EOF
      //
      this.statement.source = this.token;
      this.block.insert_item(this.statement);
      this.statement = null;
      this.token     = "";
      this.token_end = false;
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
         this.token = "";
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
      // TODO: We'll end up "losing" this statement if we hit EOF
      //
      this.statement.right = this.token;
      this.block.insert_item(this.statement);
      this.statement = null;
      this.token     = "";
      this.token_end = false;
   }
   //
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
   getHandlerForKeyword(word) {
      word = word.toLowerCase();
      switch (word) {
         case "alias":    return this._handleKeywordAlias;
         case "and":      return null; // TODO
         case "do":       return this._handleKeywordDo;
         case "else":     return null; // TODO
         case "elseif":   return null; // TODO
         case "end":      return this._handleKeywordEnd;
         case "expect":   return this._handleKeywordExpect;
         case "for":      return this._handleKeywordFor;
         case "function": return this._handleKeywordFunction;
         case "or":       return null; // TODO
         case "then":     return null; // TODO
      }
   }
   //
   _handleKeywordAlias() { // call after "alias "
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`An alias declaration must supply a name.`);
      if (!this.extractSpecificChar("="))
         this.throw_error(`Expected "=".`);
      let target = this.extractWord();
      if (!target.length)
         this.throw_error(`An alias declaration must supply a target.`);
      this.block.insert_item(new MAlias(name, target));
   }
   _handleKeywordDo() {
      let created = new MBlock;
      created.type = block_type.basic;
      this.block = this.block.insert_item(created);
   }
   _handleKeywordEnd() {
      this.block = this.block.parent;
      if (!this.block)
         this.throw_error(`Unexpected "end".`);
   }
   _handleKeywordExpect() { // call after "expect "
      let name = this.extractWord();
      if (!name.length)
         this.throw_error(`An expect declaration must supply a name.`);
      if (!this.extractSpecificChar("="))
         this.throw_error(`Expected "=".`);
      let target = this.extractWord();
      if (!target.length)
         this.throw_error(`An expect declaration must supply a target.`);
      this.block.insert_item(new MExpect(name, target));
   }
   _handleKeywordFor() { // call after "for "
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
            //
            // TODO: label as string literal or integer index
            //
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
      this.block = this.block.insert_item(loop);
   }
   _handleKeywordFunction() {
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
      this.block = this.block.insert_item(created);
   }
}