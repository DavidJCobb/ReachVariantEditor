#include "api_method.h"
#include "api_type.h"
#include "base.h"
#include "registry.h"
#include "../helpers/qt/xml.h"
#include "../util/html.h"

namespace content {
   QString api_method::filename() const noexcept {
      if (!this->overload_id.isEmpty())
         return QString("%1__%2.html").arg(this->name).arg(this->overload_id);
      return QString("%1.html").arg(this->name);
   }
   QString api_method::friendly_name(bool include_name2) const noexcept {
      QString out = this->name;
      if (!this->overload_name.isEmpty()) {
         out += QString(" (%1)").arg(this->overload_name);
      }
      if (include_name2 && !this->name2.isEmpty())
         out += QString(" <span class=\"a-k-a\">a.k.a.</span> %2").arg(this->name2);
      return out;
   }

   void api_method::load(QDomElement& root, base& member_of, bool is_condition) {
      QString stem = QString("script/api/%1/%2/")
         .arg(member_of.name)
         .arg(is_condition ? "conditions" : "actions");
      //
      this->name  = root.attribute("name");
      this->name2 = root.attribute("name2");
      this->return_value_type = root.attribute("returns");
      this->overload_name     = root.attribute("overload-name");
      this->overload_id       = root.attribute("overload-id");
      {
         auto value = root.attribute("nodiscard");
         if (!value.isEmpty()) {
            if (value.compare("true", Qt::CaseInsensitive) == 0)
               this->nodiscard = true;
            else if (value.compare("false", Qt::CaseInsensitive) == 0)
               this->nodiscard = false;
         }
      }
      {
         auto value = root.attribute("id");
         if (!value.isEmpty()) {
            bool ok;
            auto id = value.toInt(&ok);
            if (ok)
               this->opcode_id = id;
         }
      }
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
      //
      auto args = cobb::qt::xml::first_child_element_of_type(root, "args");
      {
         auto list = cobb::qt::xml::child_elements_of_type(args, "arg");
         if (list.isEmpty()) {
            //
            // We can't simply test for an absence of child elements, because the bare argument 
            // text may use HTML elements.
            //
            auto text = args.text().trimmed();
            if (!text.isEmpty())
               this->bare_argument_text = text;
         } else {
            html_serializer serializer;
            serializer.link_base_path = stem;
            //
            for (auto node : list) {
               auto elem = node.toElement();
               if (elem.isNull())
                  continue;
               if (elem.nodeName() != "arg")
                  continue;
               this->args.push_back(argument());
               auto& added = this->args.back();
               added.name = elem.attribute("name");
               added.type = elem.attribute("type");
               //
               // Get argument content:
               //
               added.content = serializer.serialize(elem);
            }
         }
      }
      //
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
   QString api_method::write(QString stem, base& member_of, page_creation_options options) {
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
      }
      //
      body = QString("<header><h1>%1</h1>%2</header>").arg(title);
      if (!this->name2.isEmpty()) {
         auto aka = QString("<span class=\"a-k-a\">a.k.a. %2%3</span>")
            .arg(prefix)
            .arg(this->name2);
         body = body.arg(aka);
      } else {
         body = body.arg("");
      }
      body += '\n';
      body += this->description_to_html();
      body += '\n';
      if (this->has_return_value()) {
         body += QString("<p>This function returns <a href=\"script/api/%1.html\">%1</a>.%2</p>")
            .arg(this->return_value_type)
            .arg(this->nodiscard ? " Calling this function without storing its return value in a variable is an error." : "");
      }
      if (this->args.size() || !this->bare_argument_text.isEmpty()) {
         body += "<h2>Arguments</h2>\n";
         if (!this->bare_argument_text.isEmpty()) {
            body += this->bare_argument_text;
         } else {
            body += "<dl>\n";
            for (auto& arg : this->args) {
               bool has_type = !arg.type.isEmpty() && arg.type.indexOf(':') == -1;
               //
               body += "   <dt>";
               if (has_type) {
                  body += QString("<a href=\"script/api/%1.html\">").arg(arg.type);
               }
               if (!arg.name.isEmpty()) {
                  body += arg.name;
               } else {
                  body += arg.type;
               }
               if (has_type)
                  body += "</a>";
               body += "</dt>\n      <dd>";
               if (arg.content.isEmpty()) {
                  api_type* target = nullptr;
                  if (has_type)
                     target = registry::get().get_type(arg.type);
                  if (target)
                     body += target->blurb;
                  else
                     body += "No description available.";
               } else
                  body += arg.content;
               body += "</dd>\n";
            }
            body += "</dl>\n";
         }
      }
      body += this->example_to_html();
      body += this->notes_to_html();
      body += this->relationships_to_html(&member_of, this->type);
      //
      options.body  = body;
      options.title = title;
      if (this->parent) {
         if (this->parent->type == entry_type::ns) {
            return registry::get().page_templates.ns_member.create_page(options);
         }
      }
      return registry::get().page_templates.type.create_page(options);
   }
}