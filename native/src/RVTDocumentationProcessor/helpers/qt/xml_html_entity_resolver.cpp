#include "xml_html_entity_resolver.h"
#include <QDebug>
#include <array>
#include <cstdint>
#include "../all_html_entities.h"

namespace cobb::qt::xml {
   QString XmlHtmlEntityResolver::resolveUndeclaredEntity(const QString& name) {
      QString text = cobb::html::look_up_entity(name);
      if (text.isEmpty())
         qDebug() << QString("Unrecognized entity: %1\n").arg(name);
      return text;
   }
}