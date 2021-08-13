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
   api_method* api_namespace::get_action_by_name(const QString&) const noexcept {
      for (auto& member : this->actions) {
         if (member->is_stub)
            continue;
         if (member->name == name || member->name2 == name)
            return member;
      }
      return nullptr;
   }
   api_method* api_namespace::get_condition_by_name(const QString&) const noexcept {
      for (auto& member : this->conditions) {
         if (member->is_stub)
            continue;
         if (member->name == name || member->name2 == name)
            return member;
      }
      return nullptr;
   }
   api_namespace_member* api_namespace::get_member_by_name(const QString&) const noexcept {
      for (auto& member : this->members)
         if (member->name == name || member->name2 == name)
            return member;
      return nullptr;
   }

   QString api_namespace::relative_subfolder_path() const noexcept {
      return QString("%1/ns_%2/")
         .arg(this->relative_folder_path)
         .arg(this->name);
   }
   /*static*/ QString api_namespace::sub_sub_folder_name_for(entry_type type) noexcept {
      switch (type) {
         case entry_type::action:
            return "actions/";
         case entry_type::condition:
            return "conditions/";
      }
      return "";
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
            this->conditions.push_back(added);
            //
            if (!added->name2.isEmpty()) {
               auto* stub = new api_method(false);
               this->conditions.append(stub);
               stub->parent  = this;
               stub->is_stub = true;
               stub->name    = added->name2;
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
            this->actions.push_back(added);
            //
            if (!added->name2.isEmpty()) {
               auto* stub = new api_method(true);
               this->actions.append(stub);
               stub->parent  = this;
               stub->is_stub = true;
               stub->name    = added->name2;
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
         this->members.push_back(member);
      }
      //
      std::sort(this->actions.begin(),    this->actions.end(),    [](const api_method* a, const api_method* b) { return a->name < b->name; });
      std::sort(this->conditions.begin(), this->conditions.end(), [](const api_method* a, const api_method* b) { return a->name < b->name; });
      std::sort(this->members.begin(),    this->members.end(),    [](const base* a, const base* b) { return a->name < b->name; });
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
         if (this->members.size()) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::generic);
            //
            body += "\n<h2>Members</h2>\n<dl>";
            for (auto* prop : this->members) {
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop->name)
                  .arg(prop->is_indexed ? "[<var>n</var>]" : "")
                  .arg(!prop->blurb.isEmpty() ? prop->blurb : "No description available.");
            }
            body += "</dl>\n";
         }
         if (this->conditions.size()) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::condition);
            //
            body += "\n<h2>Member conditions</h2>\n<dl>";
            for (auto* prop : this->conditions) {
               if (prop->is_stub)
                  continue;
               body += QString(
                  "<dt><a href=\"%1%2\">%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop->filename())
                  .arg(prop->friendly_name(true))
                  .arg(!prop->blurb.isEmpty() ? prop->blurb : "No description available.");
            }
            body += "</dl>\n";
         }
         if (this->actions.size()) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::action);
            //
            body += "\n<h2>Member actions</h2>\n<dl>";
            for (auto* prop : this->actions) {
               if (prop->is_stub)
                  continue;
               body += QString(
                  "<dt><a href=\"%1%2\">%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop->filename())
                  .arg(prop->friendly_name(true))
                  .arg(!prop->blurb.isEmpty() ? prop->blurb : "No description available.");
            }
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
            .members    = this->members_to_nav_html(),
            .member_of_link = QString("<a href=\"script/api/ns_%1.html\">%2</a>").arg(this->name).arg(this->get_friendly_name()),
         },
      };
      //
      // Generate:
      //
      {
         auto path = folder_path + "conditions/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         for (auto* member : this->conditions) {
            if (member->is_stub)
               continue;
            //
            auto filename = member->filename();
            auto content  = member->write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         }
      }
      {
         auto path = folder_path + "actions/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         for (auto* member : this->actions) {
            if (member->is_stub)
               continue;
            //
            auto filename = member->filename();
            auto content  = member->write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         }
      }
      {
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(folder_path);
         for (auto* member : this->members) {
            auto filename = member->filename();
            auto content  = member->write(folder_path, *this, options);
            //
            cobb::qt::save_file_to(folder_path + filename, content);
         }
      }
   }

   QString api_namespace::actions_to_nav_html() const noexcept {
      QString out;
      QString folder = this->relative_subfolder_path() + sub_sub_folder_name_for(entry_type::action);
      for (auto* prop : this->actions) {
         if (prop->is_stub) {
            auto* target = this->get_action_by_name(prop->name);
            if (target) {
               out += QString("<li><a href=\"%1%2\">%3</a></li>\n")
                  .arg(folder)
                  .arg(target->filename())
                  .arg(prop->friendly_name(false));
            }
            continue;
         }
         out += QString("<li><a href=\"%1%2\">%3</a></li>\n")
            .arg(folder)
            .arg(prop->filename())
            .arg(prop->friendly_name(false));
      }
      return out;
   }
   QString api_namespace::conditions_to_nav_html() const noexcept {
      QString out;
      QString folder = this->relative_subfolder_path() + sub_sub_folder_name_for(entry_type::condition);
      for (auto* prop : this->conditions) {
         if (prop->is_stub) {
            auto* target = this->get_condition_by_name(prop->name);
            if (target) {
               out += QString("<li><a href=\"%1%2\">%3</a></li>\n")
                  .arg(folder)
                  .arg(target->filename())
                  .arg(prop->friendly_name(false));
            }
            continue;
         }
         out += QString("<li><a href=\"%1%2\">%3</a></li>\n")
            .arg(folder)
            .arg(prop->filename())
            .arg(prop->friendly_name(false));
      }
      return out;
   }
   QString api_namespace::members_to_nav_html() const noexcept {
      QString out;
      QString folder = this->relative_subfolder_path() + sub_sub_folder_name_for(entry_type::generic);
      for (auto* prop : this->members) {
         out += QString("<li><a href=\"%1%2\">%3%4</a></li>\n")
            .arg(folder)
            .arg(prop->filename())
            .arg(prop->name)
            .arg(prop->is_indexed ? "[<var>n</var>]" : "");
      }
      return out;
   }
}