#include "api_type.h"
#include <QSaveFile>
#include "api_accessor.h"
#include "api_method.h"
#include "api_property.h"
#include "registry.h"
#include "../helpers/qt/save_file_to.h"
#include "../helpers/qt/xml.h"
#include "../util/html.h"
#include "../util/link_fixup.h"
#include "../util/megalo_syntax_highlight.h"

namespace content {
   QString api_type::get_friendly_name() const noexcept {
      if (!this->friendly_name.isEmpty())
         return this->friendly_name;
      if (!this->name.isEmpty())
         return this->name;
      return this->name2;
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
                  else if (name == "timers")
                     this->scope.timers  = count;
               }
            }
            this->scope.extra = util::serialize_element(node, {
               .adapt_indented_pre_tags      = true,
               .include_containing_element   = false,
               .pre_tag_content_tweak        =
                  [](QDomElement pre, QString& out) {
                     if (!pre.hasAttribute("lang"))
                        out = util::megalo_syntax_highlight(out);
                  },
               .url_tweak                    = [](QString& out) { util::link_fixup("script/api/", out);  },
               .wrap_bare_text_in_paragraphs = false,
            });
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
      for (auto node : cobb::qt::xml::const_iterable_node_list(properties.childNodes())) {
         auto elem = node.toElement();
         if (elem.isNull())
            continue;
         if (elem.nodeName() != "property")
            continue;
         auto* member = new api_property;
         member->parent = this;
         member->load(elem, *this);
         this->insert_member(*member);
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
         this->insert_member(*member);
      }

      this->sort_members();
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
               "%5 nested <a href=\"script/api/timer.html\">timers</a> available.%6%7</p>"
            )
               .arg(this->scope.numbers)
               .arg(this->scope.objects)
               .arg(this->scope.players)
               .arg(this->scope.teams)
               .arg(this->scope.timers)
               .arg(this->scope.extra.isEmpty() ? "" : " ")
               .arg(this->scope.extra);
         }
         //
         auto subfolder_path = this->relative_subfolder_path();
         //
         if (this->has_members_of_type(entry_type::property)) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::property);
            //
            body += "\n<h2>Properties</h2>\n<dl>";
            this->for_each_property([&folder, &body](auto& prop) {
               QString index;
               if (prop.is_indexed)
                  index = "[<var>n</var>]";
               //
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2%3</a></dt>\n"
                  "   <dd>%4</dd>\n"
               )
                  .arg(folder)
                  .arg(prop.name)
                  .arg(index)
                  .arg(!prop.blurb.isEmpty() ? prop.blurb : "No description available.");
            });
            body += "</dl>\n";
         }
         if (this->has_members_of_type(entry_type::accessor)) {
            auto folder = subfolder_path + sub_sub_folder_name_for(entry_type::accessor);
            //
            body += "\n<h2>Accessors</h2>\n<dl>";
            this->for_each_accessor([&folder, &body](auto& prop) {
               body += QString(
                  "<dt><a href=\"%1%2.html\">%2</a></dt>\n"
                  "   <dd>%3</dd>\n"
               )
                  .arg(folder)
                  .arg(prop.name)
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
         auto path = folder_path + "properties/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         this->for_each_property([this, &options, &path](auto& member) {
            auto filename = member.filename();
            auto content  = member.write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         });
      }
      {
         auto path = folder_path + "accessors/";
         options.relative_folder_path = QDir(base_output_folder).relativeFilePath(path);
         this->for_each_accessor([this, &options, &path](auto& member) {
            auto filename = member.filename();
            auto content  = member.write(path, *this, options);
            //
            cobb::qt::save_file_to(path + filename, content);
         });
      }
   }
}