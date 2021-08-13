#pragma once
#include "base.h"
#include "page_template.h"

namespace content {
   class api_type;

   class api_accessor : public base {
      public:
         api_accessor() : base(entry_type::accessor) {}

         QString type;
         bool getter = false;
         bool setter = false;

         void load(QDomElement&, api_type& member_of);
         QString write(QString stem, api_type& member_of, page_creation_options);

         inline QString filename() const noexcept {
            return this->name + ".html";
         }
   };
}