const QUOTE_CHARS = "\`\'\"";

const HANDLE_WORD_RESULT_STOP_EARLY = 1;

/*

   TODO:
   
   Consider changing "or" and "and" from special "expression joiner" objects to plain 
   operators. It's already the case that they can't separate every expression (i.e. 
   {a = b and c = d} is not valid), and it's already the case that an expression can 
   end up being multiple opcodes (i.e. {a = b + c}, though I'm not sure whether we 
   want to allow that; we can't reliably select a variable to use as a temporary in 
   cases like {a += b * c}.).
   
   Implement parsing for alias declarations and expect declarations.
   
    - An alias declaration is of the form {alias name = target} where target can be 
      a single variable (using other aliases is allowed), a constant integer, or a 
      constant expression involving only integers.
   
    - An expect declaration is of the form {expect variable = value} where the value 
      is a constant integer or constant expression involving only integers.
   
   Make it so that all parsed tokens store their start and end positions in the stream, 
   and not just MTexts.

*/

class MParser {
   constructor() {
      this.blockRoot    = null;
      this.blockCurrent = null;
      this.exprCurrent  = null; // last parsed expression
      this.text         = "";
      this.pos          = 0; // position within the entire stream
      this.line         = 0; // zero-indexed
      this.last_newline = 0; // zero-indexed
   }
   throw_error(text) {
      throw new Error(`Error on or near line ${this.line + 1} col ${this.pos - this.last_newline + 1}: ` + text);
   }
   parse(text, root, from) {
      this.text = text;
      this.pos  = from || 0;
      this.blockRoot    = root || new MBlock;
      this.blockCurrent = this.blockRoot;
      let length  = text.length;
      let comment = false;
      for(; this.pos < length; ++this.pos) {
         let i = this.pos;
         let expr = this.tryExtractExpression();
         if (expr) {
            this.blockCurrent.insert_item(expr);
            this.exprCurrent = expr;
            --this.pos; // the position was moved to just after the expression, but we're in a loop, so it'll be incremented again
            continue;
         }
         //
         // Keyword processing:
         //
         let word = this.extractWord();
         if (word.length) {
            let handler = this.getHandlerForKeyword(word);
            if (!handler)
               this.error(`No handler exists for keyword "${word}".`);
            let result = handler.call(this, word);
            if (result == HANDLE_WORD_RESULT_STOP_EARLY) {
               break;
            }
            --this.pos; // the position was moved to just after the word, but we're in a loop, so it'll be incremented again
         }
      }
      if (this.blockRoot != this.blockCurrent) {
         console.warn(this.blockRoot);
         console.warn(this.blockCurrent);
         throw new Error(`Unclosed block!`);
      }
      this.blockRoot.clean();
      return {
         root:    this.blockRoot,
         end:     this.pos,
         endLine: this.line,
      }
   }
   tryExtractExpression() {
      //
      // Loop through the text and extract the contents of a single expression. 
      // Stop when we encounter a keyword or any term that cannot be appended 
      // to the current expression (e.g. for "a + b c" we would stop just before 
      // the "c", since that has to be a new expression). Returns the extracted 
      // expression if it's not empty, or nothing otherwise.
      //
      // This function also handles keeping track of the line number and column 
      // number, and line comment behavior.
      //
      let text    = this.text;
      let length  = text.length;
      let comment = false;
      let current = new MExpression;
      for(; this.pos < length; ++this.pos) {
         let i = this.pos;
         let c = text[i];
         if (c == "\n") {
            ++this.line;
            this.last_newline = i;
         }
         if (comment) {
            if (c == "\n")
               comment = false;
            continue;
         }
         if (QUOTE_CHARS.indexOf(c) >= 0) {
            let content = this.findAndExtractStringLiteral(c);
            if (content + "" !== content) // for now this is how we detect anything unclosed if we already know there's an opening quote
               this.throw_error(`Unclosed string literal beginning at offset ${i}.`);
            current.insert_item(new MStringLiteral(content));
            this.pos -= 1; // we moved the position to after the string literal, but we're in a loop, so it'll be advanced by 1 more
            break;
         }
         let item = null;
         if (c == "(") {
            item = new MExpression;
            // Item will be inserted further below.
         } else if (c == ")") {
            current = current.parent;
            if (!(current instanceof MExpression))
               this.throw_error(`Unexpected/extra closing paren.`);
            continue;
         } else if (c == "-") { // handle comments
            if (i + 1 < length) {
               if (text[i + 1] == "-") {
                  comment = true;
                  ++this.pos;
                  continue;
               }
            }
         } // DON'T use (else if); c == "-" for comments overlaps next if for operators
         if (OPERATOR_START_CHARS.indexOf(c) >= 0) {
            c = extract_operator(text, i);
            this.pos += c.length - 1;
            let is_assign = false;
            if (OPERATORS_CMP.indexOf(c) >= 0 || OPERATORS_MOD.indexOf(c) >= 0) {
               item = new MOperator(c);
            } else if (c.endsWith(OPERATOR_ASSIGN)) {
               let sub = c.substring(0, c.length - 1);
               if (c == OPERATOR_ASSIGN || OPERATORS_MOD.indexOf(sub) >= 0) {
                  if (current.has_assign_operator()) {
                     this.throw_error(`Invalid expression: second assignment operator ${c}.`);
                  }
                  item = new MOperator(c);
               }
            } else
               this.throw_error(`Unrecognized operator ${c}.`);
         }
         let result = this.tryAppendItem(item, current);
         if (result) {
            current = result;
            continue;
         }
         //
         // Handle words: if they're not keywords, append them to the expression and skip to 
         // their end; if they're keywords, then exit.
         //
         if (WHITESPACE_CHARS.indexOf(c) >= 0)
            continue;
         let word = this.extractWord();
         if (this.getHandlerForKeyword(word)) {
            break;
         }
         item = new MText(word, this.pos, this.pos + word.length)
         if (!this.tryAppendItem(item, current))
            break;
         this.pos += word.length;
      }
      if (current.is_empty())
         return;
      return current;
   }
   tryAppendItem(item, expr) {
      if (!item)
         return;
      if (!expr)
         expr = this.exprCurrent;
      if (expr.can_insert_item(item)) {
         expr.insert_item(item);
      } else {
         if (expr.is_parenthetical()) {
            this.throw_error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
         }
         return;
      }
      if (item instanceof MExpression)
         expr = item;
      return expr;
   }
   getHandlerForKeyword(word) {
      switch (word) {
         case "and":
         case "or":
            return this._handleExpressionJoiner;
         case "do":
            return this._handleGenericBlockOpen;
         case "end":
            return this._handleGenericBlockClose;
         case "for":
            return this._handleForLoop;
         case "function":
            return this._handleFunction;
         case "if":
            return this._handleIf;
         case "then":
            return this._handleThen;
         case "else":
            return this._handleElse;
         case "elseif":
            return this._handleElseIf;
         case "alias":
            return this._handleAlias;
         case "expect":
            return this._handleExpect;
      }
      return null;
   }
   extractWord(i) { // does not advance this.pos
      if (isNaN(+i) || i === null)
         i = this.pos;
      let result = "";
      let length = this.text.length;
      for(; i < length; i++) {
         let c = this.text[i];
         if (OPERATOR_START_CHARS.indexOf(c) >= 0)
            break;
         if (WHITESPACE_CHARS.indexOf(c) >= 0)
            break;
         if (c == "(" || c == ")")
            break;
         result += c;
      }
      return result;
   }
   findAndExtractIntegerLiteral(i) { // advances this.pos if a number is found
      if (isNaN(+i) || i === null)
         i = this.pos;
      let result = "";
      let length = this.text.length;
      for(; i < length; i++) {
         let c = this.text[i];
         if (!result) {
            if (WHITESPACE_CHARS.indexOf(c) >= 0)
               continue;
         }
         if ((+c) + "" === c)
            result += c;
         else
            break;
      }
      if (result) {
         this.pos = i;
         return +result;
      }
      return;
   }
   findAndExtractStringLiteral(quote, i) { // advances this.pos if a string is found
      if (isNaN(+i) || i === null)
         i = this.pos;
      let result = "";
      let length = this.text.length;
      let inside = false;
      for(; i < length; i++) {
         let c = this.text[i];
         if (!quote) {
            if (WHITESPACE_CHARS.indexOf(c) >= 0)
               continue;
            if (QUOTE_CHARS.indexOf(c) < 0)
               return;
            quote  = c;
            inside = true;
            continue;
         }
         if (c == quote) {
            if (inside) {
               ++i;
               break;
            }
            inside = true;
            continue;
         }
         result += c;
      }
      if (!inside) // we never found an opening quote
         return;
      if (i == length) // we never found a closing quote; TODO: signal a parse error
         return;
      this.pos = i;
      return result;
   }
   findAndExtractWord() { // advances this.pos
      let i      = this.pos;
      let length = this.text.length;
      for(; i < length; i++) {
         let c = this.text[i];
         if (WHITESPACE_CHARS.indexOf(c) < 0)
            if (OPERATOR_START_CHARS.indexOf(c) < 0)
               if (QUOTE_CHARS.indexOf(c) < 0)
                  if (c != "(" && c != ")")
                     break;
      }
      let word = this.extractWord(i);
      this.pos = i + word.length;
      return word;
   }
   _handleExpressionJoiner(word) {
      if (!this.exprCurrent)
         this.throw_error(`Unexpected ${word} (there is no initial expression to join; this keyword cannot appear at the start of a block).`);
      if (this.exprCurrent.has_assign_operator())
         //
         // Unlike in Lua, "and" and "or" are expression separators, not operators, 
         // and so they cannot appear within an assignment expression. The code 
         // "abc = 3 and def" would be a valid statement in Lua but not in this 
         // dialect. It just keeps things simpler. For binary OR and binary AND, 
         // use "|" and "&".
         //
         this.throw_error(`Cannot OR-link or AND-link expressions when any of them are variable assignment expressions.`);
      let joiner = new MExpressionJoiner(word);
      let parent = this.exprCurrent.parent;
      //
      // TODO: Consider checking for bad constructions like "a or and b" and 
      // "c and and d" here.
      //
      parent.insert_item(joiner);
      this.exprCurrent = parent.insert_item(new MExpression);
      //
      this.pos += word.length;
   }
   _handleForLoop() {
      if (this.exprCurrent && this.exprCurrent.is_parenthetical())
         this.throw_error(`Cannot nest a block in a parenthetical expression.`);
      if (!this.blockRoot.allow_nesting)
         this.throw_error(`You cannot open a new block here.`);
      this.pos += 3;
      let details = { // details to supply to the block we're about to create
         type:  null,
         label: null,
      };
      if (this.findAndExtractWord(true) != "each")
         this.throw_error(`Invalid for-loop. Expected "each"; got "${word}".`);
      switch (this.findAndExtractWord(true)) {
         case "object": {
            let word = this.findAndExtractWord(true);
            if (word == "do") {
               details.type = MBLOCK_TYPE_FOR_EACH_OBJECT;
            } else if (word == "with") {
               if (this.findAndExtractWord(true) != "label")
                  this.throw_error(`Invalid for-each-object-with-label loop. Expected "label"; got "${word}".`);
               let label_name = this.findAndExtractStringLiteral();
               if (label_name + "" === label_name) {
                  details.label = label_name;
               } else {
                  let label_index = this.findAndExtractIntegerLiteral();
                  if (!label_index && label_index !== 0)
                     this.throw_error(`For-each-object-with-label loop failed to specify a valid Forge label.`);
                  details.label = label_index;
               }
               if (this.findAndExtractWord(true) != "do")
                  this.throw_error(`Invalid for-each-object-with-label loop. Expected "do" after the label.`);
               details.type = MBLOCK_TYPE_FOR_EACH_OBJECT_WITH_LABEL;
            } else
               this.throw_error(`Invalid for-each-object loop. Expected "do" or "with"; got "${word}".`);
         }; break;
         case "player": {
            let word = this.findAndExtractWord(true);
            if (word == "do") {
               details.type = MBLOCK_TYPE_FOR_EACH_PLAYER;
            } else if (word == "randomly") {
               if (this.findAndExtractWord(true) != "do")
                  this.throw_error(`Invalid for-each-player-randomly loop. Expected "do"; got "${word}".`);
               details.type = MBLOCK_TYPE_FOR_EACH_PLAYER_RANDOMLY;
            }
         }; break;
         case "team": {
            if (this.findAndExtractWord(true) != "do")
               this.throw_error(`Invalid for-each-team loop. Expected "do"; got "${word}".`);
            details.type = MBLOCK_TYPE_FOR_EACH_TEAM;
         }; break;
      }
      if (!details.type) {
         this.throw_error(`Invalid for-loop.`);
      }
      this.blockCurrent = this.blockCurrent.insert_item(new MBlock(details.type));
      if (details.label || details.label === 0)
         this.blockCurrent.forge_label = details.label;
   }
   _handleGenericBlockOpen(word, type) {
      if (this.exprCurrent && this.exprCurrent.is_parenthetical())
         this.throw_error(`Cannot nest a block in a parenthetical expression.`);
      if (!this.blockRoot.allow_nesting)
         this.throw_error(`You cannot open a new block here.`);
      this.blockCurrent = this.blockCurrent.insert_item(new MBlock(type || MBLOCK_TYPE_BARE));
      this.pos += word.length;
   }
   _handleGenericBlockClose(word) {
      this.blockCurrent.clean();
      this.blockCurrent = this.blockCurrent.parent;
      if (!this.blockCurrent)
         this.throw_error(`Unexpected "${word}".`);
      if (word)
         this.pos += word.length;
   }
   _handleIf(word) {
      let type;
      switch (word) {
         case "if":     type = MBLOCK_TYPE_IF; break;
         case "elseif": type = MBLOCK_TYPE_ELSE_IF; break;
      }
      this._handleGenericBlockOpen(word, type);
      //
      let bc = this.blockCurrent;
      let cond_parser = new MParser;
      let cond_root   = new MCondition;
      let cond_data = cond_parser.parse(this.text, cond_root, this.pos);
      this.pos = cond_parser.pos;
      let root = cond_data.root;
      if (root.items.length == 1 && root.items[0] instanceof MExpression) {
         bc.condition        = root.items[0];
         bc.condition.parent = null;
         bc.condition.owner  = bc;
      } else {
         bc.condition       = new MExpression;
         bc.condition.items = root.items;
      }
   }
   _handleElseIf(word) {
      this._handleGenericBlockClose();
      this._handleIf(word);
   }
   _handleElse(word) {
      this._handleGenericBlockClose();
      this._handleGenericBlockOpen(word, MBLOCK_TYPE_ELSE);
   }
   _handleThen(word) {
      if (!(this.blockRoot instanceof MCondition))
         this.throw_error(`The "then" keyword is not allowed here.`);
      this.pos += word.length;
      return HANDLE_WORD_RESULT_STOP_EARLY;
   }
   _handleAlias(word) {
      if (this.exprCurrent && this.exprCurrent.is_parenthetical())
         this.throw_error(`Cannot put an alias declaration in a parenthetical expression.`);
      this.pos += word.length;
      let expr = this.tryExtractExpression();
      if (!expr)
         this.throw_error(`Alias declaration not followed by a valid expression.`);
      let raw = new MAlias;
      raw.raw = expr;
      this.blockCurrent.insert_item(raw);
   }
   _handleExpect(word) {
      if (this.exprCurrent && this.exprCurrent.is_parenthetical())
         this.throw_error(`Cannot put an expect declaration in a parenthetical expression.`);
      this.pos += word.length;
      let expr = this.tryExtractExpression();
      if (!expr)
         this.throw_error(`Expect declaration not followed by a valid expression.`);
      let raw = new MExpect;
      raw.raw = expr;
      this.blockCurrent.insert_item(raw);
   }
   _handleFunction(word) {
      if (!this.blockRoot.allow_nesting)
         this.throw_error(`You cannot define a function here.`);
      this.pos += word.length;
      let name = this.findAndExtractWord(true);
      if (!name)
         this.throw_error(`Functions must have names.`);
      let expr = this.tryExtractExpression();
      if (!expr)
         this.throw_error(`Functions must have arguments.`);
      let item = new MFunction(name, expr);
      this.blockCurrent = this.blockCurrent.insert_item(item);
   }
}