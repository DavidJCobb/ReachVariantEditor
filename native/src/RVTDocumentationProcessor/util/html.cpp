#include "html.h"
#include "minimize_indent.h"
#include "../helpers/qt/xml.h"

namespace {
   namespace {
      QString _keywords[] = {
         "alias",
         "alt",
         "altif",
         "and",
         "declare",
         "do",
         "end",
         "enum",
         "for",
         "function",
         "if",
         "not",
         "on",
         "or",
         "then",
      };
      bool _is_operator_char(QChar c) {
         return QString("=<>!+-*/%&|~^").contains(c);
      }
      bool _is_quote_char(QChar c) {
         return QString("`'\"").contains(c);
      }
      bool _is_syntax_char(QChar c) {
         return QString("(),:").contains(c);
      }
   }
   void _syntax_highlight_in_html(const QString& in, QString& out) {
      out.clear();
      size_t size = in.size();
      out.reserve(size);
      //
      QString in_keyword;
      uint    keyword_done = 0;
      QString last_keyword; // TODO: use teal for words "attached" to a keyword, e.g. "[blue]for[/blue] [teal]each object with label[/teal]" or "[blue]declare[/blue] varname [teal]with network priority low[/teal]"
      bool    in_comment   = false;
      QChar   in_string    = '\0';
      for (uint i = 0; i < size; ++i) {
         QChar c = in[i];
         if (in_string != '\0') {
            if (c == in_string) {
               in_string = '\0';
               out += "</span>";
            }
            out += c;
            continue;
         }
         if (in_comment) {
            if (c == '\r' || c == '\n') {
               in_comment = false;
               out += "</span>";
            }
            out += c;
            continue;
         }
         if (_is_quote_char(c)) {
            in_string = c;
            out += "<span class=\"string\">";
         }
         if (c == '-' && i + 1 < size) {
            QChar d = in[i + 1];
            if (d == '-') {
               in_comment = true;
               out += "<span class=\"comment\">";
            }
         }
         //
         if (in_keyword.isEmpty()) {
            bool can_start = i == 0;
            if (i > 0)
               can_start = in[i - 1].isSpace();
            //
            if (can_start) {
               auto subject = QStringRef(&in).mid(i);
               for (auto& k : _keywords) {
                  if (subject == k) {
                     in_keyword   = k;
                     keyword_done = 0;
                     break;
                  }
               }
               if (!in_keyword.isEmpty()) {
                  bool ends = false;
                  if (i + in_keyword.size() == in.size())
                     ends = true;
                  else if (i + in_keyword.size() < in.size()) {
                     QChar d = in[i + in_keyword.size()];
                     ends = _is_quote_char(d) || _is_syntax_char(d) || d.isSpace() || _is_operator_char(d);
                  }
                  if (ends)
                     out += "<span class=\"keyword\">";
                  else
                     in_keyword.clear();
               }
            }
            if (in_keyword.isEmpty())
               last_keyword.clear();
         } else if (++keyword_done == in_keyword.size()) {
            last_keyword.clear();
            std::swap(last_keyword, in_keyword);
            out += "</span>";
         }
         //
         out += c;
      }
      if (!in_keyword.isEmpty() || in_comment || (in_string != '\0')) {
         out += "</span>";
      }
   }
   void _syntax_highlight_in_html(QString& out) {
      QString content;
      _syntax_highlight_in_html(out, content);
      out.swap(content);
   }
}

namespace {
   bool _is_relative_path(const QString& path) {
      if (path[0] == '/' || path[0] == '\\')
         return false;
      if (QStringRef(&path, 0, 7).compare(QString("script/")) == 0) // hack to avoid having to edit 90% of the documentation files again
         return false;
      return true;
   }
   void _fix_link(const QString& stem, QString& out) {
      if (out.startsWith("http"))
         return;
      //
      // Really hacky link/image fixup code:
      //
      QString hash;
      {
         auto h = out.lastIndexOf('#');
         if (h != -1) {
            hash = out.mid(h, out.size() - h);
            out = out.mid(0, h);
         }
      }
      if (_is_relative_path(out)) {
         //
         // Articles typically use hyperlink and image paths that are relative to themselves, so 
         // we need to fix those up since we're using a <base/> element to change relative paths 
         // (so that the nav and asset paths can work).
         //
         out = stem + out;
      } else if (out[0] == '/' || out[0] == '\\') {
         out = out.mid(1);
      }
      //
      // Links between articles usually lack file extensions.
      //
      auto a = out.lastIndexOf(".");
      auto b = out.lastIndexOf("/");
      if (a == -1 || (a < b && b != -1)) {
         out += ".html";
      }
      out += hash;
   }
}

QString html_serializer::serialize(QDomElement root) {
   bool is_pre_tag   = root.nodeName().compare("pre", Qt::CaseInsensitive) == 0;
   bool is_hyperlink = root.nodeName().compare("a",   Qt::CaseInsensitive) == 0;
   //
   QString out;
   for (auto node : cobb::qt::xml::const_iterable_node_list(root.childNodes())) {
      if (node.isText()) {
         auto text = node.nodeValue().toHtmlEscaped(); // TODO: this escapes double-quotes, too, and I'd rather it didn't...
         if (is_pre_tag) {
            text = util::minimize_indent(text);
            _syntax_highlight_in_html(text);
         }
         out += text;
         continue;
      }
      if (node.isElement()) {
         auto elem = node.toElement();
         auto name = elem.nodeName();
         out += QString("<%1").arg(name);
         //
         auto list = elem.attributes();
         auto size = list.size();
         for (int i = 0; i < size; ++i) {
            auto attr  = list.item(i).toAttr();
            auto name  = attr.name();
            auto value = attr.value();
            if (name.compare("href", Qt::CaseInsensitive) == 0 || name.compare("src", Qt::CaseInsensitive) == 0) {
               _fix_link(this->link_base_path, value);
            }
            out += QString(" %1=\"%2\"").arg(name).arg(value);
         }
         out += ">";
         out += this->serialize(elem);
         if (name.compare("img", Qt::CaseInsensitive) != 0) // IMG tags shouldn't be closed
            out += QString("</%1>").arg(name);
         continue;
      }
   }
   return out;
}


namespace util {
   extern QString serialize_element(QDomElement root, html_serialize_options options) {
      bool is_pre_tag   = root.nodeName().compare("pre", Qt::CaseInsensitive) == 0;
      bool is_hyperlink = root.nodeName().compare("a",   Qt::CaseInsensitive) == 0;
      //
      QString out;
      QString tag = root.nodeName();
      if (options.include_containing_element) {
         out  = "<";
         out += tag;
         //
         auto list = root.attributes();
         auto size = list.size();
         for (int i = 0; i < size; ++i) {
            auto attr  = list.item(i).toAttr();
            auto name  = attr.name();
            auto value = attr.value();
            if (options.url_tweak) {
               if (name.compare("href", Qt::CaseInsensitive) == 0 || name.compare("src", Qt::CaseInsensitive) == 0) {
                  (options.url_tweak)(value);
               }
            }
            value = xml_encode(value, {
               .escape_newlines = true,
               .escape_quotes   = true,
            });
            out += QString(" %1=\"%2\"").arg(name).arg(value);
         }
         out += ">";
      }
      //
      html_serialize_options recurse_options = options;
      recurse_options.include_containing_element = true;
      recurse_options._is_recursing              = true;
      //
      bool wrapping_in_paragraph = false;
      for (auto node : cobb::qt::xml::const_iterable_node_list(root.childNodes())) {
         if (node.isText()) {
            auto text = xml_encode(node.nodeValue());
            if (is_pre_tag) {
               if (options.adapt_indented_pre_tags)
                  text = minimize_indent(text);
               if (options.pre_tag_content_tweak)
                  (options.pre_tag_content_tweak)(text);
            }
            if (!wrapping_in_paragraph) {
               wrapping_in_paragraph = options.wrap_bare_text_in_paragraphs && !options._is_recursing && !text.trimmed().isEmpty();
               if (wrapping_in_paragraph)
                  out += "<p>";
            }
            out += text;
            continue;
         }
         if (node.isElement()) {
            if (wrapping_in_paragraph) {
               //
               // Stop paragraphs upon encountering a block-level element.
               //
               auto name = node.nodeName().toLower();
               if (name == "div" || name == "p") {
                  out += "</p>";
                  wrapping_in_paragraph = false;
               }
            }
            out += serialize_element(node.toElement(), recurse_options);
            continue;
         }
      }
      if (wrapping_in_paragraph) {
         out += "</p>";
         wrapping_in_paragraph = false;
      }
      if (options.include_containing_element) {
         if (tag.compare("img", Qt::CaseInsensitive) != 0) // IMG tags shouldn't be closed
            out += QString("</%1>").arg(tag);
      }
      return out;
   }

   extern QString xml_encode(const QString& text, xml_encode_options options) {
      int     size = text.size();
      QString out;
      out.reserve(size);
      for (int i = 0; i < size; ++i) {
         QChar c = text[i];
         if (c == '&') {
            out += "&amp;";
            continue;
         }
         if (c == '<') {
            out += "&lt;";
            continue;
         }
         if (c == '>') {
            out += "&gt;";
            continue;
         }
         if (options.escape_quotes) {
            if (c == '"') {
               out += "&quot;";
               continue;
            }
         }
         if (options.escape_newlines) {
            if (c == '\r' || c == '\n') {
               out += "&#x";
               out += QString::number(c.unicode(), 16);
               out += ';';
               continue;
            }
         }
         out += c;
      }
      return out;
   }
}