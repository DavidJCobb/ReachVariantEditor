#include "xml_html_entity_resolver.h"
#include <QDebug>
#include <array>
#include <cstdint>

namespace {
   static QString make_character(uint c) {
      return QChar(c);
   }

   std::array entities = {
      std::pair<QString, QString>{ "mdash", make_character(0x2014) },
   };
}

namespace cobb::qt::xml {
   QString XmlHtmlEntityResolver::resolveUndeclaredEntity(const QString& name) {
      for (auto& pair : entities)
         if (name.compare(pair.first, Qt::CaseInsensitive) == 0)
            return pair.second;
      qDebug() << QString("Unrecognized entity: %1\n").arg(name);
      return "";
   }
}