#include "api_type.h"
#include <QSaveFile>
#include "api_accessor.h"
#include "api_method.h"
#include "api_property.h"
#include "registry.h"
#include "../helpers/qt/save_file_to.h"
#include "../helpers/qt/xml.h"

namespace content {
   QString api_type::get_friendly_name() const noexcept {
      if (!this->friendly_name.isEmpty())
         return this->friendly_name;
      if (!this->name.isEmpty())
         return this->name;
      return this->name2;
   }
   api_method* api_type::get_action_by_name(const QString& name) const noexcept {
      for (auto& member : this->actions) {
         if (member->is_stub)
            continue;
         if (member->name == name || member->name2 == name)
            return member;
      }
      return nullptr;
   }
   api_method* api_type::get_condition_by_name(const QString& name) const noexcept {
      for (auto& member : this->conditions) {
         if (member->is_stub)
            continue;
         if (member->name == name || member->name2 == name)
            return member;
      }
      return nullptr;
   }
   api_property* api_type::get_property_by_name(const QString& name) const noexcept {
      for (auto& member : this->properties)
         if (member->name == name || member->name2 == name)
            return member;
      return nullptr;
   }
   api_accessor* api_type::get_accessor_by_name(const QString& name) const noexcept {
      for (auto& member : this->accessors)
         if (member->name == name || member->name2 == name)
            return member;
      return nullptr;
   }

   QString api_type::relative_subfolder_path() const noexcept {
      return QString("%1/%2/")
         .arg(this->relative_folder_path)
         .arg(this->name);
   }
   /*static*/ QString api_type::sub_sub_folder_name_for(entry_type type) noexcept {
      switch (type) {
         case entry_type::action:
            return "actions/";
         case entry_type::condition:
            return "conditions/";
         case entry_type::property:
            return "properties/";
         case entry_type::accessor:
            return "accessors/";
      }
      return "";
   }
   
   void api_type::load(QDomDocument& doc) {
      auto root = doc.documentElement();
      assert(root.nodeName().compare("script-type", Qt::CaseSensitive) == 0);
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
      {
         auto node = cobb::qt::xml::first_child_element_of_type(root, "scope");
         if (!node.isNull()) {
            auto list = node.attributes();
            auto size = list.size();
            for (int i = 0; i < size; ++i) {
               bool ok;
               //
               auto attr = list.item(i).toAttr();
               assert(!attr.isNull());
               auto name  = attr.name();
               auto count = attr.value().toInt(&ok);
               if (ok) {
                  this->scope.defined = true;
                  if (name == "numbers")
                     this->scope.numbers = count;
                  else if (name == "objects")
                     this->scope.objects = count;
                  else if (name == "players")
                     this->scope.players = count;
                  else if (name == "teams")
                     this->scope.teams   = count;
                  else if (name == "tcountmers")
                     this->scope.timers  = count;
               }
            }
         }
      }
      //
      auto methods    = cobb::qt::xml::first_child_element_of_type(root, "methods");
      auto properties = cobb::qt::xml::first_child_element_of_type(root, "properties");
      auto accessors  = cobb::qt::xml::first_child_element_of_type(root, "accessors");
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
      for (auto node : cobb::qt::xml::const_iterable_node_list(properties.childNodes())) {
         auto elem = node.toElement();
         if (elem.isNull())
            continue;
         if (elem.nodeName() != "property")
            continue;
         auto* member = new api_property;
         member->parent = this;
         member->load(elem, *this);
         this->properties.push_back(member);
      }
      for (auto node : cobb::qt::xml::const_iterable_node_list(accessors.childNodes())) {
         auto elem = node.toElement();
         if (elem.isNull())
            continue;
         if (elem.nodeName() != "accessor")
            continue;
         auto* member = new api_accessor;
         member->parent = this;
         member->load(elem, *this);
         this->accessors.push_back(member);
      }

      std::sort(this->actions.begin(),    this->actions.end(),    [](const api_method* a, const api_method* b) { return a->name < b->name; });
      std::sort(this->conditions.begin(), this->conditions.end(), [](const api_method* a, const api_method* b) { return a->name < b->name; });
      std::sort(this->properties.begin(), this->properties.end(), [](const base* a, const base* b) { return a->name < b->name; });
      std::sort(this->accessors.begin(),  this->accessors.end(),  [](const base* a, const base* b) { return a->name < b->name; });
      
      this->_make_member_relationships_bidirectional();
   }

   void api_type::_mirror_member_relationships(api_type& member_of, base& member, entry_type member_type) {
      /*//
      for (auto& rel : member.related) {
         if (rel.mirrored)
            continue;
         if (!rel.context.isEmpty() && rel.context != member_of.name) // this is a relationship to something in another type
            continue;
         base* target = nullptr;
         //
         auto rt = rel.type;
         if (rt == entry_type::same)
            rt = member_type;
         switch (rt) {
            case entry_type::condition:
               target = member_of.get_condition_by_name(rel.name);
               break;
            case entry_type::action:
               target = member_of.get_action_by_name(rel.name);
               break;
            case entry_type::property:
               target = member_of.get_property_by_name(rel.name);
               break;
            case entry_type::accessor:
               target = member_of.get_accessor_by_name(rel.name);
               break;
         }
         //
         if (target == nullptr)
            continue;
         rel.name = target->name; // if we matched name2, then we need to set this here so we write the proper name into HTML
         target->related.emplace_back();
         auto& mirrored = target->related.back();
         mirrored.mirrored = true;
         mirrored.name = member.name;
         mirrored.type = member_type;
         //
         for (auto& rel2 : member.related) {
            if (&rel2 == &rel)
               continue;
            target->related.emplace_back();
            auto& mirrored = target->related.back();
            mirrored = rel2;
            mirrored.mirrored = true;
         }
      }
      //*/
   }
   void api_type::_make_member_relationships_bidirectional() {
      for (auto* member : this->conditions)
         this->_mirror_member_relationships(*this, *member, entry_type::condition);
      for (auto* member : this->actions)
         this->_mirror_member_relationships(*this, *member, entry_type::action);
      for (auto* member : this->properties)
         this->_mirror_member_relationships(*this, *member, entry_type::property);
      for (auto* member : this->accessors)
         this->_mirror_member_relationships(*this, *member, entry_type::accessor);
   }

   void api_type::write(QString base_output_folder) {
      auto& registry = registry::get();
      //
      // Write the type's own page:
      //
      {
         QString body = QString("<h1>%1</h1>\n").arg(this->get_friendly_name());
         body += this->description_to_html();
         if (this->scope.defined) {
            body += QString(
               "<p>This type contains nested variables. There are "
               "%1 nested <a href=\"script/api/number.html\">numbers</a>, "
               "%2 nested <a href=\"script/api/object.html\">objects</a>, "
               "%3 nested <a href=\"script/api/player.html\">players</a>, "
               "%4 nested <a href=\"script/api/team.html\">teams</a>, and "
               "%5 nested <a href=\"script/api/timer.html\">timers</a> available.</p>"
            )
               .arg(this->scope.numbers)
               .arg(this->scope.objects)
               .arg(this->scope.players)
               .arg(this->scope.teams)
               .arg(this->scope.timers);
         }
         //
         auto subfolder_path = this->relative_subfolder_path();
         //
         if (this->properties.size()) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::property);
            //
            body += "\n<h2>Properties</h2>\n<dl>";
            for (auto* prop : this->properties) {
               QString index;
               if (prop->is_indexed)
                  index = "[<var>n</var>]";
               //
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop->name)
                  .arg(index)
                  .arg(!prop->blurb.isEmpty() ? prop->blurb : "No description available.");
            }
            body += "</dl>\n";
         }
         if (this->accessors.size()) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::accessor);
            //
            body += "\n<h2>Accessors</h2>\n<dl>";
            for (auto* prop : this->accessors) {
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2</a></dt>\n"
                  "   <dd>%3</dd>\n"
               )
                  .arg(folder)
                  .arg(prop->name)
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
         path += this->name;
         path += ".html";
         //
         cobb::qt::save_file_to(path, body);
      }
      //
      // Now prepare to generate the pages for members:
      //
      QString folder_path = QDir(base_output_folder).absoluteFilePath(this->relative_subfolder_path());
      //
      auto options = page_creation_options{
         .nav = {
            .accessors  = this->accessors_to_nav_html(),
            .actions    = this->actions_to_nav_html(),
            .conditions = this->conditions_to_nav_html(),
            .properties = this->properties_to_nav_html(),
            .member_of_link = QString("<a href=\"%1/%2.html\">%3</a>").arg(this->relative_folder_path).arg(this->name).arg(this->get_friendly_name()),
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
         auto path = folder_path + "properties/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         for (auto* member : this->properties) {
            auto filename = member->filename();
            auto content  = member->write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         }
      }
      {
         auto path = folder_path + "accessors/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         for (auto* member : this->accessors) {
            auto filename = member->filename();
            auto content  = member->write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         }
      }
   }

   QString api_type::accessors_to_nav_html() const noexcept {
      QString out;
      QString folder = this->relative_subfolder_path() + sub_sub_folder_name_for(entry_type::accessor);
      for (auto* prop : this->accessors) {
         out += QString("<li><a href=\"%1%2\">%3</a></li>\n")
            .arg(folder)
            .arg(prop->filename())
            .arg(prop->name);
      }
      return out;
   }
   QString api_type::actions_to_nav_html() const noexcept {
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
   QString api_type::conditions_to_nav_html() const noexcept {
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
   QString api_type::properties_to_nav_html() const noexcept {
      QString out;
      QString folder = this->relative_subfolder_path() + sub_sub_folder_name_for(entry_type::property);
      for (auto* prop : this->properties) {
         out += QString("<li><a href=\"%1%2\">%3%4</a></li>\n")
            .arg(folder)
            .arg(prop->filename())
            .arg(prop->name)
            .arg(prop->is_indexed ? "[<var>n</var>]" : "");
      }
      return out;
   }
}