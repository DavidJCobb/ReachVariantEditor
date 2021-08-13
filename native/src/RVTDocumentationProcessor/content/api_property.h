#pragma once
#include "base.h"
#include "page_template.h"

namespace content {
   class api_type;

   class api_property : public base {
      public:
         api_property() : base(entry_type::property) {}

         QString type;
         bool is_read_only = false;
         bool is_indexed   = false;

         void load(QDomElement&, api_type& member_of);
         QString write(QString stem, api_type& member_of, page_creation_options);

         inline QString filename() const noexcept {
            return this->name + ".html";
         }
   };
}