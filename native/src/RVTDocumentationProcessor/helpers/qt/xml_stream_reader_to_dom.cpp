#include "xml_stream_reader_to_dom.h"

namespace cobb::qt::xml {
   void XmlStreamReaderToDom::_append(QDomNode node) {
      if (node.isNull())
         return;
      if (this->depth.isEmpty())
         this->document.appendChild(node);
      else
         this->depth.back().appendChild(node);
   }
   void XmlStreamReaderToDom::parse(const QString& xml) {
      this->clear();
      this->addData(xml);
      bool fail = false;
      while (!this->atEnd()) {
         bool exit = false;
         auto type = this->readNext();
         switch (type) {
            case Invalid:
               break;
            case StartDocument:
               this->document = QDomDocument();
               this->declaration.encoding   = documentEncoding().toString();
               this->declaration.version    = documentVersion().toString();
               this->declaration.standalone = isStandaloneDocument();
               break;
            case EndDocument:
               exit = true;
               break;
            case StartElement:
               {
                  auto elem = this->document.createElementNS(namespaceUri().toString(), name().toString());
                  for (auto& attr : attributes())
                     elem.setAttributeNS(attr.namespaceUri().toString(), attr.name().toString(), attr.value().toString());
                  this->_append(elem);
                  this->depth.push_back(elem);
               }
               break;
            case EndElement:
               {
                  assert(!this->depth.isEmpty());
                  this->depth.pop_back();
               }
               break;
            case Characters:
               {
                  if (options.strip_whitespace && isWhitespace())
                     break;
                  if (isCDATA()) {
                     auto node = this->document.createCDATASection(text().toString());
                     this->_append(node);
                  } else {
                     auto node = this->document.createTextNode(text().toString());
                     this->_append(node);
                  }
               }
               break;
            case Comment:
               {
                  if (options.strip_comments)
                     break;
                  auto node = this->document.createComment(text().toString());
                  this->_append(node);
               }
               break;
            case DTD:
               {
                  auto doctype = this->document.implementation().createDocumentType(
                     dtdName().toString(),
                     dtdPublicId().toString(),
                     dtdSystemId().toString()
                  );
                  //for (auto& decl : notationDeclarations()) {} // QDomNotation is read-only
                  //for (auto& decl : entityDeclarations()) {}   // QDomEntity is read-only
                  this->document = QDomDocument(doctype);
               }
               break;
            case EntityReference:
               //
               // This branch only runs for unresolved entities, including those caught by 
               // our entityResolver (which is why there may be text to substitute in).
               //
               {
                  QDomText node;
                  node.setData(text().toString());
                  this->_append(node);
               }
               break;
            case ProcessingInstruction:
               {
                  auto pi = this->document.createProcessingInstruction(processingInstructionTarget().toString(), processingInstructionData().toString());
                  this->_append(pi);
               }
               break;
         }
         if (exit || fail)
            break;
         // do processing
      }
      if (!fail)
         fail = !this->depth.isEmpty();
      if (this->hasError() || fail) {
         qDebug() << this->document.toString();
         qDebug() << "\n\n";
         // do error handling
         this->document = QDomDocument();
      }
   }
}