#pragma once
#include <QXmlStreamReader>

namespace util {
   extern QString strip_html(const QString& html) {
      QXmlStreamReader xml(QString("<dummy>%1</dummy>").arg(html));
      QString text;
      while (!xml.atEnd()) {
         switch (xml.readNext()) {
            case QXmlStreamReader::Characters:
               text += xml.text();
               break;
            case QXmlStreamReader::StartElement:
               text += xml.readElementText(QXmlStreamReader::ReadElementTextBehaviour::IncludeChildElements);
               break;
         }
      }
      if (text.isEmpty()) // edge-case: no tags = no output
         return html;
      return text;
   }
}