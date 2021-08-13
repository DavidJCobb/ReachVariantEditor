#include "xml.h"

namespace cobb::qt::xml {
   extern QVector<QDomElement> child_elements_of_type(QDomElement parent, const QString& node_name, Qt::CaseSensitivity cs) {
      QVector<QDomElement> out;
      //
      auto list = parent.childNodes();
      auto size = list.size();
      for (int i = 0; i < size; ++i) {
         auto item = list.item(i);
         if (item.nodeType() == QDomNode::NodeType::ElementNode) {
            auto elem = item.toElement();
            auto name = elem.nodeName();
            if (name.compare(node_name, cs) == 0)
               out.push_back(item.toElement());
         }
      }
      //
      return out;
   }
   extern QDomElement first_child_element_of_type(QDomElement parent, const QString& node_name, Qt::CaseSensitivity cs) {
      auto list = parent.childNodes();
      auto size = list.size();
      for (int i = 0; i < size; ++i) {
         auto item = list.item(i);
         if (item.nodeType() == QDomNode::NodeType::ElementNode) {
            auto elem = item.toElement();
            auto name = elem.nodeName();
            if (name.compare(node_name, cs) == 0)
               return elem;
         }
      }
      return QDomElement();
   }

   extern bool element_has_child_elements(QDomElement parent) {
      auto list = parent.childNodes();
      auto size = list.size();
      for (int i = 0; i < size; ++i)
         if (list.item(i).isElement())
            return true;
      return false;
   }
}