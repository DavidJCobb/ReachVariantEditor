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
      if (!this.root) {
         this.root = new MBlock;
      }
      let block     = this.root;
      let comment   = false;
      let statement = null; // current statement
      let current   = null; // current word
      let type      = token_type.none;
      let length    = text.length;
      for(; this.pos < length; ++this.pos) {
         let c  = text[this.pos];
         let b  = this.pos > 0 ? text[this.pos - 1] : "\0";
         let di = this.findNextNonWhitespaceChar();
         let d  = text[di];
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
         if (!statement) {
            if (!current) {
               let type  = token_type.none;
               let token = this.extractWord();
               if (token) {
                  type = token_type.word;
               } else {
                  if (d == "(")
                     this.throw_error("Parentheses are only allowed for delimiting function call arguments.");
                  if (d == ")")
                     this.throw_error(`Unexpected ).`);
                  if (is_quote_char(d))
                     this.throw_error("A string literal cannot appear at the start of a statement.");
                  if (this.extractOperator() != null)
                     this.throw_error("An operator cannot appear at the start of a statement.");
                  continue;
               }
               current = token;
               //
               // TODO: check for keywords
               //
               --this.pos; // extractWord advanced us to the end of the word, but we're in a loop, and that'll advance us one more
               continue;
            }
            if (c == "(") { // handle function calls as statements
               statement = new MFunctionCall;
               if (!statement.extract_stem(current))
                  this.throw_error(`Invalid function context and/or name: "${current}".`);
               ++this.pos;
               statement.args = this.extractCallArgs(); // should advance to the index of the ")" closing the args; then the loop that we're in advances past that
               block.insert_item(statement);
               current   = null;
               statement = null;
               continue;
            }
            let token = this.extractWord();
            if (token != null) {
               this.throw_error("Invalid expression.");
            }
            token = this.extractOperator();
            if (token != null) {
               let op = new MOperator(token);
               if (is_condition) {
                  statement = new MComparison;
                  if (!op.is_comparison())
                     this.throw_error(`Operator ${op.text} is not valid here.`);
                  statement.left     = current;
                  statement.operator = op;
               } else {
                  statement = new MAssignment;
                  if (!op.is_assignment())
                     this.throw_error(`Operator ${op.text} is not valid here.`);
                  statement.target   = current;
                  statement.operator = op;
               }
               continue;
            }
         } else {
            //
            // Handle comparison statements and assignment statements.
            //
            if (statement instanceof MComparison) {
               let token = this.extractWord();
               if (token == null)
                  this.throw_error(`Comparison statement does not have a valid right-hand side.`);
               statement.right = token;
               --this.pos; // extractWord put us at the end of the word; our containing loop will advance us by one more
               //
               block.insert_item(statement);
               current   = null;
               statement = null;
               continue;
            }
            if (statement instanceof MAssignment) {
               let token = this.extractWord();
               if (token == null)
                  this.throw_error(`Assignment statement does not have a valid right-hand side.`);
               di = this.findNextNonWhitespaceChar();
               d  = text[di];
               if (d == "(") { // call args
                  this.pos = di;
                  let call = new MFunctionCall;
                  statement.source = call;
                  if (!call.extract_stem(token))
                     this.throw_error(`Invalid function context and/or name: "${token}".`);
                  ++this.pos;
                  call.args = this.extractCallArgs(); // should advance to the index of the ")" closing the args; then the loop that we're in advances past that
               } else {
                  statement.source = token;
                  --this.pos; // extractWord put us at the end of the word; our containing loop will advance us by one more
               }
               block.insert_item(statement);
               current   = null;
               statement = null;
               continue;
            }
         }
      }
      return this.root;
   }
}