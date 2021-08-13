#pragma once
#include <functional>
#include <QString>

namespace content {
   class api_namespace;
   class api_type;

   struct page_creation_options {
      int     base_depth = -1;
      QString base_href;
      QString body;
      QString relative_folder_path;
      QString title;
      //
      struct {
         QString accessors;
         QString actions;
         QString conditions;
         QString members;
         QString properties;
         QString member_of_link;
      } nav;
   };

   class page_template {
      public:
         using size_type = int;
      protected:
         enum class token_type {
            base_href,
            body,
            nav_accessors,
            nav_actions,
            nav_conditions,
            nav_members,
            nav_properties,
            nav_type_link,
            title,
         };
         struct token {
            token_type type;
            size_type  at     = -1;
            size_type  length =  0;

            bool operator<(const token& other) const noexcept { return this->at < other.at; }
         };

         struct {
            token base_href      = { token_type::base_href };
            token body           = { token_type::body };
            token title          = { token_type::title };
            token nav_accessors  = { token_type::nav_accessors };
            token nav_actions    = { token_type::nav_actions };
            token nav_conditions = { token_type::nav_conditions };
            token nav_members    = { token_type::nav_members };
            token nav_properties = { token_type::nav_properties };
            token nav_type_link  = { token_type::nav_type_link };
         } tokens;

      public:
         QString html;

         void setContent(const QString&);

         QString create_page(page_creation_options) const noexcept;

         QString createPage(const QString& title, const QString& body, const QString& base_href = QString()) const noexcept;
         QString createPage(const QString& title, const QString& body, int base_depth = 0) const noexcept;
   };
}