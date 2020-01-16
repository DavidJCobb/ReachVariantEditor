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
const OPERATOR_START_CHARS = "=!><+-*/%^~|&";

class MParsedItem {
   constructor() {
      this.parent = null;
   }
}

class MBlock extends MParsedItem {
   constructor() {
      super();
      this.items = [];
   }
   clean() {
      this.items = this.items.filter(function(e, i, a) {
         if ((e instanceof MBlock) || (e instanceof MExpression))
            return !e.is_empty();
         return true;
      });
   }
   insert_item(item) {
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
      if (!this.items.length)
         return true;
      let last = this.index(-1);
      if (item instanceof MExpression && last instanceof MExpression) // catch "(a) (b)"
         return false;
      if (item instanceof MText) {
         if (last instanceof MText) // catch "a b"
            return false;
         if (last instanceof MExpression) { // catch "(a) b"
            let prev = this.index(-2);
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
   function _lookAheadForWord(text, i) {
      let result = "";
      let length = text.length;
      for(; i < length; i++) {
         let c = text[i];
         if (OPERATOR_START_CHARS.indexOf(c) >= 0)
            break;
         if (("() \r\n\t").indexOf(c) >= 0)
            break;
         result += c;
      }
      return result;
   }
   function _lookAheadForNextWord(text, i) {
      let length = text.length;
      for(; i < length; i++) {
         let c = text[i];
         if ((" \r\n\t").indexOf(c) < 0)
            return i + 1;
      }
      return null;
   }
   let length  = text.length;
   let blockRoot    = new MBlock;
   let blockCurrent = blockRoot;
   let exprCurrent  = blockRoot.insert_item(new MExpression);
   let comment = false;
   let i;
   for (i = 0; i < length; i++) {
      let c = text[i];
      if (comment) {
         if (c == "\n")
            comment = false;
         continue;
      }
      let d = "";
      if (i > 0)
         d = text[i - 1];
      if (c == "(") {
         exprCurrent = exprCurrent.insert_item(new MExpression);
         continue;
      } else if (c == ")") {
         exprCurrent = exprCurrent.parent;
         if (!(exprCurrent instanceof MExpression))
            throw new Error(`Unexpected/extra closing paren at offset ${i}!`);
         continue;
      }
      if (c == "-") { // handle comments
         if (i + 1 < length) {
            if (text[i + 1] == "-") {
               comment = true;
               ++i;
               continue;
            }
         }
      }
      if (OPERATOR_START_CHARS.indexOf(c) >= 0) {
         for(let j = i + 1; j < length; j++) {
            let e = text[j];
            if (OPERATOR_START_CHARS.indexOf(e) < 0)
               break;
            c += e;
            ++i;
         }
         let is_assign = false;
         if (OPERATORS_CMP.indexOf(c) >= 0) {
            exprCurrent.insert_item(new MOperator(c));
            continue;
         } else if (OPERATORS_MOD.indexOf(c) >= 0) {
            exprCurrent.insert_item(new MOperator(c));
            continue;
         }
         if (c.endsWith(OPERATOR_ASSIGN)) {
            let sub = c.substring(0, c.length - 1);
            if (c == OPERATOR_ASSIGN || OPERATORS_MOD.indexOf(sub) >= 0) {
               if (exprCurrent.has_assign_operator()) {
                  throw new Error(`Invalid expression: second assignment operator ${c} at position ${i - c.length}.`);
               }
               exprCurrent.insert_item(new MOperator(c));
               continue;
            }
         }
         throw new Error(`Unrecognized operator ${c} at position ${i - c.length}.`);
      }
      if (c == " ") {
         continue;
      }
      let word = _lookAheadForWord(text, i);
      if (word.length) {
         let range = [i, i + word.length];
         i += word.length - 1;
         switch (word) {
            case "and":      // expression separator
            case "or":       // expression separator
            case "for":      // start of block-start
            case "function": // start of block-start
            case "if":       // start of block-start
               console.warn(`TODO: Handle keyword "${word}".`);
               break;
            case "do":       // end of block-start
               if (exprCurrent.is_parenthetical())
                  throw new Error(`Cannot nest a block in a parenthetical expression. Error detected near offset ${i}.`);
               blockCurrent = blockCurrent.insert_item(new MBlock);
               exprCurrent  = blockCurrent.insert_item(new MExpression);
               continue;
            case "then":     // end of block-start
            case "else":     // end of block + start of new block
            case "elseif":   // end of block + start of new block-start
               console.warn(`TODO: Handle keyword "${word}".`);
               break;
            case "end":      // end of block
               blockCurrent.clean();
               blockCurrent = blockCurrent.parent;
               if (!blockCurrent)
                  throw new Error(`Unexpected "end" near offset ${i}.`);
               exprCurrent  = blockCurrent.insert_item(new MExpression);
               continue;
            case "alias":    // declaration
            case "expect":   // declaration
               console.warn(`TODO: Handle keyword "${word}".`);
               break;
         }
         exprCurrent.insert_item(new MText(word, range[0], range[1]));
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