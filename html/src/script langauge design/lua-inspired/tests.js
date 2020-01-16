const COMPARE_OPERATORS = [
   "==",
   "!=",
   ">",
   "<",
   ">=",
   "<=",
];
const MODIFY_OPERATORS = [
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
const ASSIGN_OPERATOR = "="; // can be standalone or appended to any modify-operator

const OPERATOR_CHARS = "=!><+-*/%^~|&";

const COMPARE_LINK_KEYWORDS = [ "and", "or" ];
const BLOCK_KEYWORDS = [ "do", "else", "elseif", "end", "for", "if", "then" ];
const OTHER_KEYWORDS = [ "alias", "expect" ];

class ParsedCallStem {
   constructor(stem) {
      this.stem = stem;
   }
}
class ParsedOperator {
   constructor(o) {
      this.operator = o;
   }
}
class ParsedExpression {
   constructor() {
      this.items  = [];
      this.parent = null;
      this.last_is_separated = false;
   }
   has_assign_operator() {
      for(let i = 0; i < this.items.length; i++) {
         let item = this.items[i];
         if (item + "" !== item)
            continue;
         if (item[item.length - 1] != ASSIGN_OPERATOR)
            continue;
         if (COMPARE_OPERATORS.indexOf(item) >= 0) // likely "==", ">=", or "<="
            continue;
         return true;
      }
      return false;
   }
   insert_expression(item) {
      this.last_is_separated = false;
      if (this.items.length) {
         let last = this.items.length - 1;
         if (last >= 0) {
            let prev = this.items[last];
            if (prev + "" === prev)
               this.items[last] = new ParsedCallStem(prev);
         }
      }
      this.items.push(item);
      item.parent = this;
      return item;
   }
   insert_operator(text) {
      this.last_is_separated = false;
      let item = new ParsedOperator(text);
      this.items.push(item);
      item.parent = this;
      return item;
   }
   insert_separator() {
      this.last_is_separated = true;
   }
   insert_text(text) {
      if (!this.last_is_separated) {
         let last = this.items.length - 1;
         if (last >= 0) {
            let item = this.items[last];
            if (item + "" === item) {
               this.items[last] += text;
               return;
            }
         }
      }
      this.last_is_separated = false;
      this.items.push(text);
   }
   //
   is_call_arguments(index) {
      if (index == 0)
         return false;
      if (this.items[index] instanceof ParsedExpression) {
         if (this.items[index - 1] instanceof ParsedCallStem)
            return true;
      }
      return false;
   }
   //
   separate() {
      let last = new ParsedExpression;
      let list = [last];
      let length = this.items.length;
      for(let i = 0; i < length; i++) {
         let item = this.items[i];
         if (i == 0) {
            last.items.push(item);
            continue;
         }
         let prev = this.items[i - 1];
         //
         let item_is_string = item + "" === item;
         let prev_is_string = prev + "" === prev;
         let prev_is_expr = prev instanceof ParsedExpression;
         let item_is_expr = item instanceof ParsedExpression;
         //
         let should_separate = (item_is_string && prev_is_string); // check for "a b"
         if (!should_separate) { // check for "(a)(b)", "a(b)", "(a)b"
            if (prev_is_expr) {
               should_separate = item_is_string;
            } else if (item_is_expr) {
               should_separate = prev_is_string;
            }
         }
         if (!should_separate) { // check for "a() b" but not "a().b"
            if (prev_is_expr && i > 1) {
               let call = this.items[i - 2];
               if (call instanceof ParsedCallStem) {
                  if (item_is_expr || item instanceof ParsedCallStem || (item_is_string && !item.startsWith(".")))
                     should_separate = true;
               }
            }
         }
         if (should_separate) {
            last = new ParsedExpression;
            list.push(last);
            last.items.push(item);
            continue;
         }
         last.items.push(item);
      }
      return list;
   }
}

function parseExpression(text) {
   let length  = text.length;
   let parens  = 0;
   let root    = new ParsedExpression;
   let current = root;
   let i;
   for (i = 0; i < length; i++) {
      let c = text[i];
      let d = "";
      if (i > 0)
         d = text[i - 1];
      if (c == "(") {
         current = current.insert_expression(new ParsedExpression);
         continue;
      } else if (c == ")") {
         current = current.parent;
         continue;
      }
      if (OPERATOR_CHARS.indexOf(c) >= 0) {
         for(let j = i + 1; j < length; j++) {
            let e = text[j];
            if (OPERATOR_CHARS.indexOf(e) < 0)
               break;
            c += e;
            ++i;
         }
         let is_assign = false;
         if (COMPARE_OPERATORS.indexOf(c) >= 0) {
            current.insert_operator(c);
            continue;
         } else if (MODIFY_OPERATORS.indexOf(c) >= 0) {
            current.insert_operator(c);
            continue;
         }
         if (c.endsWith(ASSIGN_OPERATOR)) {
            let sub = c.substring(0, c.length - 1);
            if (MODIFY_OPERATORS.indexOf(sub) >= 0) {
               if (current.has_assign_operator()) {
                  throw new Error(`Invalid expression: second assignment operator ${c} at position ${i - c.length}.`);
               }
               current.insert_operator(c);
               continue;
            }
         }
         throw new Error(`Unrecognized operator ${c} at position ${i - c.length}.`);
      }
      if (c == " ") {
         current.insert_separator();
         continue;
      }
      current.insert_text(c);
   }
   return {
      list: root.separate(),
      end:  i,
   }
}