#pragma once
#include <QtXml>
#include <QXmlStreamReader>

namespace cobb::qt::xml {
   //
   // Uses QXmlStreamReader APIs to produce a QDomDocument.
   //
   class XmlStreamReaderToDom : public QXmlStreamReader {
      public:
         QDomDocument document;
         struct {
            QString version;
            QString encoding;
            bool    standalone = false;
         } declaration;
      protected:
         QVector<QDomElement> depth;

         void _append(QDomNode);

      public:
         void parse(const QString&);

         struct {
            bool strip_comments   = false;
            bool strip_whitespace = false;
         } options;
   };
}