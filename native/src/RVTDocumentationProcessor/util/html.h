#pragma once
#include <functional>
#include <QString>
#include <QtXml>

namespace util {
   struct html_serialize_options {
      bool adapt_indented_pre_tags      = false;
      bool include_containing_element   = false;
      std::function<void(QDomElement, QString&)> pre_tag_content_tweak;
      std::function<void(QString&)> url_tweak;
      bool wrap_bare_text_in_paragraphs = false;

      // Can't make this member private without disallowing aggregate initialization, which would 
      // defeat the purpose of this struct. Just don't screw with it, I guess. Leave it false.
      bool _is_recursing = false;
   };
   extern QString serialize_element(QDomElement root, html_serialize_options options = html_serialize_options());

   struct xml_encode_options {
      bool escape_newlines = false;
      bool escape_quotes   = false;
      bool escape_brackets = true;
   };
   extern QString xml_encode(const QString& text, xml_encode_options options = xml_encode_options());
}