const QUOTE_CHARS = "\`\'\"";

class MParser {
   constructor() {
      this.blockRoot    = null;
      this.blockCurrent = null;
      this.exprCurrent  = null;
      this.text         = "";
      this.pos          = 0; // position within the entire stream
      this.line         = 0; // zero-indexed
   }
   parse(text) {
      this.text = text;
      this.pos  = 0;
      this.blockRoot    = new MBlock;
      this.blockCurrent = this.blockRoot;
      this.exprCurrent  = this.blockRoot.insert_item(new MExpression);
      let length  = text.length;
      let comment = false;
      for(; this.pos < length; ++this.pos) {
         let i = this.pos;
         let c = text[i];
         if (c == "\n")
            ++this.line;
         {  // Handle parentheses, comments, and operators
            if (comment) {
               if (c == "\n")
                  comment = false;
               continue;
            }
            if (QUOTE_CHARS.indexOf(c) >= 0) {
               let content = this.findAndExtractStringLiteral(c);
               if (content + "" !== content) // for now this is how we detect anything unclosed if we already know there's an opening quote
                  throw new Error(`Unclosed string literal beginning at offset ${i}!`);
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
                  throw new Error(`Unexpected/extra closing paren at offset ${i}!`);
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
                        throw new Error(`Invalid expression: second assignment operator ${c} at position ${i - c.length}.`);
                     }
                     item = new MOperator(c);
                  }
               } else
                  throw new Error(`Unrecognized operator ${c} at position ${i - c.length}.`);
            }
            if (item) {
               if (this.exprCurrent.can_insert_item(item)) {
                  this.exprCurrent.insert_item(item);
               } else {
                  if (this.exprCurrent.is_parenthetical()) {
                     throw new Error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
                  }
                  this.exprCurrent = this.blockCurrent.insert_item(new MExpression);
                  this.exprCurrent.insert_item(item);
               }
               if (item instanceof MExpression)
                  this.exprCurrent = item;
               continue;
            }
         }
         //
         // Keyword processing:
         //
         if (c == " ") {
            continue;
         }
         let word = this.extractWord();
         if (word.length) {
            this.handleWord(word);
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
         list:    this.blockRoot,
         end:     this.pos,
         endLine: this.line,
      }
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
         case "then":     // end of block-start
         case "else":     // end of block + start of new block
         case "elseif":   // end of block + start of new block-start
            console.warn(`TODO: Handle keyword "${word}".`);
            //
            // So the way we're going to handle this is, we'll have an MBlock to represent 
            // the if-block... but the if-conditions are going to be another MBlock -- one 
            // that is configured not to allow any further MBlocks to be nested inside. 
            // This is because MExpression cannot be nested, and an if-condition can hold 
            // multiple MExpressions separated by "or" and "and." In essence, this would 
            // mean doing a simple read-ahead (counting parentheses) to find the start and 
            // end of the if-condition, and then passing the contents to this parser func-
            // tion as a recursive call. (Of course, we need to set this function up to 
            // allow for that -- it needs to be able to block nested MBlocks if the root 
            // doesn't allow them, etc..)
            //
            // TODO: If we decide to treat "or" and "and" as operators rather than special 
            // "joiners," then it becomes easier to handle if-statements: we reparse but 
            // only for expressions and not for whole blocks. Making them operators might 
            // be appropriate; we already don't allow them to join assignments as in the 
            // case of (a = b and c = d), whereas any operator can join separate assignment 
            // expressions as in the case of (a = (b = c) + (d = e)).
            //
            break;
         case "alias":    // declaration
         case "expect":   // declaration
            console.warn(`TODO: Handle keyword "${word}".`);
            break;
      }
      this.pos += word.length;
      let item = new MText(word, range[0], range[1]);
      if (this.exprCurrent.can_insert_item(item))
         this.exprCurrent.insert_item(item);
      else {
         if (this.exprCurrent.is_parenthetical()) {
            throw new Error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
         }
         this.exprCurrent = this.blockCurrent.insert_item(new MExpression);
         this.exprCurrent.insert_item(item);
      }
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
         throw new Error(`Cannot OR-link or AND-link expressions when any of them are variable assignment expressions. Error detected near offset ${this.pos}.`);
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
         throw new Error(`Cannot nest a block in a parenthetical expression. Error detected near offset ${this.pos}.`);
      this.pos += 3;
      let details = { // details to supply to the block we're about to create
         type:  null,
         label: null,
      };
      if (this.findAndExtractWord(true) != "each")
         throw new Error(`Invalid for-loop near offset ${this.pos}. Expected "each"; got "${word}".`);
      switch (this.findAndExtractWord(true)) {
         case "object": {
            let word = this.findAndExtractWord(true);
            if (word == "do") {
               details.type = MBLOCK_TYPE_FOR_EACH_OBJECT;
            } else if (word == "with") {
               if (this.findAndExtractWord(true) != "label")
                  throw new Error(`Invalid for-each-object-with-label loop near offset ${this.pos}. Expected "label"; got "${word}".`);
               let label_name = this.findAndExtractStringLiteral();
               if (label_name + "" === label_name) {
                  details.label = label_name;
               } else {
                  let label_index = this.findAndExtractIntegerLiteral();
                  if (!label_index && label_index !== 0)
                     throw new Error(`For-each-object-with-label loop failed to specify a valid Forge label, near offset ${this.pos}.`);
                  details.label = label_index;
               }
               if (this.findAndExtractWord(true) != "do")
                  throw new Error(`Invalid for-each-object-with-label loop near offset ${this.pos}. Expected "do" after the label.`);
               details.type = MBLOCK_TYPE_FOR_EACH_OBJECT_WITH_LABEL;
            } else
               throw new Error(`Invalid for-each-object loop near offset ${this.pos}. Expected "do" or "with"; got "${word}".`);
         }; break;
         case "player": {
            let word = this.findAndExtractWord(true);
            if (word == "do") {
               details.type = MBLOCK_TYPE_FOR_EACH_PLAYER;
            } else if (word == "randomly") {
               if (this.findAndExtractWord(true) != "do")
                  throw new Error(`Invalid for-each-player-randomly loop near offset ${this.pos}. Expected "do"; got "${word}".`);
               details.type = MBLOCK_TYPE_FOR_EACH_PLAYER_RANDOMLY;
            }
         }; break;
         case "team": {
            if (this.findAndExtractWord(true) != "do")
               throw new Error(`Invalid for-each-team loop near offset ${this.pos}. Expected "do"; got "${word}".`);
            details.type = MBLOCK_TYPE_FOR_EACH_TEAM;
         }; break;
      }
      if (!details.type) {
         throw new Error(`Invalid for-loop near offset ${this.pos}.`);
      }
      this.blockCurrent = this.blockCurrent.insert_item(new MBlock(details.type));
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
      if (details.label || details.label === 0)
         this.blockCurrent.forge_label = details.label;
   }
   _handleGenericBlockOpen(word) {
      if (this.exprCurrent.is_parenthetical())
         throw new Error(`Cannot nest a block in a parenthetical expression. Error detected near offset ${this.pos}.`);
      this.blockCurrent = this.blockCurrent.insert_item(new MBlock(MBLOCK_TYPE_BARE));
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
      this.pos += word.length;
   }
   _handleGenericBlockClose(word) {
      this.blockCurrent.clean();
      this.blockCurrent = this.blockCurrent.parent;
      if (!this.blockCurrent)
         throw new Error(`Unexpected "${word}" near offset ${this.pos}.`);
      this.exprCurrent  = this.blockCurrent.insert_item(new MExpression);
      this.pos += word.length;
   }
}