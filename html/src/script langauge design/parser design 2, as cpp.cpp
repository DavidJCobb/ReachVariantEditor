namespace MegaloParser {
   class ParserObject {
      public:
         virtual bool is_nestable() const noexcept { return false; }
         //
         ParserObject* parent = nullptr;
         //
         bool is_paren() const noexcept;
         bool is_somewhere_in_paren() const noexcept;
   }
   //
   class NestableParserObject : public ParserObject {
      public:
         virtual bool is_nestable() const noexcept override { return true; }
         //
         std::vector<ParserObject*> contents;
         //
         void append_child(ParserObject* o) noexcept {
            o->parent = this;
            this->contents.push_back(o);
         }
         void add_token(char c) noexcept {
            this->append_child(new Token(c));
         }
   }
   //
   class StringLiteral : public ParserObject {
      public:
         std::string contents;
   }
   class Text : public ParserObject {
      public:
         std::string contents;
   }
   class Token : public ParserObject {
      public:
         char c;
   }
   class Paren : public NestableParserObject {
      public:
   }
   class Block : public NestableParserObject {
      public:
         std::string type;
         Paren       args;
         std::string name;
         //
         void add_literal() noexcept {
            this->append_child(new StringLiteral);
         }
         //
         StringLiteral* last_literal() const noexcept {
            auto back = this->contents.back();
            return dynamic_cast<StringLiteral*>(back);
         }
   }
   
   enum comment_type {
      none,
      single_line,
      multi_line,
   }
   
   void parse(const std::string& text) {
      Block         root;
      ParserObject& current = root;
      size_t        size    = text.size();
      char          quote   = '\0';
      comment_type  comment = comment_type::none;
      for(size_t i = 0; i < size; i++) {
         auto c = text[i];
         if (quote) {
            if (quote == c) {
               quote = '\0';
               continue;
            }
            auto literal = current.last_literal();
            assert(literal, "No string literal to append to!");
            literal.contents += c;
            continue;
         }
         switch (comment) {
            case comment_type::none:
               break;
            case comment_type::single_line:
               if (c == '\n')
                  comment = comment_type::none;
               continue;
            case comment_type::multi_line:
               if (strncmp(text.data() + i, "*/, 2) == 0)
                  comment = comment_type::none;
               continue;
         }
         switch (c) {
            case '"':
            case '\'':
               quote = c;
               current.add_literal();
               continue;
            case ':':
            case ';':
            case '\n':
               current.add_token(c);
               continue;
            case '/':
               if (i + 1 < size) {
                  char d = text[i + 1];
                  if (d == '/') {
                     comment = comment_type::single_line;
                     ++i;
                     continue;
                  }
                  if (d == '*') {
                     comment = comment_type::multi_line;
                     ++i;
                     continue;
                  }
               }
               break;
         }
         if (c == '(') {
            auto paren = new Paren;
            current.append_child(paren);
            current = *paren;
         } else if (c == ')') {
            auto paren = dynamic_cast<Paren*>(&current);
            assert(paren && "Unexpected closing parentheses!");
            assert(current.parent && "Parenthetical has no parent? How??");
            current = *current.parent;
            continue;
         } else if (c == '{') {
            if (current.is_paren() || current.is_somewhere_in_paren()) {
               assert(false && "Cannot begin a block while inside of parentheses!");
            }
            bool found_name = false; // upon encountering ":"
            bool found_arg  = false; // upon encountering a Paren
            bool found_type = false; // ?????
            //
            // TODO:
            //
            //  - Investigate the idea of using a Lua-like syntax; I recall hearing that 
            //    Lua's syntax was specially designed to make parsing as fast and simple 
            //    as possible. See if we can find any literature explaining how they 
            //    designed the syntax and how they implemented parsing. Once we have 
            //    information, we should decide on a syntax; if we want to persist with 
            //    a C-style syntax, then see the next bullet points (or work on those 
            //    anyway concurrent with research into a Lua-like syntax, just to have 
            //    something to do).
            //
            //  = Separately from handling script, a UI idea: we don't have to implement 
            //    plain-English fill-in-the-blank stuff as an entirely separate editing 
            //    mode. We can build it into the code editor as an optional feature: if 
            //    you place the cursor on a line of code, we can show the statement(s) 
            //    in a non-modal popup (akin to Visual Studio stuff) and there, you can 
            //    edit things as drop-downs if you wish. You could even hover over an 
            //    argument in either view to highlight it in the other view.
            //
            //  - Scan backward through (current)'s last-children (but not descendants) 
            //    for the block name, arguments, and type.
            //
            //     - Stop upon encountering a ";" token.
            //
            //     - Stop upon encountering an existing block.
            //
            //     - Stop upon encountering a string literal.
            //
            // THESE SHOULD ALL WORK:
            //
            // type {}
            // 
            // type : name {}
            // 
            // type (arg) : name {}
            // 
            // : name {}
            // 
            // {}
            // 
            // type
            // {
            // }
            //
            // type : name
            // {
            // }
            //
            // type :
            // name
            // {
            // }
            //
         } else if (c == '}') {
            auto block = dynamic_cast<Block*>(&current);
            assert(block && "Unexpected closing curly brace!");
            assert(current.parent && "Block has no parent? How??");
            current = *current.parent;
            continue;
         }
         if (current.is_nestable())
            current.add_token(c);
      }
      assert(!quote && "Runaway quotation mark!");
      assert(comment != comment_type::multi_line && "Runaway multi-line comment!");
      assert(&root == &current && "Unclosed block!");
      //
      __debugbreak();
   }
}