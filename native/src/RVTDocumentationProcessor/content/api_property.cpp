#include "api_property.h"
#include "api_type.h"
#include "registry.h"
#include "../helpers/qt/xml.h"
#include "../util/html.h"

namespace content {
   void api_property::load(QDomElement& root, api_type& member_of) {
      QString stem = QString("script/api/%1/%2/")
         .arg(member_of.name)
         .arg("properties");
      //
      this->name         = root.attribute("name");
      this->name2        = root.attribute("name2");
      this->type         = root.attribute("type");
      this->is_indexed   = root.attribute("indexed") == "true";
      this->is_read_only = root.attribute("read-only") == "true";
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
   }
   QString api_property::write(QString stem, api_type& member_of, page_creation_options options) {
      QString body;
      QString title;
      QString prefix;
      {
         prefix = member_of.name;
         if (!prefix.isEmpty())
            prefix += '.';
         //
         title = prefix + this->name;
         if (this->is_indexed)
            title += "[<var>n</var>]";
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
      if (this->is_read_only) {
         if (this->is_indexed)
            body += "<p>These values are read-only.</p>\n";
         else
            body += "<p>This value is read-only.</p>\n";
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