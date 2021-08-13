#include "megalo_syntax_highlight.h"
#include <array>
#include <QString>
#include <QVector>
#include "../helpers/qt/xml.h"
#include "html.h"
#include "minimize_indent.h"

namespace util::impl {
   int megalo_keyword::longest_phrase_matching(const phrase_t& words) const noexcept {
      int w_sz = words.size();
      int last = -1;
      for (int i = 0; i < this->phrases.size(); ++i) {
         auto& phrase = this->phrases[i];
         int   p_sz   = phrase.size();
         if (p_sz <= w_sz) {
            bool match = true;
            for (int i = 0; i < p_sz; ++i) {
               if (phrase[i] == "*")
                  continue;
               if (words[i].compare(phrase[i], Qt::CaseInsensitive) != 0) {
                  match = false;
                  break;
               }
            }
            if (match) {
               if (last >= 0) {
                  //
                  // We want to find the longest phrase that matches.
                  //
                  if (this->phrases[last].size() > p_sz)
                     continue;
               }
               last = i;
               continue;
            }
         }
      }
      return last;
   }
}

namespace {
   std::array keywords = {
      util::impl::megalo_keyword{ "alias" },
      util::impl::megalo_keyword{ "alt" },
      util::impl::megalo_keyword{ "altif" },
      util::impl::megalo_keyword{ "and" },
      util::impl::megalo_keyword{ "declare",
         {
            { "*", "with", "network", "priority", "default" },
            { "*", "with", "network", "priority", "low" },
            { "*", "with", "network", "priority", "high" },
            { "*", "with", "network", "priority", "local" },
         }
      },
      util::impl::megalo_keyword{ "do" },
      util::impl::megalo_keyword{ "end" },
      util::impl::megalo_keyword{ "enum" },
      util::impl::megalo_keyword{ "for",
         {
            { "each", "player" },
            { "each", "player", "randomly" },
            { "each", "object" },
            { "each", "object", "with", "label" },
            { "each", "team" },
         }
      },
      util::impl::megalo_keyword{ "function" },
      util::impl::megalo_keyword{ "if" },
      util::impl::megalo_keyword{ "not" },
      util::impl::megalo_keyword{ "on" },
      util::impl::megalo_keyword{ "or" },
      util::impl::megalo_keyword{ "then" },
   };

   static bool _is_operator_char(QChar c) {
      return QString("=<>!+-*/%&|~^").contains(c);
   }
   static bool _is_quote_char(QChar c) {
      return QString("`'\"").contains(c);
   }
   static bool _is_syntax_char(QChar c) {
      return QString("(),:").contains(c);
   }
   static bool _is_word_char(QChar c) {
      return !c.isSpace() && !_is_operator_char(c) && !_is_quote_char(c) && !_is_syntax_char(c);
   }

   QVector<QString> _extract_next_few_words(QStringRef source, int max_word_count = 6) {
      QVector<QString> words;
      //
      auto size = source.size();
      auto word = QString();
      for (int i = 0; i < size; ++i) {
         QChar c = source[i];
         if (_is_word_char(c)) {
            word += c;
         } else {
            if (!word.isEmpty()) {
               words.push_back(word);
               word.clear();
               if (words.size() >= max_word_count)
                  break;
            }
         }
      }
      if (!word.isEmpty())
         words.push_back(word);
      return words;
   }
}

namespace util {
   void megalo_syntax_highlighter::_exit_keyword() {
      state.keyword.main       = nullptr;
      state.keyword.phrase     = -1;
      state.keyword.subkeyword = -1;
   }
   /*static*/ QString megalo_syntax_highlighter::_enter_markup(const QString& type) {
      return QString("<span class=\"%1\">").arg(type);
   }
   /*static*/ QString megalo_syntax_highlighter::_exit_markup(const QString& type) {
      return "</span>";
   }

   QString megalo_syntax_highlighter::_escape_character(QChar c) const noexcept {
      if (!options.escape_xml_entities)
         return c;
      if (c == '<')
         return "&lt;";
      if (c == '>')
         return "&gt;";
      return c;
   }

   QString megalo_syntax_highlighter::parse_more(const QString& script, bool is_end) {
      QString out;
      if (script.isEmpty())
         return out;
      auto size = script.size();
      out.reserve(size);
      //
      for (uint i = 0; i < size; ++i) {
         QChar c = script[i];
         QChar d = i > 0 ? script[i - 1] : state.previous_char;
         //
         bool is_line_break = c == '\r' || c == '\n';
         //
         if (!state.past_indent) {
            state.past_indent = !c.isSpace();
            if (!state.past_indent && !is_line_break) {
               ++state.indent_count;
               if (state.indent_count <= options.reduce_indent_by)
                  continue;
            }
         } else {
            if (is_line_break)
               state.past_indent = false;
         }
         if (options.skip_xml_entities) {
            if (state.in_xml_entity) {
               if (c == ';')
                  state.in_xml_entity = false;
               out += c;
               continue;
            } else {
               if (c == '&') {
                  state.in_xml_entity = true;
                  out += c;
                  continue;
               }
            }
         }
         //
         if (state.in_string != '\0') {
            if (c == state.in_string) {
               state.in_string = '\0';
               out += _exit_markup("string");
            }
            out += _escape_character(c);
            continue;
         }
         if (state.in_comment) {
            if (is_line_break) {
               state.in_comment = false;
               out += _exit_markup("comment");
            }
            out += _escape_character(c);
            continue;
         }
         if (_is_quote_char(c)) {
            state.in_string = c;
            out += _enter_markup("string");
            out += c;
            continue;
         }
         if (c == '-' && i + 1 < size) {
            QChar d = script[i + 1];
            if (d == '-') {
               state.in_comment = true;
               out += _enter_markup("comment");
               out += c;
               continue;
            }
         }
         //
         // Non-string, non-comment content:
         //
         if (!state.keyword.main) {
            bool can_start = d.isNull() || d.isSpace();
            //
            if (can_start && _is_word_char(c)) {
               auto subject = QStringRef(&script).mid(i);
               auto words   = _extract_next_few_words(subject);
               //
               for (auto& k : keywords) {
                  if (k.initial.compare(words[0], Qt::CaseInsensitive) == 0) {
                     state.keyword.main = &k;
                     break;
                  }
               }
               if (state.keyword.main) {
                  //
                  // We seem to be in a keyword, but let's double-check that the keyword actually 
                  // ends, whether at the end of the script or at a punctuation or space character. 
                  // That way, we can avoid false-positives e.g. "alias" matching "aliasing."
                  //
                  auto k_sz = state.keyword.main->initial.size();
                  bool ends = false;
                  if (i + k_sz == script.size())
                     ends = true;
                  else
                     ends = !_is_word_char(script[i + k_sz]);
                  //
                  if (!ends) {
                     state.keyword.main = nullptr;
                  } else {
                     //
                     // We're definitely in a valid keyword. Let's pop out some HTML:
                     //
                     out += _enter_markup("keyword");
                     //
                     // Now let's test for sub-keywords.
                     //
                     words.pop_front(); // the first word will be the initial keyword. remove it.
                     state.keyword.phrase     = state.keyword.main->longest_phrase_matching(words);
                     state.keyword.subkeyword = -1;
                  }
               }
            }
         } else {
            auto& sk = state.keyword;
            //
            bool this_is_word = _is_word_char(c);
            bool prev_is_word = _is_word_char(script[i - 1]);
            //
            if (!this_is_word && prev_is_word) {
               //
               // End of a word.
               //
               bool is_subkeyword = sk.phrase >= 0 && sk.subkeyword >= 0;
               bool has_tag       = true;
               if (is_subkeyword) {
                  has_tag = sk.main->phrases[sk.phrase][sk.subkeyword] != "*";
               }
               if (has_tag)
                  out += _exit_markup(is_subkeyword ? "subkeyword" : "keyword");
               if (sk.phrase >= 0) {
                  ++sk.subkeyword;
                  if (sk.subkeyword >= sk.main->phrases[sk.phrase].size()) {
                     this->_exit_keyword();
                  }
               } else {
                  this->_exit_keyword();
               }
            } else if (this_is_word && !prev_is_word) {
               //
               // Start of a word.
               //
               bool has_tag = true;
               if (sk.phrase >= 0) {
                  auto& phrase = sk.main->phrases[sk.phrase];
                  auto  index  = sk.subkeyword;
                  if (index < phrase.size())
                     has_tag = phrase[index] != "*";
               }
               if (has_tag)
                  out += _enter_markup("subkeyword");
            }
         }
         //
         out += _escape_character(c);
      }
      state.previous_char = script.back();
      if (is_end)
         this->end();
      return out;
   }
   QString megalo_syntax_highlighter::end() {
      QString out;
      if (state.keyword.main || state.in_comment || (state.in_string != '\0')) {
         out += "</span>";
         this->_exit_keyword();
         state.in_comment = false;
         state.in_string  = '\0';
      }
      return out;
   }
   QString megalo_syntax_highlighter::before_markup_break() const noexcept {
      QString out;
      if (state.keyword.main) {
         auto& sk = state.keyword;
         out += this->_exit_markup(sk.subkeyword >= 0 ? "subkeyword" : "keyword");
      }
      if (state.in_comment)
         out += this->_exit_markup("comment");
      if (state.in_string != '\0')
         out += this->_exit_markup("string");
      return out;
   }
   QString megalo_syntax_highlighter::after_markup_break() const noexcept {
      QString out;
      if (state.keyword.main) {
         auto& sk = state.keyword;
         out += this->_enter_markup(sk.subkeyword >= 0 ? "subkeyword" : "keyword");
      }
      if (state.in_comment)
         out += this->_enter_markup("comment");
      if (state.in_string != '\0')
         out += this->_enter_markup("string");
      return out;
   }

   QString megalo_syntax_highlighter::parse_dom(QDomElement root, parse_dom_options pdo) {
      QString out;
      if (pdo.adjust_indent && pdo._minimum_indent < 0) {
         assert(!pdo._is_recursing);
         pdo._minimum_indent = get_minimum_indent(root.text());
         //
         options.reduce_indent_by = pdo._minimum_indent;
      }
      options.escape_xml_entities = true;
      options.skip_xml_entities   = false;
      //
      QString tag = root.nodeName();
      if (pdo._is_recursing || pdo.include_containing_element) {
         out  = "<";
         out += tag;
         //
         auto list = root.attributes();
         auto size = list.size();
         for (int i = 0; i < size; ++i) {
            auto attr  = list.item(i).toAttr();
            auto name  = attr.name();
            auto value = attr.value();
            if (pdo.url_tweak) {
               if (name.compare("href", Qt::CaseInsensitive) == 0 || name.compare("src", Qt::CaseInsensitive) == 0) {
                  (pdo.url_tweak)(value);
               }
            }
            value = xml_encode(value, {
               .escape_newlines = true,
               .escape_quotes   = true,
            });
            out += QString(" %1=\"%2\"").arg(name).arg(value);
         }
         out += ">";
         //
         out += this->before_markup_break();
      }
      //
      parse_dom_options recurse_options = pdo;
      recurse_options.include_containing_element = true;
      recurse_options._is_recursing              = true;
      //
      bool wrapping_in_paragraph = false;
      for (auto node : cobb::qt::xml::const_iterable_node_list(root.childNodes())) {
         if (node.isText()) {
            out += this->after_markup_break();
            out += this->parse_more(node.nodeValue());
            continue;
         }
         if (node.isElement()) {
            out += parse_dom(node.toElement(), recurse_options);
            continue;
         }
      }
      if (!pdo._is_recursing) {
         out += this->end();
      }
      if (pdo.include_containing_element) {
         out += QString("</%1>").arg(tag);
      }
      return out;
   }

   
   extern QString megalo_syntax_highlight(const QString& script) {
      megalo_syntax_highlighter highlighter;
      return highlighter.parse_more(script, true);
   }

}