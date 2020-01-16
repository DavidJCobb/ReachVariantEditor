const STATEMENT_TYPE_NONE    = 0;
const STATEMENT_TYPE_COMPARE = 1;
const STATEMENT_TYPE_ASSIGN  = 2;
const STATEMENT_TYPE_CALL    = 3;

class Statement {
   constructor() {
      this.type = STATEMENT_TYPE_NONE;
      this.text = "";
   }
}
class Block {
   constructor() {
      this.children = []; // statements or blocks
   }
}

const EXPR_PHASE_LEFTHAND  = 0;
const EXPR_PHASE_OPERATOR  = 1;
const EXPR_PHASE_RIGHTHAND = 2;

class Expression {
   constructor() {
      this.type      = STATEMENT_TYPE_NONE;
      this.text      = "";
      this.lefthand  = "";
      this.operator  = "";
      this.righthand = "";
      this.callargs  = "";
      //
      this.phase       = 0; // lefthand, operator, righthand
      this.in_callargs = false;
   }
}

// VALID STATEMENTS:
// lhs operator rhs
// lhs.name()
// lhs.name([lhs[, lhs[...]])
//
// GIVEN:
// operator:
//    =
//    +=
//    -=
//    *=
//    /=
//    %=
//    >>=
//    <<=
//    >>>=
//    ==
//    >=
//    <=
//    >
//    <
//    !=
//
// lhs:
//    word
//    word[.word[...]]
//
// rhs:
//    word
//    word[.word[...]]
//    word[.word[...]].name()
//    integer
//
// word:
//    [alphanumerics not starting with a number]
//    word[integer]
//    word[alias]

function parseMegalo(text, block) {
   let root    = new Block();
   let current = root;
   //
   let length  = text.length;
   let comment = false;
   let expr    = new Expression;
   for(let i = 0; i < length; i++) {
      let c = text[i];
      if (comment) {
         if (c == "\n")
            comment = false;
         continue;
      }
      if (c == "-" && text[i + 1] == "-") {
         ++i;
         comment = true;
         continue;
      }
      
   }
   return root;
}