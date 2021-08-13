#pragma once
#include <filesystem>
#include "base.h"

namespace content {
   class api_accessor;
   class api_method;
   class api_property;

   class api_type : public base {
      //
      // A class which represents a type in the Megalo scripting language; it loads data from an XML 
      // file whose root element is <script-type/>.
      //
      // The class assumes that the value of the "name" attribute on that root element is the same as 
      // the filename it will be exporting to. It exports to an HTML file whose path is determined 
      // based on the (loaded_from) member. Its members will have HTML files exported to subfolders 
      // named after the type.
      //
      protected:
         void _make_member_relationships_bidirectional();
         void _mirror_member_relationships(api_type& member_of, base& member, entry_type member_type);
         
      public:
         api_type() : base(entry_type::type) {}

         QString relative_folder_path;
         QString friendly_name;
         QVector<api_method*>   conditions;
         QVector<api_method*>   actions;
         QVector<api_property*> properties;
         QVector<api_accessor*> accessors;
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
         api_method* get_action_by_name(const QString&) const noexcept;
         api_method* get_condition_by_name(const QString&) const noexcept;
         api_property* get_property_by_name(const QString&) const noexcept;
         api_accessor* get_accessor_by_name(const QString&) const noexcept;

         QString relative_subfolder_path() const noexcept;
         static QString sub_sub_folder_name_for(entry_type) noexcept;

         void load(QDomDocument& doc);
         void write(QString base_output_folder);

         QString accessors_to_nav_html() const noexcept;
         QString actions_to_nav_html() const noexcept;
         QString conditions_to_nav_html() const noexcept;
         QString properties_to_nav_html() const noexcept;
   };
}