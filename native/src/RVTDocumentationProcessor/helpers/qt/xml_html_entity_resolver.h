#pragma once
#include <QXmlStreamEntityResolver>

namespace cobb::qt::xml {
   class XmlHtmlEntityResolver : public QXmlStreamEntityResolver {
      public:
         virtual QString resolveUndeclaredEntity(const QString& name) override;
   };
}