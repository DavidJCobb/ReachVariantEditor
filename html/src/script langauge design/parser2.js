class DelimiterType {
   constructor(options) {
      this.name  = options.name;
      this.open  = options.open  || options.char;
      this.close = options.close || this.open;
      this.isStringLiteral = options.isStringLiteral || false;
      this.dontAllowStatementSeparators = options.dontAllowStatementSeparators || false;
   }
}

let DELIMITERS = [
   new DelimiterType({
      name: "block",
      open: "{",
      close: "}"
   }),
   new DelimiterType({
      name: "paren",
      open: "(",
      close: ")",
      dontAllowStatementSeparators: true
   }),
   new DelimiterType({
      name: "quote_dbl",
      char: '"',
      isStringLiteral: true
   }),
   new DelimiterType({
      name: "quote_sng",
      char: "'",
      isStringLiteral: true
   }),
];
let DELIMITER_BLOCK = DELIMITERS[0];
let DELIMITER_PAREN = DELIMITERS[1];

class Block {
   constructor() {
      this.type  = "";
      this.arg   = "";
      this.name  = "";
      this.items = [];
   }
}
class Comment {
   constructor() {
      this.isSingleLine = false;
      this.text = "";
   }
}
class Delimiter {
   constructor(type) {
      this.type    = type;
      this.content = [];
      this.parent  = null;
   }
   addDelim(d) {
      this.content.push(d);
      d.parent = this;
      return d;
   }
   addSeparator(s) {
      this.content.push(s);
   }
   addText(t) {
      let l = this.content.length;
      if (l) {
         let i = this.content[l - 1]
         if (i + "" === i) {
            this.content[l - 1] += t;
            return;
         }
      }
      this.content.push(t);
   }
   //
   findDelim(type) {
      let list = this.content;
      let size = list.length;
      for(let i = 0; i < size; i++) {
         let item = list[i];
         if (item instanceof Delimiter) {
            if (item.type == type)
               return { item: item, parent: this };
            let i = item.findDelim(type);
            if (i)
               return i;
         }
      }
      return null;
   }
   indexOfDelim(d) {
      return this.content.indexOf(d);
   }
   //
   refine() {
      let type = this.type;
      if (type && type.isStringLiteral)
         return;
      let list = this.content;
      let size = list.length;
      if (!type || type == DELIMITER_BLOCK) {
         for(let i = 0; i < size; i++) {
            let item = list[i];
            if (!(item instanceof Delimiter))
               continue;
            if (item.type != DELIMITER_BLOCK)
               continue;
            let block = new Block;
            let colon;
            let arg;
            let name;
            let j;
            let k = i;
            for(j = i; j >= 0; j--) {
               let last = list[j];
               if (last instanceof Delimiter) {
                  if (block.arg || last.type != DELIMITER_PAREN) {
                     block = null;
                     break;
                  }
                  block.arg = last.content; // TODO: to string
                  k = j;
                  continue;
               }
               if (last instanceof PotentialStatementSeparator) {
                  if (last.c == ":") {
                     if (block.arg || block.name)
                        throw new Error(`Unexpected ":".`);
                     block.name = list.slice(j, i);
                     k = j;
                     continue;
                  }
                  if (last.c == ";") {
                     break;
                  }
               }
            }
            if (k > j) {
               block.name = list.slice(j, k);
            }
            if (!block)
               continue;
            block.items = item.content;
            list[i] = block;
         }
      }
      for(let i = 0; i < size; i++)
         if (list[i] instanceof Delimiter)
            list[i].refine();
   }
}
class PotentialStatementSeparator {
   constructor(character) {
      this.c = character;
   }
}

function parse_megalo_2(text) {
   let root    = new Delimiter(null);
   let current = root;
   let size    = text.length;
   let isLineComment  = false;
   let isBlockComment = false;
   {  // Pass 1
      for(let i = 0; i < size; i++) {
         let c = text[i];
         {  // Comments
            let ch2 = text.substring(i, i + 2);
            if (isLineComment) {
               if (c == "\n")
                  isLineComment = false;
               continue;
            }
            if (isBlockComment) {
               if (ch2 == "*/")
                  isBlockComment = false;
               continue;
            }
            if (ch2 == "//") {
               isLineComment = true;
               continue;
            }
            if (ch2 == "/*") {
               isBlockComment = true;
               continue;
            }
         }
         if (current.type && current.type.isStringLiteral) {
            if (c == current.type.close) {
               current = current.parent;
               continue;
            }
            current.addText(c);
            continue;
         }
         switch (c) {
            case ":":
            case "\n":
            case ";":
               current.addSeparator(new PotentialStatementSeparator(c));
               continue;
         }
         let handled = false;
         for(let d of DELIMITERS) {
            if (c == d.close) {
               if (d.isStringLiteral) {
                  if (current.type != d) {
                     if (d.close != d.open)
                        throw new Error("Unexpected \"" + c + "\".");
                  } else {
                     current = current.parent;
                     handled = true;
                     break;
                  }
               } else {
                  if (current.type != d)
                     throw new Error(`Unexpected "${c}".`);
                  current = current.parent;
                  handled = true;
                  break;
               }
            }
            if (c == d.open) {
               current = current.addDelim(new Delimiter(d));
               handled = true;
               break;
            }
         }
         if (handled)
            continue;
         current.addText(c);
      }
   }
   if (isBlockComment)
      throw new Error("Runaway block comment.");
   if (root != current) {
      console.error("Unclosed block(s):", root, current);
      throw new Error("Unclosed block(s).");
   }
   {  // Pass 2
      //
      // For each {} delimiter:
      //  - Search backward for a ":"
      //  - Search backward for a () delimiter, stopping at a line break or semicolon
      //  - Search backward for the block name, stopping at a line break or semicolon
      //
      // For each () delimiter:
      //  - Search backward for "if" or "else if"
      //
      root.refine();
   }
   return root;
}