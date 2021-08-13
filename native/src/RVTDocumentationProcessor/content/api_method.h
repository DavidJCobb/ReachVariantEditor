#pragma once
#include "base.h"
#include "page_template.h"

namespace content {
   class api_type;

   class api_method : public base {
      //
      // A member function of an APIType or APINamespace.
      //
      public:
         api_method(bool is_action) : base(is_action ? entry_type::action : entry_type::condition) {}

         struct argument {
            QString name;
            QString type; // optional
            QString content; // optional
         };
         
         QString overload_name;
         QString overload_id;
         QString bare_argument_text; // some methods use <args>some text here</args> instead of <args> <arg/> </args>
         QString return_value_type;  // optional
         QVector<argument> args;
         int32_t opcode_id = -1;
         bool    nodiscard = true;  // only relevant if (return_value_type) is not empty
         bool    is_stub   = false; // used to simplify the sidebar listing both names for an opcode with two names: we just create a dummy entry whose name is the same as the real entry's name2
         
         inline bool has_return_value() const noexcept { return !this->return_value_type.isEmpty(); }

         void load(QDomElement&, base& member_of, bool is_condition);
         QString write(QString stem, base& member_of, page_creation_options);

         QString filename() const noexcept;
         QString friendly_name(bool include_name2 = false) const noexcept;
   };
}