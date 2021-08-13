#include "api_namespace_member.h"
#include "api_namespace.h"
#include "api_type.h"
#include "registry.h"
#include "../helpers/qt/xml.h"
#include "../util/html.h"

namespace content {
   void api_namespace_member::load(QDomElement& root, api_namespace& member_of) {
      QString stem = QString("script/api/%1/")
         .arg(member_of.name);
      //
      this->name         = root.attribute("name");
      this->name2        = root.attribute("name2");
      this->type         = root.attribute("type");
      this->is_indexed   = root.attribute("indexed") == "true";
      this->is_read_only = root.attribute("read-only") == "true";
      this->is_none      = root.attribute("none") == "true";
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
   QString api_namespace_member::write(QString stem, api_namespace& member_of, page_creation_options options) {
      QString body;
      QString title;
      QString prefix;
      {
         prefix = member_of.name;
         if (member_of.type == entry_type::ns && prefix == "unnamed")
            prefix.clear();
         else if (prefix.startsWith("ns_"))
            prefix.remove(0, 3);
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
      if (this->is_indexed) {
         assert(!this->is_none && "how can a namespace member be none AND indexed?!");
         if (this->is_read_only)
            body += "<p>These values are read-only.</p>\n";
      } else {
         if (this->is_none)
            body += "<p>This is a \"none\" value, so you cannot assign to it or access members on it.</p>\n";
         else if (this->is_read_only)
            body += "<p>This value is read-only.</p>\n";
      }
      body += this->example_to_html();
      body += this->notes_to_html();
      body += this->relationships_to_html(&member_of, entry_type::generic);
      //
      options.body  = body;
      options.title = title;
      return registry::get().page_templates.ns_member.create_page(options);
   }
}