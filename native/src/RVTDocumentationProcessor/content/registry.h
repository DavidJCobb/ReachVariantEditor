#pragma once
#include <filesystem>
#include <vector>
#include <QString>
#include "base.h"
#include "page_template.h"

namespace content {
   class api_namespace;
   class api_type;

   class registry {
      //
      // Used to load and retain all types and namespaces. Instead of exporting them to HTML one by one, we want 
      // to retain them all and then export them all at once, because some pages may refer to data in other pages. 
      // The main case where this occurs is an APIMethod that has an argument with no description: we would look 
      // at that argument's type, pull the blurb from that APIType, and use that blurb as the description -- which 
      // of course means that that APIType needs to be in memory at the time we export the APIMethod as HTML.
      //
      private:
         registry();
      public:
         static registry& get() {
            static registry instance;
            return instance;
         }
         ~registry();
         
         std::filesystem::path root_path;
         std::vector<api_type*> types;
         std::vector<api_namespace*> namespaces;

         struct {
            page_template article;
            page_template ns_member; // for namespace members
            page_template type;      // for type members. NOT for the types themselves
         } page_templates;
         
         void clear();
         api_type* get_type(const QString& name);
         void load_type(const QString& relative_folder_path, QDomDocument& doc);
         void load_namespace(const QString& relative_folder_path, QDomDocument& doc);
         
         int  depth_of(std::filesystem::path path);
         QString make_stem(std::filesystem::path path);
         QString make_stem(int);

         void post_load_mirror_all_relationships();
      protected:
         void _mirror_relationships_for_member(api_namespace& type, base& member, entry_type member_type);
         void _mirror_relationships_for_member(api_type& type, base& member, entry_type member_type);
   };
}