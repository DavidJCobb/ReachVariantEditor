#pragma once
#include <filesystem>
#include "base.h"
#include "page_template.h"
#include "api_method.h"
#include "api_namespace_member.h"

namespace content {
   class api_namespace : public base {
      public:
         api_namespace() : base(entry_type::ns) {}

         QString relative_folder_path;
         QString friendly_name;
         QVector<api_method*>   conditions;
         QVector<api_method*>   actions;
         QVector<api_namespace_member*> members;

         QString get_friendly_name() const noexcept;
         api_method* get_action_by_name(const QString&) const noexcept;
         api_method* get_condition_by_name(const QString&) const noexcept;
         api_namespace_member* get_member_by_name(const QString&) const noexcept;

         QString relative_subfolder_path() const noexcept;
         static QString sub_sub_folder_name_for(entry_type) noexcept;

         void load(QDomDocument&);
         void write(QString base_output_folder);

         QString actions_to_nav_html() const noexcept;
         QString conditions_to_nav_html() const noexcept;
         QString members_to_nav_html() const noexcept;
   };
}