#include "page_template.h"
#include "api_method.h"
#include "api_namespace.h"
#include "api_type.h"
#include "../util/html.h"
#include "../util/strip_html.h"
#include <array>
#include <cassert>

namespace {
   static QString base_tag = "<base href=\"%1\" />";
}

namespace content {
   void page_template::setContent(const QString& source) {
      this->html = source;
      //
      {
         QString needle = "<title>";
         this->tokens.title.at = source.indexOf("<title>", Qt::CaseInsensitive);
         if (this->tokens.title.at >= 0)
            this->tokens.title.at += needle.size();
      }
      {
         QString needle = "<content-placeholder id=\"main\" />";
         this->tokens.body.at = source.indexOf(needle);
         if (this->tokens.body.at >= 0) {
            this->tokens.body.length = needle.size();
         }
      }
      {
         QString needle = "<base />";
         this->tokens.base_href.at = source.indexOf(needle);
         if (this->tokens.base_href.at >= 0)
            this->tokens.base_href.length = needle.size();
      }
      {
         auto&   token  = this->tokens.nav_accessors;
         QString needle = "<content-placeholder id=\"nav-accessors\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
      {
         auto&   token  = this->tokens.nav_actions;
         QString needle = "<content-placeholder id=\"nav-actions\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
      {
         auto&   token  = this->tokens.nav_conditions;
         QString needle = "<content-placeholder id=\"nav-conditions\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
      {
         auto& token = this->tokens.nav_members;
         QString needle = "<content-placeholder id=\"nav-members\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
      {
         auto&   token  = this->tokens.nav_properties;
         QString needle = "<content-placeholder id=\"nav-properties\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
      {
         auto&   token  = this->tokens.nav_type_link;
         QString needle = "<content-placeholder id=\"nav-self-link\" />";
         token.at = source.indexOf(needle);
         if (token.at >= 0)
            token.length = needle.size();
      }
   }

   QString page_template::createPage(const QString& raw_title, const QString& body, const QString& base_href) const noexcept {
      assert(this->tokens.body.at >= 0);
      //
      QString title = util::strip_html(raw_title);
      QString out;
      {
         int size = this->html.size();
         size += title.size();
         size += body.size()      - this->tokens.body.length;
         size += base_href.size() - this->tokens.base_href.length + base_tag.size() - 2;
         size += 4096; // blind estimate for the navigation sidebar edits
         //
         out.reserve(size);
      }
      std::array list = { this->tokens.base_href, this->tokens.body, this->tokens.title };
      std::sort(list.begin(), list.end());
      //
      int from = 0;
      for (auto& token : list) {
         out += QStringRef(&this->html, from, token.at - from);
         from = token.at + token.length;
         //
         switch (token.type) {
            case token_type::base_href:
               out += base_tag.arg(base_href);
               break;
            case token_type::body:
               out += body;
               break;
            case token_type::title:
               out += title;
               break;
         }
      }
      auto size = this->html.size();
      if (from < size)
         out += QStringRef(&this->html, from, size - from);
      //
      return out;
   }
   QString page_template::createPage(const QString& title, const QString& body, int base_depth) const noexcept {
      QString base_href;
      while (base_depth-- > 0) {
         base_href += "../";
      }
      return this->createPage(title, body, base_href);
   }

   QString page_template::create_page(page_creation_options options) const noexcept {
      assert(this->tokens.body.at >= 0);
      if (options.base_href.isEmpty()) {
         if (options.base_depth > 0) {
            while (options.base_depth-- > 0) {
               options.base_href += "../";
            }
         } else if (!options.relative_folder_path.isEmpty()) {
            //options.base_href = QDir(options.relative_folder_path).relativeFilePath("/");
            //
            // The above seems to actually resolve the relative path relative to the CWD, which is not what we want.
            // Given a relative path like "foo/bar/baz" we want "../../../" as our result here.
            //
            options.base_href = QDir(QDir("/").absoluteFilePath(options.relative_folder_path)).relativeFilePath("/");
         }
      }
      options.title = util::strip_html(options.title);
      //
      QString out;
      {
         int size = this->html.size();
         size += options.title.size();
         size += options.body.size()      - this->tokens.body.length;
         size += options.base_href.size() - this->tokens.base_href.length + base_tag.size() - 2;
         size += 4096; // blind estimate for the navigation sidebar edits
         //
         out.reserve(size);
      }
      std::array list = {
         this->tokens.base_href,
         this->tokens.body,
         this->tokens.title,
         this->tokens.nav_accessors,
         this->tokens.nav_actions,
         this->tokens.nav_conditions,
         this->tokens.nav_members,
         this->tokens.nav_properties,
         this->tokens.nav_type_link,
      };
      std::sort(list.begin(), list.end());
      //
      int from = 0;
      for (auto& token : list) {
         if (token.at < 0)
            continue;
         out += QStringRef(&this->html, from, token.at - from);
         from = token.at + token.length;
         //
         switch (token.type) {
            case token_type::base_href:
               if (options.base_href.isEmpty())
                  out += "<base />";
               else
                  out += base_tag.arg(options.base_href);
               break;
            case token_type::body:
               out += options.body;
               break;
            case token_type::title:
               out += options.title;
               break;
            case token_type::nav_accessors:
               out += options.nav.accessors;
               break;
            case token_type::nav_actions:
               out += options.nav.actions;
               break;
            case token_type::nav_conditions:
               out += options.nav.conditions;
               break;
            case token_type::nav_members:
               out += options.nav.members;
               break;
            case token_type::nav_properties:
               out += options.nav.properties;
               break;
            case token_type::nav_type_link:
               out += options.nav.member_of_link;
               break;
         }
      }
      auto size = this->html.size();
      if (from < size)
         out += QStringRef(&this->html, from, size - from);
      //
      return out;
   }
}