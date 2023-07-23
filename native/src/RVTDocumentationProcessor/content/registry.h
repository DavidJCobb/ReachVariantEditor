#pragma once
#include <filesystem>
#include <vector>
#include <QString>
#include "base.h"
#include "page_template.h"

namespace content {
   class api_parent_object;
   class api_namespace;
   class api_type;
   class category;

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
         //
         QVector<category*> categories;

         struct {
            page_template article;
            page_template ns_member; // for namespace members
            page_template redirect;
            page_template type;      // for type members. NOT for the types themselves
         } page_templates;
         
         void clear();
         api_type* get_type(const QString& name);
         void load_type(const QString& relative_folder_path, QDomDocument& doc);
         void load_namespace(const QString& relative_folder_path, QDomDocument& doc);

         api_parent_object* get_namespace_or_type(const QString& name) const noexcept;
         category* get_or_create_category(const QString& id);

         void post_load_mirror_all_relationships();
      protected:
         void _mirror_relationships_for_member(base& member);
   };
}