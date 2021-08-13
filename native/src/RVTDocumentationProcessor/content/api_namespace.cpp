#include "api_namespace.h"
#include "registry.h"
#include "../helpers/qt/save_file_to.h"
#include "../helpers/qt/xml.h"

namespace content {
   QString api_namespace::get_friendly_name() const noexcept {
      if (!this->friendly_name.isEmpty())
         return this->friendly_name;
      if (!this->name.isEmpty())
         return this->name;
      return this->name2;
   }

   void api_namespace::load(QDomDocument& doc) {
      auto root = doc.documentElement();
      assert(root.nodeName().compare("script-namespace", Qt::CaseSensitive) == 0);
      //
      this->name  = root.attribute("name");
      this->name2 = root.attribute("name2");
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "blurb");
         if (!node.isNull())
            this->_load_blurb(node, this->relative_folder_path);
      }
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "description");
         if (!node.isNull())
            this->_load_description(node, this->relative_folder_path);
      }
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "friendly");
         if (!node.isNull())
            this->friendly_name = node.text();
      }
      //
      auto methods = cobb::qt::xml::first_child_element_of_type(root, "methods");
      auto members = cobb::qt::xml::first_child_element_of_type(root, "members");
      //
      {
         auto conditions = cobb::qt::xml::first_child_element_of_type(methods, "conditions");
         auto actions    = cobb::qt::xml::first_child_element_of_type(methods, "actions");
         //
         for (auto node : cobb::qt::xml::const_iterable_node_list(conditions.childNodes())) {
            auto elem = node.toElement();
            if (elem.isNull())
               continue;
            if (elem.nodeName() != "method")
               continue;
            auto* added = new api_method(false);
            added->parent = this;
            added->load(elem, *this, true);
            this->insert_member(*added);
            //
            if (!added->name2.isEmpty()) {
               auto* stub = new api_method(false);
               stub->parent  = this;
               stub->is_stub = true;
               stub->name    = added->name2;
               this->insert_member(*stub);
            }
         }
         for (auto node : cobb::qt::xml::const_iterable_node_list(actions.childNodes())) {
            auto elem = node.toElement();
            if (elem.isNull())
               continue;
            if (elem.nodeName() != "method")
               continue;
            auto* added = new api_method(true);
            added->parent = this;
            added->load(elem, *this, false);
            this->insert_member(*added);
            //
            if (!added->name2.isEmpty()) {
               auto* stub = new api_method(true);
               stub->parent  = this;
               stub->is_stub = true;
               stub->name    = added->name2;
               this->insert_member(*stub);
            }
         }
      }
      for (auto node : cobb::qt::xml::const_iterable_node_list(members.childNodes())) {
         auto elem = node.toElement();
         if (elem.isNull())
            continue;
         if (elem.nodeName() != "member")
            continue;
         auto* member = new api_namespace_member;
         member->parent = this;
         member->load(elem, *this);
         this->insert_member(*member);
      }
      //
      this->sort_members();
   }
   void api_namespace::write(QString base_output_folder) {
      auto& registry = registry::get();
      //
      //
      // Write the namespace's own page:
      //
      {
         QString body = QString("<h1>%1</h1>\n").arg(this->get_friendly_name());
         body += this->description_to_html();
         //
         auto subfolder_path = this->relative_subfolder_path();
         //
         if (this->has_members_of_type(entry_type::generic)) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::generic);
            //
            body += "\n<h2>Members</h2>\n<dl>";
            this->for_each_generic_member([&folder, &body](auto& prop) {
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop.name)
                  .arg(prop.is_indexed ? "[<var>n</var>]" : "")
                  .arg(!prop.blurb.isEmpty() ? prop.blurb : "No description available.");
            });
            body += "</dl>\n";
         }
         if (this->has_members_of_type(entry_type::condition)) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::condition);
            //
            body += "\n<h2>Member conditions</h2>\n<dl>";
            this->for_each_condition([&folder, &body](auto& prop) {
               if (prop.is_stub)
                  return;
               body += QString(
                  "<dt><a href=\"%1%2\">%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop.filename())
                  .arg(prop.friendly_name(true))
                  .arg(!prop.blurb.isEmpty() ? prop.blurb : "No description available.");
            });
            body += "</dl>\n";
         }
         if (this->has_members_of_type(entry_type::action)) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::action);
            //
            body += "\n<h2>Member actions</h2>\n<dl>";
            this->for_each_action([&folder, &body](auto& prop) {
               if (prop.is_stub)
                  return;
               body += QString(
                  "<dt><a href=\"%1%2\">%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop.filename())
                  .arg(prop.friendly_name(true))
                  .arg(!prop.blurb.isEmpty() ? prop.blurb : "No description available.");
            });
            body += "</dl>\n";
         }
         body = registry::get().page_templates.article.create_page({
            .body  = body,
            .relative_folder_path = this->relative_folder_path,
            .title = this->get_friendly_name(),
         });
         //
         QString path = QDir(base_output_folder).absoluteFilePath(this->relative_folder_path);
         if (path.back() != '/' && path.back() != '\\')
            path += '/';
         path += "ns_";
         path += this->name;
         path += ".html";
         //
         cobb::qt::save_file_to(path, body);
      }
      //
      // Now prepare to generate the pages for members:
      //
      QString folder_path = QDir(base_output_folder).absoluteFilePath(this->relative_subfolder_path());
      if (folder_path.back() != '/' && folder_path.back() != '\\')
         folder_path += '/';
      //
      auto options = page_creation_options{
         .nav = {
            .actions    = this->actions_to_nav_html(),
            .conditions = this->conditions_to_nav_html(),
            .members    = this->generic_members_to_nav_html(),
            .member_of_link = QString("<a href=\"script/api/ns_%1.html\">%2</a>").arg(this->name).arg(this->get_friendly_name()),
         },
      };
      //
      // Generate:
      //
      {
         auto path = folder_path + "conditions/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         this->for_each_condition([this, &options, &path](auto& member) {
            if (member.is_stub)
               return;
            //
            auto filename = member.filename();
            auto content  = member.write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         });
      }
      {
         auto path = folder_path + "actions/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         this->for_each_action([this, &options, &path](auto& member) {
            if (member.is_stub)
               return;
            //
            auto filename = member.filename();
            auto content  = member.write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         });
      }
      {
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(folder_path);
         this->for_each_generic_member([this, &options, &folder_path](auto& member) {
            auto filename = member.filename();
            auto content  = member.write(folder_path, *this, options);
            //
            cobb::qt::save_file_to(folder_path + filename, content);
         });
      }
   }
}