class RawAlias {
   constructor(n, v) {
      this.name  = n.trim();
      this.value = v.trim();
   }
}
class RawExpect {
   constructor(n, v) {
      this.name  = n.trim();
      this.value = v.trim();
   }
}

class RawBlock {
   // if (condition) {}
   // for each player {}
   // for each player randomly {}
   // subroutine: name {}
   constructor(line_number) {
      this.line_number = line_number || null;
      this.declaration = ""; // type (args) : name
      this.type = "";
      this.name = "";
      this.args = "";
      this.expectations = [];
      this.children     = []; // lines, RawAliases, and RawBlocks
      this.parent = null;
      this.isSingleChild = false; // for single-line IFs
   }
   appendChild(b) {
      b.parent = this;
      this.children.push(b);
      return b;
   }
   appendLine(s) {
      if (!s)
         return;
      this.children.push(s);
   }
   refine() {
      {  // Split declaration
         let d = this.declaration;
         let p = d.indexOf("(");
         if (p >= 0) {
            this.type = d.substring(0, p);
            let pCount = 1;
            for(let i = p + 1; i < d.length; i++) {
               let c = d[i];
               switch (c) {
                  case "(": ++pCount; break;
                  case ")": --pCount; break;
                  default:
                     this.args += c;
               }
               if (!pCount) {
                  d = d.substring(i + 1);
                  break;
               }
            }
         }
         let c = d.indexOf(":");
         if (c >= 0) {
            this.name = d.substring(c + 1).trim();
            if (!this.type)
               this.type = d.substring(0, c);
         } else {
            if (!this.type)
               this.type = d;
         }
         this.type = this.type.trim();
      }
      for(let i = 0; i < this.children.length; i++) {
         let child = this.children[i];
         if (child instanceof RawBlock) {
            child.refine();
            continue;
         }
         if (child + "" !== child) // shouldn't happen
            continue;
         child = child.trim();
         if (child.startsWith("alias ")) {
            let pair = child.substring(6).split("=");
            this.children[i] = new RawAlias(pair[0], pair[1]); // TODO: validate
            continue;
         }
         if (child.startsWith("expect ")) {
            if (this.type != "subroutine") {
               console.warn("The (expect) directive is not valid here.");
            }
            let pair = child.substring(7).split("==");
            this.expectations.push(new RawExpect(pair[0], pair[1])); // TODO: validate
            this.children.splice(i, 1);
            i--;
            continue;
         }
      }
   }
}

function _parseLines(lines, callback) {
   for(let i = 0; i < lines.length; i++) {
      let line = lines[i].trim();
      let remaining = callback(i, line);
      if (remaining && remaining + "" === remaining) {
         lines[i] = remaining;
         i--;
      }
   }
}

function parse_megalo(text) {
   let is_in_if      = false;
   let current_block = new RawBlock(0);
   let root_block    = current_block;
   //
   let lines = text.split("\n");
   //
   let extended_declaration = "";
   let extended_paren_count = 0;
   let just_closed_braceless_if = false;
   _parseLines(lines, function(i, line) {
      if (extended_declaration) {
         //
         // We found the start of a block declaration, i.e. "blockname (", but the 
         // contents of the parenthetical were split across multiple lines. We need 
         // to search for any of the following:
         //
         //  - An opening curly brace, which marks the end of the declaration.
         //
         //  - A colon, which marks the start of the block's name, and thereby 
         //    guarantees that we will encounter a curly brace.
         //
         //  - If we have not encountered a colon and we encounter any character 
         //    that is not a curly brace, then this block must be a braceless block 
         //    with only a single child, and the encountered character must be the 
         //    first character of that child.
         //
         let must_have_braces = false;
         for(let j = 0; j < line.length; j++) {
            let c = line[j];
            if (extended_paren_count == 0) {
               if (c == "(" || c == ")") {
                  throw new Error("Syntax error: Unexpected parenthetical e.g. if (foo) (bar)");
               }
               if (c == "{") {
                  let b = new RawBlock(i);
                  b.declaration = extended_declaration + line.substring(0, j);
                  current_block = current_block.appendChild(b);
                  extended_declaration = "";
                  return line.substring(j + 1);
               }
               if (/\s/.test(c)) {
                  extended_declaration += c;
                  continue;
               }
               if (c == ":") {
                  extended_declaration += c;
                  must_have_braces = true;
                  continue;
               }
               if (!must_have_braces) {
                  //
                  // We've now passed the condition part of a braceless if-statement.
                  //
                  let b = new RawBlock(i);
                  b.declaration   = extended_declaration;
                  b.isSingleChild = true;
                  current_block = current_block.appendChild(b);
                  //
                  extended_declaration = "";
                  return line.substring(j);
               }
            }
            switch (c) {
               case "(":
                  extended_declaration += c;
                  extended_paren_count++;
                  break;
               case ")":
                  extended_declaration += c;
                  extended_paren_count--;
                  break;
               case "{":
                  throw new Error("Unexpected {");
               case "\n":
                  extended_declaration += " ";
                  break;
               default:
                  extended_declaration += c;
            }
         }
         return;
      }
      let curly_open  = line.indexOf("{");
      let curly_close = line.indexOf("}");
      if (curly_close >= 0) {
         if (curly_close > 0)
            current_block.appendLine(line.substring(0, curly_close));
         current_block = current_block.parent;
         if (!current_block)
            throw new Error("Extra closing brace!");
         line = line.substring(curly_close + 1).trim();
         if (line)
            return line;
         return;
      }
      if (curly_open >= 0) { // is block start
         let b = new RawBlock(i);
         b.declaration = line.substring(0, curly_open);
         current_block = current_block.appendChild(b);
         //
         if (curly_close > 0) {
            current_block.appendLine(line.substring(curly_open + 1, curly_close));
            return line.substring(curly_close);
         }
         current_block.appendLine(line.substring(curly_open + 1));
         return;
      }
      if (just_closed_braceless_if && line.startsWith("else")) {
         if (line.match(/^else\s+if\s*\(/)) {
            let j = line.indexOf("(");
            extended_declaration = line.substring(0, j + 1);
            extended_paren_count = 1;
            return line.substring(j + 1);
         }
         if (line.length == 4 || /\s/.test(line[4])) {
            let b = new RawBlock(i);
            b.declaration   = "else";
            b.isSingleChild = true;
            current_block = current_block.appendChild(b);
            return line.substring(5);
         }
      }
      if (line.startsWith("if")) { // special handling needed for multi-line ifs
         if (line.match(/^if\s*\(/)) {
            let j = line.indexOf("(");
            extended_declaration = line.substring(0, j + 1);
            extended_paren_count = 1;
            return line.substring(j + 1);
         }
      }
      if (!line || line.startsWith("//"))
         return;
      current_block.appendLine(line);
      if (current_block.isSingleChild) {
         current_block = current_block.parent;
         just_closed_braceless_if = true;
      } else
         just_closed_braceless_if = false;
   });
   if (extended_declaration || extended_paren_count) {
      throw new Error("An if-block's declaration overflowed!");
   }
   root_block.refine();
   return root_block;
}