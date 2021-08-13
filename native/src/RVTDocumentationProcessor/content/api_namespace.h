#pragma once
#include <filesystem>
#include "base.h"
#include "api_parent_object.h"
#include "page_template.h"
#include "api_method.h"
#include "api_namespace_member.h"

namespace content {
   class api_namespace : public api_parent_object {
      public:
         api_namespace() : api_parent_object(entry_type::ns) {}

         QString friendly_name;

         QString get_friendly_name() const noexcept;

         void load(QDomDocument&);
         void write(QString base_output_folder);
   };
}