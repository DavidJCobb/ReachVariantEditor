#include "xml.h"
#include <algorithm>
#include <cstdarg>
#include "strings.h"

namespace {
   const char* entities[] = {
      "&amp;",
      "&apos;",
      "&gt;",
      "&lt;",
      "&nbsp;",
      "&newline;",
      "&quot;",
      "&minus;",
   };
   constexpr char results[] = {
      '&',
      '\'',
      '>',
      '<',
      ' ',
      '\n',
      '"',
      '-',
   };
   static_assert(std::extent<decltype(entities)>::value == std::extent<decltype(results)>::value, "List of allowed XML entities and list of XML entity replacement strings are mismatched!");

   // returns number of chars skipped
   uint32_t _handleXMLEntity(cobb::xml::document& doc, const char* ampersand, std::string& writeTo, uint32_t lineNumber, uint32_t colNumber) {
      char d = ampersand[1];
      char e = d ? ampersand[2] : '\0';
      if (d == '#') {
         const char* offset = ampersand + 2;
         uint32_t base = 10;
         if (e == 'x') {
            base = 16;
            offset++;
         }
         char*    end    = nullptr;
         uint32_t number = strtoul(offset, &end, base);
         if ((end == offset) || (end && *end != ';')) {
            doc.log_error("found a '&' glyph that isn't a valid character entity.", lineNumber, colNumber);
            return 0;
         }
         if (number == 0) {
            doc.log_error("null characters are not allowed in this document even when escaped using XML entities.", lineNumber, colNumber);
            return 0;
         }
         if (number > 255) {
            doc.log_formatted_error("character entity uses code %d, which is out-of-range; Oblivion uses single-byte text. Skipping this entity.", lineNumber, colNumber, number);
         } else {
            writeTo += (char)number;
         }
         return end - ampersand;
      }
      auto _compare = &strncmp;
      if (doc.case_insensitive)
         _compare = &_strnicmp;
      for (auto it = doc.entities.begin(); it != doc.entities.end(); ++it) {
         auto& entity = it->entity;
         auto  length = entity.size();
         if (_compare(ampersand, entity.c_str(), length) == 0) {
            writeTo += it->substitution;
            return length;
         }
      }
      for (uint32_t i = 0; i < std::extent<decltype(entities)>::value; i++) {
         auto entity = entities[i];
         auto length = strlen(entity);
         if (_compare(ampersand, entity, length) == 0) {
            writeTo += results[i];
            return length;
         }
      }
      doc.log_error("found a '&' glyph that isn't a valid entity.", lineNumber, colNumber);
      return 0;
   }
};

namespace cobb {
   namespace xml {
      void document::clear() {
         this->tokens.clear();
      }
      void document::define_entity(const char* entity, const char* substitution) {
         this->entities.emplace_back(entity, substitution);
      }
      void document::define_html_entities() {
         const char* entities[] = {
            "&copy;",
            "&mdash;",
            "&reg;",
         };
         for (size_t i = 0; i < std::extent<decltype(entities)>::value; ++i)
            this->define_entity(entities[i], entities[i]);
      }

      int32_t document::find_element(const char* tagname) const {
         for (size_t i = 0; i < this->tokens.size(); ++i) {
            auto& token = this->tokens[i];
            if (token.code != token_code::element_open)
               continue;
            if (token.name == tagname)
               return i;
            if (this->case_insensitive && strncmp(token.name.c_str(), tagname, token.name.size()) == 0)
               return i;
         }
         return -1;
      }
      void document::element_content_to_string(uint32_t index, std::string& out) {
         out.clear();
         //
         std::string name;
         uint32_t    nesting = 1;
         bool        is_in_open_tag = false;
         //
         name = this->tokens[index].name;
         //
         for (size_t i = index + 1; i < this->tokens.size(); ++i) {
            auto&       token = this->tokens[i];
            std::string temp;
            switch (token.code) {
               case token_code::element_open:
                  if (is_in_open_tag)
                     out += '>';
                  cobb::sprintf(temp, "<%s", token.name.c_str());
                  is_in_open_tag = true;
                  if (token.name == name)
                     ++nesting;
                  break;
               case token_code::attribute:
                  cobb::sprintf(temp, " %s=\"%s\"", token.name.c_str(), token.value.c_str());
                  break;
               case token_code::text_content:
                  if (is_in_open_tag)
                     out += '>';
                  is_in_open_tag = false;
                  out += token.value;
                  break;
               case token_code::element_close:
                  is_in_open_tag = false;
                  if (token.name == name) {
                     --nesting;
                     if (!nesting)
                        break;
                  }
                  cobb::sprintf(temp, "</%s>", token.name.c_str());
                  break;
            }
            out += temp;
            if (nesting == 0)
               break;
         }
      }
      const char* document::root_node_name() const {
         for (auto& token : this->tokens)
            if (token.code == token_code::element_open)
               return token.name.c_str();
         return "";
      }

      void document::add_element_start(uint32_t line, std::string& tagName) {
         this->tokens.emplace_back(line, token_code::element_open);
         auto last = this->tokens.rbegin();
         if (this->case_insensitive) {
            std::transform(tagName.begin(), tagName.end(), tagName.begin(), tolower);
         }
         std::swap(last->name, tagName);
      }
      void document::add_element_end(uint32_t line, std::string& tagName) {
         this->tokens.emplace_back(line, token_code::element_close);
         auto last = this->tokens.rbegin();
         if (this->case_insensitive) {
            std::transform(tagName.begin(), tagName.end(), tagName.begin(), tolower);
         }
         std::swap(last->name, tagName);
      }
      void document::add_element_attribute(uint32_t line, std::string& name, std::string& value) {
         this->tokens.emplace_back(line, token_code::attribute);
         auto last = this->tokens.rbegin();
         if (this->case_insensitive) {
            std::transform(name.begin(), name.end(), name.begin(), tolower);
         }
         std::swap(last->name, name);
         std::swap(last->value, value);
      }
      void document::add_text_content(uint32_t line, std::string& textContent) {
         if (this->strip_whitespace)
            cobb::trim(textContent);
         if (!textContent.size())
            return;
         this->tokens.emplace_back(line, token_code::text_content);
         auto last = this->tokens.rbegin();
         std::swap(last->value, textContent);
      }
      void document::log_error(std::string text, int32_t line, int32_t col) {
         this->errors.emplace_back(text, line, col);
      }
      void document::log_formatted_error(std::string text, int32_t line, int32_t col, ...) {
         va_list args;
         va_start(args, col);
         va_list safe;
         va_copy(safe, args);
         //
         this->errors.emplace_back(text, line, col);
         auto& err = this->errors.back();
         auto& out = err.text;
         {
            char b[128];
            if (vsnprintf(b, sizeof(b), text.c_str(), args) < 128) {
               out = b;
               va_end(safe);
               va_end(args);
               return;
            }
         }
         uint32_t s = 256;
         char* b = (char*)malloc(s);
         int32_t r = vsnprintf(b, s, text.c_str(), args);
         while (r > s) {
            va_copy(args, safe);
            s += 20;
            free(b);
            b = (char*)malloc(s);
            r = vsnprintf(b, s, text.c_str(), args);
         }
         out = b;
         free(b);
         va_end(safe);
         va_end(args);
      };

      bool parse(document& doc, std::string content) {
         enum State {
            kState_InContent     = 0,
            kState_InOpenBracket = 1,
            kState_InOpenTag     = 2,
            kState_InCloseTag    = 3,
            kState_InAttribute   = 4,
            kState_InComment     = 5,
            kState_SelfClosing   = 6,
         };
         auto     size    = content.size();
         auto     data    = content.data();
         bool     isCDATA = false;
         State    state = kState_InContent;
         uint32_t stateChangeLine = 0;
         char     currentQuote     = '\0';
         uint32_t currentQuoteLine = 0;
         std::string name;
         std::string value;
         std::string lastTagName; // used for self-closing tags
         bool hasSpaces = false;
         //
         uint32_t lineNumber = 1;
         uint32_t colNumber  = 1;
         for (uint32_t i = 0; i < size; ++i) {
            char b = i > 0 ? data[i - 1] : '\0';
            char c = data[i];
            char d = i + 1 < size ? data[i + 1] : '\0';
            char e = i + 2 < size ? data[i + 2] : '\0';
            char f = i + 3 < size ? data[i + 3] : '\0';
            if (c == '\n') {
               lineNumber++;
               colNumber = 1;
            } else
               colNumber++;
            //
            if (i == 0) { // XML declaration; spec heavily implies it must be at the start of the file
               if (strncmp(data, "<?xml", 4) == 0) {
                  auto at = strstr(data, "?>");
                  if (!at) {
                     doc.log_error("The XML declaration is unterminated.", lineNumber);
                     return false;
                  }
                  i = (at - data) + 2 - 1; // plus length of "?>", minus 1 since continue skips a char
                  continue;
               }
               if (doc.allow_html && strncmp(data, "<!doctype", 8) == 0) {
                  auto at = strstr(data, ">");
                  if (!at) {
                     doc.log_error("The HTML doctype is unterminated.", lineNumber);
                     return false;
                  }
                  i = (at - data) + 2 - 1; // plus length of "?>", minus 1 since continue skips a char
                  continue;
               }
            }
            //
            if (state != kState_InComment) {
               //
               // Handle CDATA.
               //
               if (!isCDATA) {
                  if (i + 9 < size && strncmp(data + i, "<![CDATA[", 9) == 0) {
                     isCDATA = true;
                     i += 8; // continue skips one more char
                     continue;
                  }
               } else {
                  if (c == ']' && d == ']' && e == '>') {
                     isCDATA = false;
                     i += 2; // continue skips one more char
                     continue;
                  }
                  switch (state) {
                     case kState_InContent:
                        value += c;
                        break;
                     case kState_InOpenBracket:
                        name += c;
                        break;
                     case kState_InOpenTag:
                        name += c;
                        break;
                     case kState_InAttribute:
                        if (!currentQuote) {
                           doc.log_error("expected attribute value opening quote; got CDATA instead.", lineNumber, colNumber);
                           return false;
                        }
                        value += c;
                        break;
                     case kState_SelfClosing:
                        break;
                     case kState_InCloseTag:
                        name += c;
                        break;
                  }
                  continue;
               }
            }
            //
            // Handle non-CDATA:
            //
            if (state == kState_InContent) {
               if (c == '<') {
                  name = "";
                  lastTagName = "";
                  state = kState_InOpenBracket;
                  stateChangeLine = lineNumber;
                  if (value.size()) {
                     doc.add_text_content(lineNumber, value);
                     value = "";
                  }
                  continue;
               }
               if (c == '&') {
                  uint32_t target = _handleXMLEntity(doc, data + i, value, lineNumber, colNumber);
                  if (target == 0)
                     return false;
                  i += target - 1; // continue skips 1 char
                  continue;
               }
               value += c;
               continue;
            }
            if (state == kState_InOpenBracket) {
               if (strchr("<&", c)) {
                  doc.log_formatted_error("char '%c' is not valid after an opening angle bracket.", lineNumber, colNumber, c);
                  return false;
               }
               if (!name.size()) {
                  if (c == '/') {
                     state = kState_InCloseTag;
                     stateChangeLine = lineNumber;
                     continue;
                  }
                  if (b == '<' && c == '!' && d == '-' && e == '-') { // test for comments
                     state = kState_InComment;
                     stateChangeLine = lineNumber;
                     name  = "";
                     value = "";
                     i += 2; // continue will skip one more char
                     continue;
                  }
                  if (strchr(">=", c)) {
                     doc.log_error("opening tag with no tagname.", lineNumber, colNumber);
                     return false;
                  }
                  if (strchr("'\"&", c)) {
                     doc.log_formatted_error("unexpected '%c'.", lineNumber, colNumber, c);
                     return false;
                  }
                  if (c == '?') {
                     if (strncmp(data + i - 1, "<?xml", 5) == 0) {
                        doc.log_error("apparent XML declaration not at the very start of the file. There can't be anything before the declaration -- not even comments or whitespace.", lineNumber, colNumber);
                        return false;
                     }
                     doc.log_error("unexpected '?'.", lineNumber, colNumber);
                     return false;
                  }
                  if (!isspace(c))
                     name += c;
               } else {
                  if (isspace(c)) {
                     lastTagName = name;
                     doc.add_element_start(lineNumber, name);
                     state = kState_InOpenTag;
                     stateChangeLine = lineNumber;
                     name  = "";
                     value = "";
                     continue;
                  }
                  if (c == '/') {
                     lastTagName = name;
                     state = kState_SelfClosing;
                     stateChangeLine = lineNumber;
                     continue;
                  }
                  if (c == '>') {
                     doc.add_element_start(lineNumber, name);
                     value = "";
                     state = kState_InContent;
                     stateChangeLine = lineNumber;
                     continue;
                  }
                  if (c == '=') {
                     doc.log_error("'=' glyph after a tag name.", lineNumber, colNumber);
                     return false;
                  }
                  if (c == '&') {
                     doc.log_error("'&' glyph not permitted here.", lineNumber, colNumber);
                     return false;
                  }
                  name += c;
               }
            }
            if (state == kState_InComment) {
               if (c == '-' && d == '-') {
                  if (e == '>') {
                     state = kState_InContent;
                     stateChangeLine = lineNumber;
                     i += 2; // continue will skip one more char
                     continue;
                  }
                  doc.log_error("for compatibility with SGML, XML does not allow the string \"--\" in comments.", lineNumber, colNumber);
                  return false;
               }
            }
            if (state == kState_InOpenTag) {
               if (isspace(c))
                  continue;
               if (!name.size()) {
                  if (strchr("<=&", c)) {
                     doc.log_formatted_error("expected attribute name or end of opening tag; got '%c'.", lineNumber, colNumber, c);
                     return false;
                  }
                  if (c == '/') {
                     state = kState_SelfClosing;
                     stateChangeLine = lineNumber;
                     continue;
                  }
                  if (c == '>') {
                     state = kState_InContent;
                     stateChangeLine = lineNumber;
                     value = "";
                     continue;
                  }
               } else {
                  if (c == '=') {
                     state = kState_InAttribute;
                     stateChangeLine = lineNumber;
                     value = "";
                     continue;
                  }
                  if (strchr("<>'\"/&", c)) {
                     doc.log_formatted_error("expected '=' glyph in attribute; got '%c' instead.", lineNumber, colNumber, c);
                     return false;
                  }
               }
               name += c;
            }
            if (state == kState_InAttribute) {
               if (!currentQuote) {
                  if (isspace(c))
                     continue;
                  if (c == '"' || c == '\'') {
                     currentQuote = c;
                     currentQuoteLine = lineNumber;
                     continue;
                  }
                  doc.log_formatted_error("expected attribute value opening quote; got '%c' instead.", lineNumber, colNumber, c);
                  return false;
               }
               if (c == currentQuote) {
                  doc.add_element_attribute(lineNumber, name, value);
                  state = kState_InOpenTag;
                  stateChangeLine = lineNumber;
                  currentQuote = '\0';
                  continue;
               }
               if (c == '&') {
                  uint32_t target = _handleXMLEntity(doc, data + i, value, lineNumber, colNumber);
                  if (target == 0)
                     return false;
                  i += target - 1; // continue skips 1 char
                  continue;
               }
               value += c;
            }
            if (state == kState_SelfClosing) {
               if (isspace(c))
                  continue;
               if (c == '>') {
                  if (!lastTagName.size()) {
                     doc.log_error("unable to remember name of self-closing tag.", lineNumber, colNumber);
                     return false;
                  }
                  doc.add_element_end(lineNumber, lastTagName);
                  lastTagName = "";
                  value = "";
                  state = kState_InContent;
                  stateChangeLine = lineNumber;
                  continue;
               }
               doc.log_formatted_error("expected end of self-closing tag ('<'); got '%c'.", lineNumber, colNumber, c);
               return false;
            }
            if (state == kState_InCloseTag) {
               if (!name.size()) {
                  if (isspace(c))
                     continue;
                  if (c == '>') {
                     doc.log_error("closing tag with no tagname.", lineNumber, colNumber);
                     return false;
                  }
                  if (strchr("/='\"<&", c)) {
                     doc.log_formatted_error("expected a closing tagname; got '%c'.", lineNumber, colNumber, c);
                     return false;
                  }
                  name += c;
                  hasSpaces = false;
               } else {
                  if (isspace(c)) {
                     hasSpaces = true;
                     continue;
                  }
                  if (hasSpaces) {
                     doc.log_formatted_error("unknown content after a closing tag's tagname, beginning with '%c'.", lineNumber, colNumber, c);
                     return false;
                  }
                  if (strchr("/='\"<&", c)) {
                     doc.log_formatted_error("unexpected char '%c' found in a closing tag.", lineNumber, colNumber, c);
                     return false;
                  }
                  if (c == '>') {
                     doc.add_element_end(lineNumber, name);
                     value = "";
                     state = kState_InContent;
                     stateChangeLine = lineNumber;
                     continue;
                  }
                  name += c;
               }
            }
         }
         if (isCDATA) {
            doc.log_error("unterminated CDATA.");
            return false;
         }
         if (currentQuote) {
            doc.log_formatted_error("runaway attribute-value with opening quotation mark '%c', beginning on the indicated line, reached the end of the document.", currentQuoteLine, document::error::no_col_number, currentQuote);
            return false;
         }
         if (state != kState_InContent) { // report runaway states
            switch (state) {
               case kState_InAttribute:
                  doc.log_error("runaway attribute reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
               case kState_InComment:
                  doc.log_error("runaway comment reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
               case kState_InCloseTag:
                  if (name.size())
                     doc.log_formatted_error("runaway close tag for tagname \"%s\" reached the end of the document. Token began near the indicated line.", stateChangeLine, document::error::no_col_number, name.c_str());
                  else
                     doc.log_error("runaway close tag with no name reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
               case kState_InOpenBracket:
                  if (name.size())
                     doc.log_formatted_error("runaway open-angle-bracket reached the end of the document; tagname may have been \"%s\". Token began near line %d.", stateChangeLine, document::error::no_col_number, name.c_str());
                  else
                     doc.log_error("runaway open-angle-bracket reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
               case kState_InOpenTag:
                  if (name.size())
                     doc.log_formatted_error("runaway open tag for tagname \"%s\" reached the end of the document. Token began near line %d.", stateChangeLine, document::error::no_col_number, name.c_str());
                  else
                     doc.log_error("runaway open tag with no name reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
               case kState_SelfClosing:
                  if (lastTagName.size())
                     doc.log_formatted_error("runaway self-closing tag for tagname \"%s\" reached the end of the document. Token began near line %d.", stateChangeLine, document::error::no_col_number, lastTagName.c_str());
                  else
                     doc.log_error("runaway self-closing tag (unidentifiable) reached the end of the document; line number is roughly where the token began.", stateChangeLine);
                  break;
            }
            return false;
         }
         {  // Verify proper nesting.
            bool foundRoot = false;
            std::vector<token*> hierarchy;
            for (auto it = doc.tokens.begin(); it != doc.tokens.end(); ++it) {
               auto& token = *it;
               switch (token.code) {
                  case token_code::element_open:
                     if (!doc.is_fragment && !hierarchy.size()) {
                        if (foundRoot) {
                           doc.log_error("there can only be one root node.", token.line);
                           return false;
                        }
                        foundRoot = true;
                     }
                     hierarchy.push_back(&token);
                     break;
                  case token_code::element_close:
                     if (!hierarchy.size()) {
                        doc.log_formatted_error("close tag for \"%s\" found outside of any element.", token.line, document::error::no_col_number, token.name.c_str());
                        return false;
                     }
                     auto& last = (*hierarchy.rbegin())->name;
                     if (token.name != last) {
                        doc.log_formatted_error("mismatched tags: expected close tag for \"%s\" and got close tag for \"%s\" instead.", token.line, document::error::no_col_number, last.c_str(), token.name.c_str());
                        return false;
                     }
                     hierarchy.pop_back();
                     break;
               }
            }
            auto size = hierarchy.size();
            if (size) {
               auto first = *hierarchy.begin();
               auto last  = *hierarchy.rbegin();
               if (size > 1)
                  doc.log_formatted_error("reached the end of the document with %d unclosed tags; the first was \"%s\" near line %d, and the last was \"%s\" near line %d.",
                     document::error::no_line_number,
                     document::error::no_col_number,
                     size,
                     first->name.c_str(),
                     first->line,
                     last->name.c_str(),
                     last->line
                  );
               else
                  doc.log_formatted_error("reached the end of the document with an unclosed tag named \"%s\" beginning near the indicated line.", first->line, document::error::no_col_number, first->name.c_str());
               return false;
            }
         }
         return true;
      };
   }
};