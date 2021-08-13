#pragma once
#include "base.h"
#include "page_template.h"

namespace content {
   class api_namespace;

   class api_namespace_member : public base {
      public:
         QString type;
         bool is_read_only = false;
         bool is_indexed   = false;
         bool is_none      = false;

         void load(QDomElement&, api_namespace& member_of);
         QString write(QString stem, api_namespace& member_of, page_creation_options);

         inline QString filename() const noexcept {
            return this->name + ".html";
         }
   };
}