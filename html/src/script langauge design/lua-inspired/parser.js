const OPERATORS_CMP = [
   "==",
   "!=",
   ">",
   "<",
   ">=",
   "<=",
];
const OPERATORS_MOD = [
   "+",
   "-",
   "*",
   "/",
   "%",
   ">>",
   "<<",
   ">>>",
   "^", // XOR
   "~",
   "|",
   "&",
];
const OPERATOR_ASSIGN = "="; // can be standalone or appended to any modify-operator
const OPERATOR_START_CHARS = "=!><+-*/%^~|&"; // TODO: rename to "operator chars"
const WHITESPACE_CHARS = " \r\n\t";

function extract_operator(text, i) {
   let result = "";
   let length = text.length;
   for(; i < length; i++) {
      let c = text[i];
      if (OPERATOR_START_CHARS.indexOf(c) < 0)
         break;
      result += c;
   }
   return result;
}
function extract_word(text, i) {
   let result = "";
   let length = text.length;
   for(; i < length; i++) {
      let c = text[i];
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
function find_next_word(text, i) {
   let length = text.length;
   for(; i < length; i++) {
      let c = text[i];
      if (WHITESPACE_CHARS.indexOf(c) < 0)
         if (OPERATOR_START_CHARS.indexOf(c) < 0)
            if (c != "(" && c != ")")
               return i;
   }
   return null;
}

class MParsedItem {
   constructor() {
      this.parent = null;
   }
}

const MBLOCK_TYPE_ROOT                       = 0;
const MBLOCK_TYPE_IF                         = 1;
const MBLOCK_TYPE_FOR_EACH_OBJECT            = 2;
const MBLOCK_TYPE_FOR_EACH_OBJECT_WITH_LABEL = 3;
const MBLOCK_TYPE_FOR_EACH_PLAYER            = 4;
const MBLOCK_TYPE_FOR_EACH_PLAYER_RANDOMLY   = 5;
const MBLOCK_TYPE_FUNCTION                   = 6;
const MBLOCK_TYPE_ELSE                       = 7;
const MBLOCK_TYPE_ELSE_IF                    = 8;
const MBLOCK_TYPE_BARE                       = 9; // do...end
const MBLOCK_TYPE_FOR_EACH_TEAM              = 10;

class MAlias extends MParsedItem {
   constructor(name, target) {
      this.name   = name;
      this.target = target;
   }
}
class MBlock extends MParsedItem {
   constructor(type) {
      super();
      this.type        = type || MBLOCK_TYPE_ROOT;
      this.forge_label = null;
      this.condition   = null; // for if-statements, an MBlock with no nesting allowed
      this.items       = [];
      //
      this.allow_nesting = true; // use for if-statements' parentheticals
   }
   clean() {
      this.items = this.items.filter(function(e, i, a) {
         if ((e instanceof MBlock) || (e instanceof MExpression))
            return !e.is_empty();
         return true;
      });
   }
   insert_item(item) {
      if (!this.allow_nesting) {
         if (item instanceof MBlock)
            throw new Error("This block doesn't allow nesting.");
      }
      this.items.push(item);
      item.parent = this;
      return item;
   }
   is_empty() {
      return this.items.length == 0;
   }
}
class MCallStem extends MParsedItem {
   constructor(stem) {
      super();
      this.stem = stem;
   }
}
class MOperator extends MParsedItem {
   constructor(o) {
      super();
      this.operator = o;
   }
}
class MExpressionJoiner extends MParsedItem {
   constructor(t) {
      super();
      this.text = t;
   }
}
class MText extends MParsedItem {
   constructor(text, start, end) {
      super();
      this.text  = text;
      this.range = [start, end];
   }
   is_numeric() {
      let text = this.text;
      let size = text.length;
      for(let i = 0; i < size; i++) {
         let c = text[i];
         if (c == "-")
            continue;
         if ((+c) + "" === c)
            continue;
         if (c == ".")
            continue;
         return false;
      }
      return true;
   }
}
class MExpression extends MParsedItem { // parentheticals
   constructor() {
      super();
      this.items = [];
   }
   //
   has_assign_operator() {
      for(let i = 0; i < this.items.length; i++) {
         let item = this.items[i];
         if (!(item instanceof MOperator))
            continue;
         item = item.operator || "";
         if (item[item.length - 1] != OPERATOR_ASSIGN)
            continue;
         if (OPERATORS_CMP.indexOf(item) >= 0) // likely "==", ">=", or "<="
            continue;
         return true;
      }
      return false;
   }
   is_empty() {
      return this.items.length == 0;
   }
   is_parenthetical() {
      return (this.parent instanceof MExpression);
   }
   item(index) {
      if (index < 0) {
         index = this.items.length + index;
         if (index < 0)
            return null;
      } else if (index >= this.items.length)
         return null;
      return this.items[index];
   }
   last() {
      if (!this.items.length)
         return null;
      return this.items[this.items.length - 1];
   }
   //
   can_insert_item(item) { // returns (false) if you should start a new expression instead
      if (!this.items.length) // an empty expression can always take a first token
         return true;
      let last = this.item(-1);
      if (item instanceof MExpression && last instanceof MExpression) // catch "(a) (b)"
         return false;
      if (item instanceof MText) {
         if (last instanceof MText) // catch "a b"
            return false;
         if (last instanceof MExpression) { // catch "(a) b"
            let prev = this.item(-2);
            if (prev instanceof MCallStem) { // allow "func(a).b"
               let glyph = last.text[last.text.length - 1];
               return (glyph == ".");
            }
            return false;
         }
      }
      if (last instanceof MText) {
         if (item instanceof MExpression) // allow "a()"
            return true;
         // MText followed by MText is already checked for above
      }
      return true;
   }
   insert_item(item) {
      if (this.items.length) {
         if (item instanceof MExpression) {
            //
            // Handle function calls. By this point we will already have taken 
            // the name of the function, and we are now getting the parentheses-
            // enclosed arguments. We can fix up the previously-received name by 
            // converting it from MText to MCallStem.
            //
            let index = this.items.length - 1;
            let last  = this.items[index];
            if (last instanceof MText)
               this.items[index] = new MCallStem(last.text);
         } else if (item instanceof MText) {
            //
            // Handle negative signs on incoming numbers. By this point we will 
            // have already received the minus and classed it as an operator; 
            // if it's preceded by another operator, and if the incoming text 
            // is numeric, then we know that that minus must actually have been 
            // a negative sign.
            //
            if (this.items.length > 1) {
               let index = this.items.length - 1;
               let last  = this.items[index];
               let prev  = this.items[index - 1];
               if (last instanceof MOperator && prev instanceof MOperator) {
                  if (last.operator == "-") {
                     this.items.splice(index, 1);
                     item.text = "-" + item.text;
                  }
               }
            }
         }
      }
      this.items.push(item);
      item.parent = this;
      return item;
   }
}

function parseMegalo(text) {
   let length       = text.length;
   let blockRoot    = new MBlock;
   let blockCurrent = blockRoot;
   let exprCurrent  = blockRoot.insert_item(new MExpression);
   let comment      = false;
   let i;
   for (i = 0; i < length; i++) {
      let c = text[i];
      {  // Handle parentheses, comments, and operators
         if (comment) {
            if (c == "\n")
               comment = false;
            continue;
         }
         let item = null;
         if (c == "(") {
            item = new MExpression;
            // Item will be inserted further below.
         } else if (c == ")") {
            exprCurrent = exprCurrent.parent;
            if (!(exprCurrent instanceof MExpression))
               throw new Error(`Unexpected/extra closing paren at offset ${i}!`);
            continue;
         } else if (c == "-") { // handle comments
            if (i + 1 < length) {
               if (text[i + 1] == "-") {
                  comment = true;
                  ++i;
                  continue;
               }
            }
         } // DON'T use (else if); c == "-" for comments overlaps next if for operators
         if (OPERATOR_START_CHARS.indexOf(c) >= 0) {
            c = extract_operator(text, i);
            i += c.length - 1;
            let is_assign = false;
            if (OPERATORS_CMP.indexOf(c) >= 0 || OPERATORS_MOD.indexOf(c) >= 0) {
               item = new MOperator(c);
            } else if (c.endsWith(OPERATOR_ASSIGN)) {
               let sub = c.substring(0, c.length - 1);
               if (c == OPERATOR_ASSIGN || OPERATORS_MOD.indexOf(sub) >= 0) {
                  if (exprCurrent.has_assign_operator()) {
                     throw new Error(`Invalid expression: second assignment operator ${c} at position ${i - c.length}.`);
                  }
                  item = new MOperator(c);
               }
            } else
               throw new Error(`Unrecognized operator ${c} at position ${i - c.length}.`);
         }
         if (item) {
            if (exprCurrent.can_insert_item(item)) {
               exprCurrent.insert_item(item);
            } else {
               if (exprCurrent.is_parenthetical()) {
                  throw new Error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
               }
               exprCurrent = blockCurrent.insert_item(new MExpression);
               exprCurrent.insert_item(item);
            }
            if (item instanceof MExpression)
               exprCurrent = item;
            continue;
         }
      }
      //
      // Keyword processing:
      //
      if (c == " ") {
         continue;
      }
      let word = extract_word(text, i);
      if (word.length) {
         let range = [i, i + word.length];
         switch (word) {
            case "and":      // expression separator
            case "or":       // expression separator
               if (exprCurrent.has_assign_operator())
                  //
                  // Unlike in Lua, "and" and "or" are expression separators, not operators, 
                  // and so they cannot appear within an assignment expression. The code 
                  // "abc = 3 and def" would be a valid statement in Lua but not in this 
                  // dialect. It just keeps things simpler. For binary OR and binary AND, 
                  // use "|" and "&".
                  //
                  throw new Error(`Cannot OR-link or AND-link expressions when any of them are variable assignment expressions. Error detected near offset ${i}.`);
               {
                  let joiner = new MExpressionJoiner(word);
                  let parent = exprCurrent.parent;
                  //
                  // TODO: Consider checking for bad constructions like "a or and b" and 
                  // "c and and d" here.
                  //
                  parent.insert_item(joiner);
                  exprCurrent = parent.insert_item(new MExpression);
               }
               i += word.length - 1; // skip to the end of the word
               continue;
            case "for":      // start of block-start
               if (exprCurrent.is_parenthetical())
                  throw new Error(`Cannot nest a block in a parenthetical expression. Error detected near offset ${i}.`);
               i += 3;
               {
                  function _getNextWord(mandatory) {
                     let j = find_next_word(text, i);
                     if (j == null) {
                        if (mandatory)
                           throw new Error(`The file ends early, cutting off a for-block.`);
                        return "";
                     }
                     i = j;
                     let word = extract_word(text, i);
                     if (word.length)
                        i += word.length;
                     return word;
                  }
                  let details = { // details to supply to the block we're about to create
                     type:  null,
                     label: null,
                  };
                  //
                  // TODO: When we port this to C++, can we represent the different for-loop phrases with 
                  // tree data of some kind? We'd want a tree and not simply an ordered list so that we can 
                  // have error messages like "Expected 'do' or 'with'" in the case of a "for each object" 
                  // phrase followed by a wrong word, where either of the two given words would have been 
                  // correct.
                  //
                  if (_getNextWord(true) != "each")
                     throw new Error(`Invalid for-loop near offset ${i}. Expected "each"; got "${word}".`);
                  switch (_getNextWord(true)) {
                     case "object": {
                        word = _getNextWord(true);
                        if (word == "do") {
                           details.type = MBLOCK_TYPE_FOR_EACH_OBJECT;
                        } else if (word == "with") {
                           if (_getNextWord(true) != "label")
                              throw new Error(`Invalid for-each-object-with-label loop near offset ${i}. Expected "label"; got "${word}".`);
                           //
                           // TODO: get the label name; then get the "do" keyword
                           //
                        } else
                           throw new Error(`Invalid for-each-object loop near offset ${i}. Expected "do" or "with"; got "${word}".`);
                     }; break;
                     case "player": {
                        word = _getNextWord(true);
                        if (word == "do") {
                           details.type = MBLOCK_TYPE_FOR_EACH_PLAYER;
                        } else if (word == "randomly") {
                           if (_getNextWord(true) != "do")
                              throw new Error(`Invalid for-each-player-randomly loop near offset ${i}. Expected "do"; got "${word}".`);
                           details.type = MBLOCK_TYPE_FOR_EACH_PLAYER_RANDOMLY;
                        }
                     }; break;
                     case "team": {
                        if (_getNextWord(true) != "do")
                           throw new Error(`Invalid for-each-team loop near offset ${i}. Expected "do"; got "${word}".`);
                        details.type = MBLOCK_TYPE_FOR_EACH_TEAM;
                     }; break;
                  }
                  if (!details.type) {
                     throw new Error(`Invalid for-loop near offset ${i}.`);
                  }
                  blockCurrent = blockCurrent.insert_item(new MBlock(details.type));
                  exprCurrent  = blockCurrent.insert_item(new MExpression);
                  if (details.label)
                     blockCurrent.forge_label = details.label;
                  //
                  // The _getNextWord function sets (i) to the end of the found word. However, 
                  // the parsing loop will increment (i) by one, so when we have all of the 
                  // words we want to parse, we actually need to move (i) back by one so that 
                  // at the start of the next parsing loop, it's at the end of the found words.
                  //
                  i--;
               }
               continue;
            case "function": // start of block-start
            case "if":       // start of block-start
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
               break; // replace with CONTINUE once code to handle this keyword is written
            case "do":       // end of block-start
               if (exprCurrent.is_parenthetical())
                  throw new Error(`Cannot nest a block in a parenthetical expression. Error detected near offset ${i}.`);
               blockCurrent = blockCurrent.insert_item(new MBlock(MBLOCK_TYPE_BARE));
               exprCurrent  = blockCurrent.insert_item(new MExpression);
               i += word.length - 1; // skip the word "do"
               continue;
            case "then":     // end of block-start
            case "else":     // end of block + start of new block
            case "elseif":   // end of block + start of new block-start
               console.warn(`TODO: Handle keyword "${word}".`);
               break; // replace with CONTINUE once code to handle this keyword is written
            case "end":      // end of block
               blockCurrent.clean();
               blockCurrent = blockCurrent.parent;
               if (!blockCurrent)
                  throw new Error(`Unexpected "end" near offset ${i}.`);
               exprCurrent  = blockCurrent.insert_item(new MExpression);
               i += word.length - 1; // skip the word "do"
               continue;
            case "alias":    // declaration
            case "expect":   // declaration
               console.warn(`TODO: Handle keyword "${word}".`);
               break; // replace with CONTINUE once code to handle this keyword is written
         }
         i += word.length - 1;
         let item = new MText(word, range[0], range[1]);
         if (exprCurrent.can_insert_item(item))
            exprCurrent.insert_item(item);
         else {
            if (exprCurrent.is_parenthetical()) {
               throw new Error(`Cannot have adjacent statements in a parenthetical expression; a joiner ("or"/"and") is needed.`);
            }
            exprCurrent = blockCurrent.insert_item(new MExpression);
            exprCurrent.insert_item(item);
         }
      }
   }
   if (blockRoot != blockCurrent) {
      throw new Error(`Unclosed block!`);
   }
   blockRoot.clean();
   return {
      list: blockRoot,
      end:  i,
   }
}