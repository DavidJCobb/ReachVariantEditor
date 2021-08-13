#pragma once
#include <filesystem>
#include "base.h"
#include "api_parent_object.h"

namespace content {
   class api_accessor;
   class api_method;
   class api_property;

   class api_type : public api_parent_object {
      //
      // A class which represents a type in the Megalo scripting language; it loads data from an XML 
      // file whose root element is <script-type/>.
      //
      // The class assumes that the value of the "name" attribute on that root element is the same as 
      // the filename it will be exporting to. It exports to an HTML file whose path is determined 
      // based on the (loaded_from) member. Its members will have HTML files exported to subfolders 
      // named after the type.
      //
      public:
         api_type() : api_parent_object(entry_type::type) {}

         QString friendly_name;
         bool is_variable = false;
         struct {
            bool    defined = false;
            uint8_t numbers = 0;
            uint8_t timers  = 0;
            uint8_t teams   = 0;
            uint8_t players = 0;
            uint8_t objects = 0;
         } scope; // if this type can be a variable scope, how many of each variable type can it hold?

         QString get_friendly_name() const noexcept;

         void load(QDomDocument& doc);
         void write(QString base_output_folder);
   };
}