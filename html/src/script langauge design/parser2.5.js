class StatementSep {
   constructor(c) {
      this.character = c;
   }
}
class StringLiteral {
   constructor() {
      this.text = "";
   }
   append(t) {
      this.text += t;
   }
}
class MegaloBlock {
   constructor() {
      this.type  = "";
      this.arg   = "";
      this.name  = "";
      this.items = [];
   }
   addStatementSeparator(c) {
      this.items.push(new StatementSep(c));
   }
   addStringLiteral(t) {
      this.items.push(new StringLiteral(t));
   }
   addCode(c) {
      let i = this.items.length - 1;
      if (i >= 0) {
         let last = this.items[i];
         if (last + "" === last) {
            this.items[i] += c;
            return;
         }
      }
      this.items.push(c);
   }
}
class MegaloParen {
   constructor() {
      this.depth  = 0;
      this.text   = "(";
      this.parent = null;
   }
   take(c) {
      if (c == "(")
         ++this.depth;
      else if (c == ")") {
         if (--this.depth < 0)
            throw new Error("Bad nesting.");
      }
      this.text += c;
   }
}

function parse_megalo_2(text) {
   let root    = new MegaloBlock();
   let current = root;
   let size    = text.length;
   let quote   = null;
   let string  = null;
   let comment = null;
   let parenCount   = 0;
   let parenContent = "";
   for(let i = 0; i < size; i++) {
      let c = text[i];
      if (comment == "//") {
         if (c == "\n")
            comment = null;
         continue;
      }
      if (comment == "/*") {
         if (c == "*" && text[i + 1] == "/")
            comment = null;
         continue;
      }
      if (quote) {
         if (c == quote) {
            current.addStringLiteral(string);
            string = null;
            quote  = null;
         } else
            string += c;
         continue;
      }
      switch (c) {
         case "\n":
         case ":":
            current.addStatementSeparator(c);
            break;
         case "/":
            if (text[i + 1] == "/") {
               comment = "//";
               continue;
            }
            if (text[i + 1] == "*") {
               comment = "/*";
               continue;
            }
            break;
      }
      if (c == "(") {
         // TODO
      } else if (c == ")") {
         // TODO
      }
      if (c == "{") {
         let list  = current.items;
         let count = list.length;
         let block = new MegaloBlock();
         let j;
         let done  = false;
         for(j = count - 1; j >= 0; j++) {
            let prev = list[j];
            if (prev instanceof MegaloBlock)
               break;
            if (prev instanceof StatementSep) {
               switch (prev.character) {
                  case ";":
                     done = true;
                     break;
                  case "\n":
                     if (block.type)
                        done = true;
                     break;
                  case ":":
                     if (block.arg)
                        throw new Error("Unexpected \":\".");
               }
            }
            if (done)
               break;
         }
         list.splice(j + 1, i - j);
         list.push(block);
      }
      current.addCode(c);
   }
   if (root != current) {
      console.error("Unclosed block(s):", root, current);
      throw new Error(`Unclosed block(s).`);
   }
   return root;
}