function _make_enum(list) {
   let out = {};
   for(let entry of list)
      out[entry] = Symbol(entry);
   return out;
}

const token_type = _make_enum([
   "none",
   "constant_int",
   "constant_string",
   "word",
   "variable",
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

class MBlock extends MParsedItem {
   constructor() {
      super();
      this.items = []; // statements and blocks
   }
   insert_item(i) {
      this.items.push(i);
   }
}
class MVariablePart {
   constructor() {
      this.name  = "";
      this.index = -1; // NOTE: string literals allowed, since aliases can be constant ints
   }
}
class MVariableReference {
   constructor() {
      this.parts = []; // std::vector<MVariablePart>
   }
   extract(text) {
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
      this.root = null;
   }
   throw_error(text) {
      throw new Error(`Error on or near line ${this.line + 1} col ${this.pos - this.last_newline + 1}: ` + text);
   }
   extractChar(desired) {
      let text   = this.text;
      let length = text.length;
      let i      = this.pos;
      for(; i < length; ++i) {
         let c = text[i];
         if (c == desired) {
            this.pos = i;
            return desired;
         }
         if (!is_whitespace_char(c))
            break;
      }
      return null;
   }
   extractByFunctor(functor) {
      let text   = this.text;
      let length = text.length;
      let word   = "";
      let i      = this.pos;
      for(; i < length; ++i) {
         let c      = text[i];
         let result = functor(c);
         if (result === true) {
            return i;
         }
      }
   }
   extractCallArgs() {
      let text   = this.text;
      let length = text.length;
      //
      let start = this.pos;
      let args  = [];
      while (true) {
         if (this.extractChar(")"))
            break;
         if (this.pos >= this.text.length)
            this.throw_error(`Unclosed call arguments beginning at stream offset ${start}.`);
         //
         let arg = this.extractWord();
         if (arg == null) {
            arg = this.extractStringLiteral();
            if (arg == null) {
               arg = this.extractOperator();
               if (arg)
                  this.throw_error("Operators are not valid here.");
            }
         }
         //
         if (arg == null) { // no argument recognized
            if (!this.extractChar(")"))
               this.throw_error("Syntax error in function call arguments.");
            break;
         }
         if (this.extractChar(",")) {
            args.push(arg);
            ++this.pos; // skip past comma
            continue;
         }
         if (!this.extractChar(")"))
            this.throw_error("Syntax error in function call arguments.");
         args.push(arg);
         break;
      }
      return args;
   }
   extractOperator() {
      let op = "";
      let i  = this.extractByFunctor(function(c) {
         if (is_whitespace_char(c)) {
            if (!op)
               return; // continue
            return true; // stop
         }
         if (!is_operator_char(c))
            return true; // stop
         op += c;
      });
      if (op.length) {
         this.pos = i;
         return op;
      }
      return null;
   }
   extractStringLiteral() {
      let result = "";
      let inside = null;
      let i    = this.extractByFunctor((function(c) {
         if (!inside) {
            if (is_whitespace_char(c))
               return; // continue
            if (!is_quote_char(c))
               return true; // stop
            inside = c;
            return; // continue
         }
         if (c == inside)
            return true; // stop
         if (inside != "`" && c == "\n") {
            this.throw_error(`Unterminated string constant (use backticks or escaping for multi-line strings).`);
         }
         result += c;
      }).bind(this));
      if (inside) {
         this.pos = i + 1;
         return result;
      }
      return null;
   }
   extractWord() {
      let word = "";
      let i    = this.extractByFunctor(function(c) {
         if (is_whitespace_char(c)) {
            if (!word.length)
               return; // continue
            return true; // stop
         }
         if (!word.length) {
            if (c == "-") { // handle numeric sign
               word += c;
               return; // continue
            }
         }
         if (is_operator_char(c) || is_quote_char(c) || is_syntax_char(c))
            return true; // stop
         word += c;
      });
      if (word.length) {
         this.pos = i;
         return word;
      }
      return null;
   }
   findNextNonWhitespaceChar() {
      let text   = this.text;
      let length = text.length;
      let i      = this.pos;
      for(; i < length; ++i) {
         let c = text[i];
         if (is_whitespace_char(c))
            continue;
         return i;
      }
      return null;
   }
   parse(text, is_condition) {
      this.text = text;
      if (!this.root)
         this.root = new MBlock;
      let block     = this.root;
      let comment   = false;
      let statement = null;  // current statement
      let call      = null;  // current call (statement or assignment righthand side)
      let token     = "";    // current token being parsed (lefthand word / operator / call argument / righthand word)
      let token_end = false; // whether we hit whitespace, meaning that the current word is "over." if so, then another word character indicates {word word} which is bad syntax (outside of keywords)
      let length    = text.length;
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
         //
         // If we're not in a statement, then the next token must be a word. If that word is a 
         // keyword, then we handle it accordingly. If it is not a keyword, then it must be 
         // followed either by an operator (in which case we're in a compare or assign statement) 
         // or by an opening parentheses (in which case the statement is a function call).
         //
         if (!statement) {
            if (!token.length) {
               if (c != "-" && is_operator_char(c)) // minus-as-numeric-sign must be special-cased
                  this.throw_error(`Unexpected ${c}. Statements cannot begin with an operator.`);
               if (is_syntax_char(c))
                  this.throw_error(`Unexpected ${c}.`);
               if (is_quote_char(c))
                  this.throw_error(`Unexpected ${c}. Statements cannot begin with a string literal.`);
               if (is_whitespace_char(c))
                  continue;
               token += c;
               continue;
            }
            if (is_whitespace_char(c)) {
               token_end = true;
               //
               // ==================================================================================
               //
               // TODO: Handle keywords here.
               //
               // ==================================================================================
               //
               continue;
            }
            if (is_quote_char(c))
               this.throw_error(`Unexpected ${c}. Statements of the form {word "string" ...} are not valid.`);
            if (c == "(") {
               statement = call = new MFunctionCall;
               if (!call.extract_stem(token))
                  this.throw_error(`Invalid function context and/or name: "${token}".`);
               token     = "";
               token_end = false;
               continue;
            }
            if (c == ")" || c == ",")
               this.throw_error(`Unexpected ${c}.`);
            if (is_operator_char(c)) {
               if (is_condition) {
                  statement = new MComparison;
                  statement.left = token;
               } else {
                  statement = new MAssignment;
                  statement.target = token;
               }
               token     = c;
               token_end = false;
            }
            if (token_end)
               this.throw_error(`Statements of the form {word word} are not valid.`);
            token += c;
            if (token[0] == "-" && isNaN(+c)) {
               //
               // We allowed the word to start with "-" in case it was a number, but it 
               // has turned out not to be a number. That means that the "-" was an 
               // operator, not a numeric sign. Wait, that's illegal.
               //
               this.throw_error(`Unexpected -. Statements cannot begin with an operator.`);
            }
            continue;
         }
         //
         // If, on the other hand, we're in a statement, then we can finish things up based on 
         // what kind of statement we're in.
         //
         if (call) {
            //
            // Function calls can exist as standalone statements or as the righthand side of 
            // assignment statements. In both cases, the end of the function call is the end 
            // of the statement. (You can assign the result of a function call to a variable 
            // (or rather, that's the syntax we're going with for specific trigger opcodes), 
            // but we don't support arbitrary expressions so things like (a = b() + c) won't 
            // be valid.)
            //
            if (c == ",") {
               if (!token)
                  this.throw_error(`Unexpected ${c}.`); // func(a, , b) is not valid
               call.args.push(token);
               token     = "";
               token_end = false;
               continue;
            }
            if (c == ")") {
               //
               // TODO: detect and handle syntax error: func(a, )
               //
               if (token)
                  //
                  // The (token) variable is, here, the raw argument, e.g. "3" or "\"abc\"". 
                  // At some point we should give MFunctionCall an "append argument" method 
                  // which takes the token and normalizes it into some object (e.g. a class 
                  // for constant integers, a class for string literals, a class for variables, 
                  // a class for built-in terms like MP object types, and so on).
                  //
                  call.args.push(token);
               token     = "";
               token_end = false;
               //
               // End the statement.
               //
               block.insert_item(statement);
               call      = null;
               statement = null;
               continue;
            }
            if (is_whitespace_char(c)) {
               if (token.length)
                  token_end = true;
               continue;
            }
            if (token_end)
               this.throw_error(`Statements of the form {word word} are not valid.`);
            token += c;
            continue;
         }
         if (!call && statement instanceof MAssignment) {
            if (!statement.operator) {
               //
               // If the statement doesn't have an operator stored, then the operator is currently 
               // being parsed and exists in (token).
               //
               if (is_operator_char(c)) {
                  token += c;
                  continue;
               }
               statement.operator = new MOperator(token);
               token = "";
               //
               // Fall through to righthand-side handling so we don't miss the first character 
               // after the operator in cases like {a=b} where there's no whitespace.
               //
            }
            if ((token.length || c != "-") && is_operator_char(c))
               this.throw_error(`Unexpected ${c} on the righthand side of an assignment statement.`);
            if (is_quote_char(c))
               this.throw_error(`Unexpected ${c}. You cannot assign strings to variables.`);
            if (!token.length) {
               if (is_whitespace_char(c))
                  continue;
               if (c == "(")
                  this.throw_error(`Unexpected ${c}. Parentheses are only allowed as delimiters for function arguments.`);
            } else {
               if (c == "(") {
                  //
                  // Handle function call as righthand side.
                  //
                  call = new MFunctionCall;
                  if (!call.extract_stem(token))
                     this.throw_error(`Invalid function context and/or name: "${token}".`);
                  statement.source = call;
                  token     = "";
                  token_end = false;
                  continue;
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
               token += c;
               continue;
            }
            //
            // If we get here, then we've encountered the end of the statement's righthand side.
            //
            // TODO: We'll end up "losing" this statement if we hit EOF
            //
            statement.source = token;
            block.insert_item(statement);
            statement = null;
            token     = "";
            token_end = false;
            continue;
         }
         if (statement instanceof MComparison) {
            if (!statement.operator) {
               //
               // If the statement doesn't have an operator stored, then the operator is currently 
               // being parsed and exists in (token).
               //
               if (is_operator_char(c)) {
                  token += c;
                  continue;
               }
               statement.operator = new MOperator(token);
               token = "";
               //
               // Fall through to righthand-side handling so we don't miss the first character 
               // after the operator in cases like {a=b} where there's no whitespace.
               //
            }
            //
            // Handle righthand side.
            //
            if ((token.length || c != "-") && is_operator_char(c))
               this.throw_error(`Unexpected ${c} on the righthand side of a comparison statement.`);
            if (is_quote_char(c))
               this.throw_error(`Unexpected ${c}. You cannot compare variables to strings.`);
            if (!token.length && is_whitespace_char(c))
               continue;
            if (c == "(") {
               if (token)
                  this.throw_error(`Unexpected ${c}. You cannot compare variables to the result of a function call.`);
               this.throw_error(`Unexpected ${c}. Parentheses are only allowed as delimiters for function arguments.`);
            }
            if (c == ")" || c == ",")
               this.throw_error(`Unexpected ${c}.`);
            if (!is_whitespace_char(c)) {
               token += c;
               continue;
            }
            //
            // If we get here, then we've encountered the end of the statement's righthand side.
            //
            // TODO: We'll end up "losing" this statement if we hit EOF
            //
            statement.right = token;
            block.insert_item(statement);
            statement = null;
            token     = "";
            token_end = false;
            continue;
         }
      }
      return this.root;
   }
}