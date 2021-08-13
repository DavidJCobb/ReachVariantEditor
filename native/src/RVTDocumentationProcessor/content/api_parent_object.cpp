#include "api_parent_object.h"
#include "api_accessor.h"
#include "api_method.h"
#include "api_namespace_member.h"
#include "api_property.h"

namespace content {
   api_method* api_parent_object::get_action_by_name(const QString& name) const noexcept {
      return (api_method*)this->lookup_nested_article(entry_type::action, name);
   }
   api_method* api_parent_object::get_condition_by_name(const QString& name) const noexcept {
      return (api_method*)this->lookup_nested_article(entry_type::condition, name);
   }
   api_property* api_parent_object::get_property_by_name(const QString& name) const noexcept {
      return (api_property*)this->lookup_nested_article(entry_type::property, name);
   }
   api_accessor* api_parent_object::get_accessor_by_name(const QString& name) const noexcept {
      return (api_accessor*)this->lookup_nested_article(entry_type::accessor, name);
   }
   api_namespace_member* api_parent_object::get_generic_member_by_name(const QString& name) const noexcept {
      return (api_namespace_member*)this->lookup_nested_article(entry_type::generic, name);
   }

   base* api_parent_object::lookup_nested_article(entry_type t, const QString& name) const noexcept {
      for (auto* child : this->children) {
         if (child->type != t)
            continue;
         if (name.compare(child->name, Qt::CaseInsensitive) == 0)
            return child;
         if (name.compare(child->name2, Qt::CaseInsensitive) == 0)
            return child;
      }
      return nullptr;
   }

   QString api_parent_object::relative_subfolder_path() const noexcept {
      return QString("%1/%3%2/")
         .arg(this->relative_folder_path)
         .arg(this->name)
         .arg(this->type == entry_type::ns ? "ns_" : "");
   }
   /*static*/ QString api_parent_object::sub_sub_folder_name_for(entry_type type) noexcept {
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

   bool api_parent_object::has_members_of_type(entry_type t) const noexcept {
      for (auto* child : this->children)
         if (child->type == t)
            return true;
      return false;
   }

   namespace {
      template<typename T> void _for_each_helper(entry_type t, const api_parent_object& self, std::function<void(T&)> functor) {
         for (auto* child : self.children) {
            if (child->type != t)
               continue;
            (functor)(*(T*)child);
         }
      }
   }
   void api_parent_object::for_each_action(std::function<void(api_method&)> functor) const noexcept {
      _for_each_helper(entry_type::action, *this, functor);
   }
   void api_parent_object::for_each_condition(std::function<void(api_method&)> functor) const noexcept {
      _for_each_helper(entry_type::condition, *this, functor);
   }
   void api_parent_object::for_each_accessor(std::function<void(api_accessor&)> functor) const noexcept {
      _for_each_helper(entry_type::accessor, *this, functor);
   }
   void api_parent_object::for_each_property(std::function<void(api_property&)> functor) const noexcept {
      _for_each_helper(entry_type::property, *this, functor);
   }
   void api_parent_object::for_each_generic_member(std::function<void(api_namespace_member&)> functor) const noexcept {
      _for_each_helper(entry_type::generic, *this, functor);
   }

   QString api_parent_object::members_to_nav_html(entry_type type) const noexcept {
      QString out;
      QString folder = QDir::cleanPath(this->relative_subfolder_path() + sub_sub_folder_name_for(type));
      for (auto* member : this->children) {
         if (member->type != type)
            continue;
         QString file;
         QString text;
         switch (type) {
            case entry_type::action:
            case entry_type::condition:
               {
                  auto* casted = (api_method*)member;
                  file = casted->filename();
                  text = casted->friendly_name(false);
                  if (casted->is_stub) {
                     auto* target = (api_method*)this->lookup_nested_article(casted->type, casted->name);
                     if (target)
                        file = target->filename();
                     else
                        continue;
                  }
               }
               break;
            case entry_type::accessor:
               {
                  auto* casted = (api_accessor*)member;
                  file = casted->filename();
                  text = casted->name;
               }
               break;
            case entry_type::property:
               {
                  auto* casted = (api_property*)member;
                  file = casted->filename();
                  text = casted->name;
                  if (casted->is_indexed)
                     text += "[<var>n</var>]";
               }
               break;
            case entry_type::generic:
               {
                  auto* casted = (api_namespace_member*)member;
                  file = casted->filename();
                  text = casted->name;
                  if (casted->is_indexed)
                     text += "[<var>n</var>]";
               }
               break;
            default:
               continue;
         }
         out += QString("<li><a href=\"%1/%2\">%3</a></li>\n")
            .arg(folder)
            .arg(file)
            .arg(text);
      }
      return out;
   }
   QString api_parent_object::accessors_to_nav_html() const noexcept {
      return this->members_to_nav_html(entry_type::accessor);
   }
   QString api_parent_object::actions_to_nav_html() const noexcept {
      return this->members_to_nav_html(entry_type::action);
   }
   QString api_parent_object::conditions_to_nav_html() const noexcept {
      return this->members_to_nav_html(entry_type::condition);
   }
   QString api_parent_object::properties_to_nav_html() const noexcept {
      return this->members_to_nav_html(entry_type::property);
   }
   QString api_parent_object::generic_members_to_nav_html() const noexcept {
      return this->members_to_nav_html(entry_type::generic);
   }

   void api_parent_object::insert_member(base& member) {
      this->children.push_back(&member);
      member.parent = this;
   }
   void api_parent_object::sort_members() {
      qSort(this->children.begin(), this->children.end(), [](base* a, base* b) {
         auto c = a->name.compare(b->name);
         if (c == 0)
            c = a->name2.compare(b->name2);
         return c < 0;
      });
   }
}