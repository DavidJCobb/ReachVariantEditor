#include "html.h"
#include "minimize_indent.h"
#include "../helpers/qt/xml.h"

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
         switch (c.unicode()) { // spaces
            case 0xA0: // nbsp
            case 0x2002:
            case 0x2003:
            case 0x2004:
            case 0x2005:
            case 0x2007:
            case 0x2008:
            case 0x2009:
            case 0x200A:
            case 0x200B:
            case 0x205F:
               out += QString("&#x%1;").arg(c.unicode(), 0, 16);
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