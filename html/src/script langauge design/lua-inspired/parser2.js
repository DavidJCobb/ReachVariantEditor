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

*/

class MParser {
   constructor() {
      this.blockRoot    = null;
      this.blockCurrent = null;
      this.exprCurrent  = null;
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
      this.exprCurrent  = this.blockRoot.insert_item(new MExpression);
      let length  = text.length;
      let comment = false;
      for(; this.pos < length; ++this.pos) {
         let i = this.pos;
         let c = text[i];
         if (c == "\n") {
            ++this.line;
            this.last_newline = i;
         }
         {  // Handle parentheses, comments, and operators
            if (comment) {
               if (c == "\n")
                  comment = false;
               continue;
            }
            if (QUOTE_CHARS.indexOf(c) >= 0) {
               let content = this.findAndExtractStringLiteral(c);
               if (content + "" !== content) // for now this is how we detect anything unclosed if we already know there's an opening quote
                  this.throw_error(`Unclosed string literal beginning at offset ${i}.`);
               this.exprCurrent.insert_item(new MStringLiteral(content));
               this.pos -= 1; // we moved the position to after the string literal, but we're in a loop, so it'll be advanced by 1 more
               continue;
            }
            let item = null;
            if (c == "(") {
               item = new MExpression;
               // Item will be inserted further below.
            } else if (c == ")") {
               this.exprCurrent = this.exprCurrent.parent;
               if (!(this.exprCurrent instanceof MExpression))
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
                     if (this.exprCurrent.has_assign_operator()) {
                        this.throw_error(`Invalid expression: second assignment operator ${c}.`);
                     }
                     item = new MOperator(c);
                  }
               } else
                  this.throw_error(`Unrecognized operator ${c}.`);
            }
            if (this.tryAppendItem(item))
               continue;
         }
         //
         // Keyword processing:
         //
         if (c == " ") {
            continue;
         }
         let word = this.extractWord();
         if (word.length) {
            let result = this.handleWord(word);
            if (result == HANDLE_WORD_RESULT_STOP_EARLY) {
               break;
            }
            this.pos--; // the position was moved to just after the word, but we're in a loop, so it'll be incremented again
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
   tryAppendItem(item) {
      if (!item)
         return false;
      if (this.exprCurrent.can_insert_item(item)) {
         this.exprCurrent.insert_item(item);
      } else {
         if (this.exprCurrent.is_parenthetical()) {
            this.throw_error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
         }
         this.exprCurrent = this.blockCurrent.insert_item(new MExpression);
         this.exprCurrent.insert_item(item);
      }
      if (item instanceof MExpression)
         this.exprCurrent = item;
      return true;
   }
   handleWord(word) {
      let range = [this.pos, this.pos + word.length];
      switch (word) {
         case "and":
         case "or":
            this._handleExpressionJoiner(word);
            return;
         case "do":
            this._handleGenericBlockOpen(word);
            return;
         case "end":
            this._handleGenericBlockClose(word);
            return;
         case "for":
            this._handleForLoop();
            return;
         case "function": // start of block-start
            console.warn(`TODO: Handle keyword "${word}".`);
            break;
         case "if":       // start of block-start
            this._handleIf(word);
            return;
         case "then":     // end of block-start
            if (!(this.blockRoot instanceof MCondition))
               this.throw_error(`The "then" keyword is not allowed here.`);
            this.pos += word.length;
            return HANDLE_WORD_RESULT_STOP_EARLY;
         case "else":     // end of block + start of new block
            this._handleElse(word);
            return;
         case "elseif":   // end of block + start of new block-start
            this._handleGenericBlockClose();
            this._handleIf(word);
            return;
         case "alias":    // declaration
         case "expect":   // declaration
            console.warn(`TODO: Handle keyword "${word}".`);
            break;
      }
      //
      // Handling for non-keywords:
      //
      this.pos += word.length;
      let item = new MText(word, range[0], range[1]);
      this.tryAppendItem(item);
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
      if (this.exprCurrent.is_parenthetical())
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
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
      if (details.label || details.label === 0)
         this.blockCurrent.forge_label = details.label;
   }
   _handleGenericBlockOpen(word, type) {
      if (this.exprCurrent.is_parenthetical())
         this.throw_error(`Cannot nest a block in a parenthetical expression.`);
      if (!this.blockRoot.allow_nesting)
         this.throw_error(`You cannot open a new block here.`);
      this.blockCurrent = this.blockCurrent.insert_item(new MBlock(type || MBLOCK_TYPE_BARE));
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
      this.pos += word.length;
   }
   _handleGenericBlockClose(word) {
      this.blockCurrent.clean();
      this.blockCurrent = this.blockCurrent.parent;
      if (!this.blockCurrent)
         this.throw_error(`Unexpected "${word}".`);
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
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
   _handleElse(word) {
      this._handleGenericBlockClose();
      this._handleGenericBlockOpen(word, MBLOCK_TYPE_ELSE);
   }
}