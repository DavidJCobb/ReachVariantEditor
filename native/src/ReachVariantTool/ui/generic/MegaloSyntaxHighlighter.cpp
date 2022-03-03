/*

This file is provided under the Creative Commons 0 License.
License: <https://creativecommons.org/publicdomain/zero/1.0/legalcode>
Summary: <https://creativecommons.org/publicdomain/zero/1.0/>

One-line summary: This file is public domain or the closest legal equivalent.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include "MegaloSyntaxHighlighter.h"
#include <array>
#include <QRegularExpressionMatchIterator>
#include <QString>

#include "../../helpers/ini.h"
#include "../../services/ini.h"
#include "../../services/syntax_highlight_option.h"

namespace {
   using Keyword = MegaloSyntaxHighlighter::Keyword;
}
Keyword* Keyword::make_subkeyword(const QString& s, bool requires_next) {
   auto* k = new Keyword;
   k->word = s;
   k->is_subkeyword = true;
   k->requires_next = requires_next;
   this->possible.push_back(k);
   return k;
}
Keyword* Keyword::make_phrase(const QVector<QString>& s) {
   auto* next = this;
   for (auto& word : s) {
      next = next->make_subkeyword(word, true);
   }
   if (next != this || !next->is_subkeyword)
      next->requires_next = false;
   return next;
}

namespace {
   const std::array ini_settings = {
      &ReachINI::CodeEditor::sFormatCommentLine,
      &ReachINI::CodeEditor::sFormatCommentBlock,
      &ReachINI::CodeEditor::sFormatKeyword,
      &ReachINI::CodeEditor::sFormatSubkeyword,
      &ReachINI::CodeEditor::sFormatNumber,
      &ReachINI::CodeEditor::sFormatOperator,
      &ReachINI::CodeEditor::sFormatStringSimple,
      &ReachINI::CodeEditor::sFormatStringBlock,
   };

   static QVector<Keyword*> all_keywords;

   Keyword* _lookup_keyword(const QString& word) {
      for (auto* k : all_keywords)
         if (word.compare(k->word, Qt::CaseInsensitive) == 0)
            return k;
      return nullptr;
   }
   void _generate_all_keywords() {
      static bool done = false;
      if (done)
         return;
      done = true;
      //
      for (auto& s : { "alias", "alt", "altif", "and", "declare", "do", "end", "enum", "for", "function", "if", "not", "on", "or", "then" }) {
         auto* k = new Keyword;
         k->word = s;
         all_keywords.push_back(k);
      }
      //
      if (auto* word = _lookup_keyword("declare")) {
         auto* next = word->make_phrase({ "*", "with", "network", "priority" });
         next->make_subkeyword("low");
         next->make_subkeyword("high");
         next->make_subkeyword("local");
      }
      if (auto* word = _lookup_keyword("for")) {
         auto* next = word->make_subkeyword("each", true);
         next->make_subkeyword("player")->make_subkeyword("randomly");
         next->make_subkeyword("object")->make_subkeyword("with", true)->make_subkeyword("label");
         next->make_subkeyword("team");
      }
   }

   const std::array operators = {
      // Assign:
      QString("="),
      QString("+="),
      QString("-="),
      QString("*="),
      QString("/="),
      QString("~="),
      QString("^="),
      QString("<<="),
      QString(">>="),
      // Compare:
      QString("=="),
      QString("!="),
      QString("<"),
      QString(">"),
      QString("<="),
      QString(">="),
   };

   static bool is_keyword_boundary(QChar c) {
      if (c == '_')
         return false;
      if (c.isLetterOrNumber())
         return false;
      return true;
   }

   static bool is_name_char(QChar c) {
      // Per Lua 5.4 manual section 3.1: names can be Latin letters, Arabic-indic digits, and underscores, 
      // not beginning with a digit and not being a reserved word
      if (c == '_')
         return true;
      if (c.isLetterOrNumber())
         return true;
      return false;
   }

   //
   // Given a Lua long bracket, e.g. `[[` or `[===[`, extracts the level (number of equal signs) 
   // and the total length of the token. If there is no long bracket, the level is set to -1.
   // 
   // The first character in the QStringRef should be the first `[`.
   //
   static void extract_long_bracket(const QStringRef& view, int& level, int& token_length) {
      level        = -1;
      token_length =  0;
      if (view[0] != '[')
         return;
      bool match = true;
      bool ended = false;
      auto size  = view.size();
      int  i     = 1;
      for (; i < size; ++i) {
         QChar c = view[i];
         if (c != '=') {
            ended = c == '[';
            match = ended;
            break;
         }
      }
      if (match && ended) {
         token_length = i + 1;
         level        = i - 1;
      }
   }

   static bool extract_long_bracket_close(const QStringRef& view, int level, int& token_length) {
      token_length = 0;
      if (view[0] != ']')
         return false;
      bool match = true;
      bool ended = false;
      auto size  = view.size();
      int  i     = 1;
      for (; i < size; ++i) {
         QChar c = view[i];
         if (c != '=') {
            ended = c == ']';
            match = ended;
            break;
         }
      }
      if (match && ended) {
         if (i - 1 == level) {
            token_length = i + 1;
            return true;
         }
      }
      return false;
   }

   static const Keyword* match_keyword(const QStringRef& view, const Keyword* last) {
      auto     size  = view.size();
      Keyword* match = nullptr;
      if (last) {
         for (auto* k : last->possible) {
            if (k->word == "*") {
               match = k;
               break;
            }
            if (view.compare(k->word, Qt::CaseInsensitive) == 0) {
               match = k;
               break;
            }
         }
      }
      if (!match) {
         //
         // If we don't have a previous keyword, or if we failed to match the 
         // next word in a phrase (possibly because the phrase has ended), then 
         // we need to look for a new base-level keyword.
         //
         for (auto* k : all_keywords) {
            if (view.compare(k->word, Qt::CaseInsensitive) == 0) {
               match = k;
               break;
            }
         }
      }
      return match;
   }
   static QStringRef extract_word(const QStringRef& view) {
      int size  = view.size();
      int i     = 0;
      int brace = 0;
      for (; i < size; ++i) {
         auto c = view[i];
         if (c.isLetterOrNumber())
            continue;
         if (c == '_' || c == '[' || c == ']' || c == '.')
            continue;
         break;
      }
      return QStringRef(view.string(), view.position(), i);
   }

   static const QString* extract_operator(const QStringRef& view) {
      for (auto& keyword : operators)
         if (view.startsWith(keyword))
            return &keyword;
      return nullptr;
   }

   // returns length of the number, if there was a valid one at the start of the string
   static int skip_number(const QStringRef& view) {
      auto size = view.size();
      bool hex  = false;
      int  i    = 0;
      if (view[0] == '-')
         ++i;
      if (view.mid(i, 2).compare(QStringLiteral("0x"), Qt::CaseInsensitive) == 0) {
         hex  = true;
         i   += 2;
      }
      bool any_digits     = false;
      bool has_decimal    = false;
      bool has_scientific = false; // 1234e5 or 0x1234p5
      for (; i < size; ++i) {
         QChar c = view[i];
         if (c.isNumber()) {
            any_digits = true;
            continue;
         }
         if (c == '.') {
            if (has_decimal)
               break;
            has_decimal = true;
            continue;
         }
         if (c == '-' && i > 0) {
            //
            // Allow negative exponents; otherwise, stop.
            //
            QChar b = view[i - 1];
            if (hex) {
               if (b != 'p' && b != 'P')
                  break;
            }
            if (b != 'e' && b != 'E')
               break;
         }
         if (hex) {
            if (QStringLiteral("abcdefABCDEF").indexOf(c) >= 0) {
               any_digits = true;
               continue;
            }
            if (c == 'p' || c == 'P') {
               if (has_scientific)
                  break;
               has_decimal    = false; // ensure decimal points in the exponent are allowed
               has_scientific = true;
               continue;
            }
         } else {
            if (c == 'e' || c == 'E') {
               if (has_scientific)
                  break;
               has_decimal    = false; // ensure decimal points in the exponent are allowed
               has_scientific = true;
               continue;
            }
         }
         //
         // Non-number character:
         //
         break;
      }
      if (!any_digits)
         return 0;
      return i;
   }
}

MegaloSyntaxHighlighter::MegaloSyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document) {
   _generate_all_keywords();
   //
   this->reloadFormattingFromINI();
   QObject::connect(&ReachINI::getForQt(), &cobb::qt::ini::file::settingChanged, this, [this](cobb::ini::setting* setting, cobb::ini::setting_value_union oldValue, cobb::ini::setting_value_union newValue) {
      for (auto* ptr : ini_settings) {
         if (setting == ptr) {
            this->reloadFormattingFromINI();
            return;
         }
      }
   });
}
void MegaloSyntaxHighlighter::highlightBlock(const QString& text) {
   auto state   = BlockState(this->previousBlockState());
   auto initial = state;
   int  start   = 0;
   const auto size = text.size();
   //
   const Keyword* last_keyword = nullptr;
   //
   if (state.is_none()) {
      auto prev = this->currentBlock().previous();
      if (prev.isValid()) {
         if (auto* data = dynamic_cast<ComplexBlockState*>(prev.userData()))
            last_keyword = data->keyword;
      }
   }
   //
   bool backslash = false;
   for (int i = 0; i < size; ++i) {
      QChar c  = text[i];
      auto  cd = text.mid(i, 2);
      if (state.is_none()) {
         if (c == '"' || c == '\'') {
            start = i;
            state = BlockState(TokenType::String_Simple, c.toLatin1());
            continue;
         }
         if (this->features.block_strings) {
            if (c == '[') {
               int length =  0;
               int level  = -1;
               extract_long_bracket(QStringRef(&text, i, size - i), level, length);
               if (level > BlockState::max_supported_param)
                  level = -1;
               if (level >= 0) {
                  start = i;
                  state = BlockState(TokenType::String_Block, level);
                  continue;
               }
            }
         }
         if (cd == "--") {
            //
            // Handle comments:
            //
            int   length =  0;
            int   level  = -1;
            if (this->features.block_comments) {
               QChar e = (i + 2) < size ? text.at(i + 2) : '\0';
               if (e == '[') {
                  extract_long_bracket(QStringRef(&text, i + 2, size - i - 2), level, length);
                  if (level > BlockState::max_supported_param)
                     level = -1;
               }
            }
            start = i;
            if (level >= 0) {
               state = BlockState(TokenType::Comment_Block, level);
            } else {
               state = BlockState(TokenType::Comment_Line);
            }
            i += length;
            continue;
         }
         //
         auto next = QStringRef(&text, i, size - i);
         if (!is_keyword_boundary(c)) {
            QChar b = i ? text[i - 1] : '\0';
            if (is_keyword_boundary(b)) {
               auto view = extract_word(next);
               last_keyword = match_keyword(view, last_keyword);
               if (last_keyword) {
                  auto& word   = last_keyword->word;
                  auto  length = view.size();
                  if (word != "*") {
                     this->setFormat(i, length, last_keyword->is_subkeyword ? this->formats.subkeyword : this->formats.keyword);
                  }
                  i += length - 1;
                  continue;
               }
            }
         }
         if (i == 0 || is_keyword_boundary(text[i - 1])) {
            if (c.isDigit() || c == '-') {
               int length = skip_number(next);
               if (length) {
                  this->setFormat(i, length, this->formats.number);
                  i += length - 1;
                  continue;
               }
            }
         }
         if (auto* keyword = extract_operator(next)) {
            auto length = keyword->size();
            this->setFormat(i, length, this->formats.op);
            i += length - 1;
            continue;
         }
         continue;
      }
      //
      // We're in some sort of special token.
      //
      auto next   = QStringRef(&text, i, size - i);
      int  length = 0;
      switch (state.code()) {
         case TokenType::String_Simple:
            backslash = (c == '\\');
            if (c == QChar::fromLatin1(state.param()) && (i == 0 || text[i - 1] != '\\')) {
               this->setFormat(start, i - start + 1, this->formats.string.simple);
               state.clear();
            }
            break;
         case TokenType::String_Block:
            if (extract_long_bracket_close(next, state.param(), length)) {
               this->setFormat(start, i - start + length, this->formats.string.block);
               state.clear();
               i += length - 1;
            }
            break;
         case TokenType::Comment_Block:
            if (extract_long_bracket_close(next, state.param(), length)) {
               if (text.mid(i + length, 2) == "--") {
                  this->setFormat(start, i - start + length + 2, this->formats.comment.block);
                  state.clear();
                  i += length + 2 - 1;
               }
            }
            break;
      }
   }
   //
   // Reached the end of the block. We typically apply formatting to tokens when we've 
   // encountered their ends, so if we're inside of any tokens now, we need to apply 
   // formatting at this time.
   // 
   // Qt's documentation doesn't say this outright, but the end of a text block is always 
   // either the end of a line or the end of the document entirely. As such, any states 
   // that should not persist past the end of the line must be exited here.
   //
   switch (state.code()) {
      case TokenType::Comment_Block:
         this->setFormat(start, text.size() - start, this->formats.comment.block);
         break;
      case TokenType::Comment_Line:
         this->setFormat(start, text.size() - start, this->formats.comment.line);
         state.clear();
         break;
      case TokenType::String_Simple:
         if (backslash || initial.code() == TokenType::String_Simple) {
            this->setFormat(start, text.size() - start, this->formats.string.simple);
         } else {
            state.clear();
         }
         break;
      case TokenType::String_Block:
         this->setFormat(start, text.size() - start, this->formats.string.block);
         break;
   }
   //
   // Block state and userdata:
   //
   auto* ud = dynamic_cast<ComplexBlockState*>(currentBlockUserData());
   if (last_keyword) {
      if (!ud) {
         ud = new ComplexBlockState;
         setCurrentBlockUserData(ud);
      }
      ud->keyword = last_keyword;
   } else {
      if (ud)
         ud->keyword = nullptr;
   }
   setCurrentBlockState(state.to_int());
}

void MegaloSyntaxHighlighter::reloadFormattingFromINI() {
   bool error;
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatCommentBlock.currentStr.c_str());
      auto& format  = this->formats.comment.block;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setFontItalic(true);
         format.setForeground(QColor::fromRgb(32, 160, 8));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatCommentLine.currentStr.c_str());
      auto& format  = this->formats.comment.line;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setFontItalic(true);
         format.setForeground(QColor::fromRgb(32, 160, 8));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatKeyword.currentStr.c_str());
      auto& format  = this->formats.keyword;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setFontWeight(QFont::Weight::Bold);
         format.setForeground(QColor::fromRgb(0, 16, 255));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatSubkeyword.currentStr.c_str());
      auto& format  = this->formats.subkeyword;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setFontWeight(QFont::Weight::Bold);
         format.setForeground(QColor::fromRgb(0, 120, 190));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatNumber.currentStr.c_str());
      auto& format  = this->formats.number;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setForeground(QColor::fromRgb(200, 100, 0));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatOperator.currentStr.c_str());
      auto& format  = this->formats.op;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setForeground(QColor::fromRgb(0, 0, 128));
         format.setFontWeight(QFont::Weight::Bold);
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatStringSimple.currentStr.c_str());
      auto& format  = this->formats.string.simple;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setForeground(QColor::fromRgb(140, 140, 140));
      }
   }
   {
      auto  setting = QString::fromUtf8(ReachINI::CodeEditor::sFormatStringBlock.currentStr.c_str());
      auto& format  = this->formats.string.block;
      auto  working = ReachINI::parse_syntax_highlight_option(setting, error);
      if (!error) {
         format = working;
      } else {
         format = QTextCharFormat();
         format.setForeground(QColor::fromRgb(160, 0, 80));
      }
   }
}