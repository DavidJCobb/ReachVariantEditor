#pragma once
#include <functional>
#include "base.h"

namespace content {
   class api_accessor;
   class api_method;
   class api_namespace_member;
   class api_property;

   class api_parent_object : public base {
      public:
         using base::base;

         QString        relative_folder_path;
         QVector<base*> children;

         api_method*   get_action_by_name(const QString&) const noexcept;
         api_method*   get_condition_by_name(const QString&) const noexcept;
         api_property* get_property_by_name(const QString&) const noexcept;
         api_accessor* get_accessor_by_name(const QString&) const noexcept;
         api_namespace_member* get_generic_member_by_name(const QString&) const noexcept;

         base* lookup_nested_article(entry_type, const QString&) const noexcept;

         QString relative_subfolder_path() const noexcept;
         static QString sub_sub_folder_name_for(entry_type) noexcept;

         bool has_members_of_type(entry_type) const noexcept;

         void for_each_action(std::function<void(api_method&)>) const noexcept;
         void for_each_condition(std::function<void(api_method&)>) const noexcept;
         void for_each_accessor(std::function<void(api_accessor&)>) const noexcept;
         void for_each_property(std::function<void(api_property&)>) const noexcept;
         void for_each_generic_member(std::function<void(api_namespace_member&)>) const noexcept;

         QString members_to_nav_html(entry_type) const noexcept;
         QString accessors_to_nav_html() const noexcept;
         QString actions_to_nav_html() const noexcept;
         QString conditions_to_nav_html() const noexcept;
         QString properties_to_nav_html() const noexcept;
         QString generic_members_to_nav_html() const noexcept;

         void insert_member(base&);
         void sort_members();
   };
}