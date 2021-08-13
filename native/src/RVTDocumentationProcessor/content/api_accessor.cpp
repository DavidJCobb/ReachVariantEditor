#include "api_accessor.h"
#include "api_type.h"
#include "registry.h"
#include "../helpers/qt/xml.h"
#include "../util/html.h"

namespace content {
   void api_accessor::load(QDomElement& root, api_type& member_of) {
      QString stem = QString("script/api/%1/%2/")
         .arg(member_of.name)
         .arg("accessors");
      //
      this->name  = root.attribute("name");
      this->name2 = root.attribute("name2");
      this->type  = root.attribute("type");
      if (root.hasAttribute("getter"))
         this->getter = true;
      if (root.hasAttribute("setter"))
         this->setter = true;
      //
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "blurb");
         if (!node.isNull())
            this->_load_blurb(node, stem);
      }
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "description");
         if (!node.isNull())
            this->_load_description(node, stem);
      }
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "example");
         if (!node.isNull())
            this->_load_example(node, stem);
      }
      for (auto node : cobb::qt::xml::const_iterable_node_list(root.childNodes())) {
         auto elem = node.toElement();
         if (elem.isNull())
            continue;
         if (elem.nodeName() == "note") {
            this->_load_note(elem, stem);
            continue;
         }
         if (elem.nodeName() == "related") {
            this->_load_relationship(elem);
            continue;
         }
      }
      this->_load_all_categories_in(root);
   }
   QString api_accessor::write(QString stem, api_type& member_of, page_creation_options options) {
      QString body;
      QString title;
      QString prefix;
      {
         prefix = member_of.name;
         if (!prefix.isEmpty())
            prefix += '.';
         //
         title = prefix + this->name;
      }
      //
      body = QString("<header><h1>%1</h1>%2</header>").arg(title);
      if (!this->type.isEmpty()) {
         auto type = registry::get().get_type(this->type);
         //
         body = body.arg(
            QString("<span class=\"type\">Type: <a href=\"script/api/%1.html\">%2</a></span>")
               .arg(this->type)
               .arg(type ? type->get_friendly_name() : this->type)
         );
      } else {
         body = body.arg("");
      }
      body += '\n';
      body += this->description_to_html();
      body += '\n';
      if (this->getter) {
         if (this->setter) {
            body += "<p>This accessor supports read and write access.</p>\n";
         } else {
            body += "<p>This accessor supports read access only.</p>\n"; // should never happen; accessors should only be used when the "set" opcode allows arbitrary mathematical operators
         }
      } else {
         body += "<p>This accessor supports write access only; you cannot read an existing value.</p>\n";
      }
      body += this->example_to_html();
      body += this->notes_to_html();
      body += this->relationships_to_html(&member_of, entry_type::accessor);
      //
      options.body  = body;
      options.title = title;
      return registry::get().page_templates.type.create_page(options);
   }
}