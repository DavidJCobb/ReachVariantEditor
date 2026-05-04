#include "./highlighter.h"
#include <array>
#include <QRegularExpressionMatchIterator>
#include <QString>

#include "../../helpers/ini.h"
#include "../../services/ini.h"
#include "../../services/syntax_highlight_option.h"

#include "./block_state.h"
#include "./block_state_none_param.h"
#include "./block_type.h"
#include "./keywords.h"

namespace {
   using namespace ui::megalo_syntax_highlighting;

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

   static const keyword* match_keyword(const QStringRef& view, const keyword* last) {
      const keyword* match = nullptr;
      if (last) {
         for (const auto k_idx : last->branches_to) {
            if (k_idx == keyword::index_of_none)
               break;
            const auto& k = all_keywords[k_idx];
            if (k.is_wildcard)
               return &k;
            const auto k_view = QLatin1String(k.word.data(), k.word.size());
            if (view == k_view)
               return &k;
         }
      }
      //
      // If we don't have a previous keyword, or if we failed to match the 
      // next word in a phrase (possibly because the phrase has ended), then 
      // we need to look for a new base-level keyword.
      //
      return lookup_keyword_by_name(view);
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
      for (const auto& op : operators)
         if (view.startsWith(op))
            return &op;
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
            if ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
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

namespace ui::megalo_syntax_highlighting {
   highlighter::highlighter(QObject* parent) : QSyntaxHighlighter(parent) {
      _init();
   }
   highlighter::highlighter(QTextDocument* document) : QSyntaxHighlighter(document) {
      _init();
   }
   void highlighter::_init() {
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

   void highlighter::highlightBlock(const QString& text) {
      auto state   = block_state(this->previousBlockState());
      auto initial = state;
      int  start   = 0;
      const auto size = text.size();
      
      const keyword* last_keyword = nullptr;

      if (state.type() == block_type::none) {
         block_state_none_param param{ state.param() };
         last_keyword = param.last_keyword();
      }
      
      bool backslash = false;
      for (int i = 0; i < size; ++i) {
         QChar c = text[i];
         if (state.is_none()) { // if not in a comment or string
            if (c == '"' || c == '\'') {
               start = i;
               state = block_state(block_type::string_simple, c.toLatin1());
               continue;
            }
            if (this->features.block_strings) {
               if (c == '[') {
                  int length =  0;
                  int level  = -1;
                  extract_long_bracket(QStringRef(&text, i, size - i), level, length);
                  if (level > block_state::max_supported_param)
                     level = -1;
                  if (level >= 0) {
                     start = i;
                     state = block_state(block_type::string_block, level);
                     continue;
                  }
               }
            }
            if (c == '-' && i + 1 < size && text[i + 1] == '-') {
               //
               // Handle comments:
               //
               int length =  0;
               int level  = -1;
               if (this->features.block_comments) {
                  QChar e = (i + 2) < size ? text[i + 2] : '\0';
                  if (e == '[') {
                     extract_long_bracket(QStringRef(&text, i + 2, size - i - 2), level, length);
                     if (level > block_state::max_supported_param)
                        level = -1;
                  }
               }
               start = i;
               if (level >= 0) {
                  state = block_state(block_type::comment_block, level);
               } else {
                  state = block_state(block_type::comment_line);
               }
               i += length;
               continue;
            }
            
            auto next = QStringRef(&text, i, size - i);
            if (!is_keyword_boundary(c)) { // handle keywords
               //
               // `c` is a character that is valid in keywords. But is it the *start* 
               // of a word?
               //
               if (i == 0 || is_keyword_boundary(text[i - 1])) {
                  //
                  // It *is* the start of a word, so let's extract that word and see 
                  // if it's a keyword specifically.
                  //
                  const auto view = extract_word(next);
                  if (last_keyword) {
                     const auto* this_keyword = lookup_subkeyword_by_name(*last_keyword, view);
                     if (this_keyword) {
                        last_keyword = this_keyword;
                        const auto length = view.size();
                        if (!this_keyword->is_wildcard)
                           this->setFormat(i, length, this->formats.subkeyword);
                        i += length - 1;
                        continue;
                     }
                  }
                  const auto* this_keyword = lookup_keyword_by_name(view);
                  if (this_keyword && this_keyword->is_subkeyword)
                     this_keyword = nullptr;
                  if (this_keyword) {
                     last_keyword = this_keyword;
                     const auto length = view.size();
                     this->setFormat(i, length, this->formats.keyword);
                     i += length - 1;
                     continue;
                  }
               }
            }
            if (i == 0 || is_keyword_boundary(text[i - 1])) { // handle number literals
               if (c.isDigit() || c == '-') {
                  int length = skip_number(next);
                  if (length) {
                     this->setFormat(i, length, this->formats.number);
                     i += length - 1;
                     continue;
                  }
               }
            }
            if (const auto* op = extract_operator(next)) { // handle operators
               auto length = op->size();
               this->setFormat(i, length, this->formats.op);
               i += length - 1;
               continue;
            }
            continue;
         }
         //
         // We're in a comment or string. Search for its end; if we find it, exit that 
         // state.
         //
         auto next   = QStringRef(&text, i, size - i);
         int  length = 0;
         switch (state.type()) {
            case block_type::string_simple:
               if (c == QChar::fromLatin1(state.param()) && (i == 0 || text[i - 1] != '\\')) {
                  this->setFormat(start, i - start + 1, this->formats.string.simple);
                  state.clear();
               }
               break;
            case block_type::string_block:
               if (extract_long_bracket_close(next, state.param(), length)) {
                  this->setFormat(start, i - start + length, this->formats.string.block);
                  state.clear();
                  i += length - 1;
               }
               break;
            case block_type::comment_block:
               if (extract_long_bracket_close(next, state.param(), length)) {
                  this->setFormat(start, i - start + length, this->formats.comment.block);
                  state.clear();
                  i += length - 1;
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
      switch (state.type()) {
         case block_type::comment_block:
            this->setFormat(start, text.size() - start, this->formats.comment.block);
            break;
         case block_type::comment_line:
            this->setFormat(start, text.size() - start, this->formats.comment.line);
            state.clear();
            break;
         case block_type::string_simple:
            if (backslash || initial.type() == block_type::string_simple) {
               this->setFormat(start, text.size() - start, this->formats.string.simple);
            } else {
               state.clear();
            }
            break;
         case block_type::string_block:
            this->setFormat(start, text.size() - start, this->formats.string.block);
            break;
         case block_type::none:
            {
               block_state_none_param param;
               param.set_last_keyword(last_keyword);
               state = block_state(block_type::none, (block_state_none_param::underlying_type)param);
            }
            break;
      }
      this->setCurrentBlockState((block_state::underlying_type)state);
   }

   void highlighter::reloadFormattingFromINI() {
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
}