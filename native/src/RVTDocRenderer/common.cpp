#include "common.h"
#include <cassert>
#include "helpers/strings.h"

namespace {
   void _append_text_content(std::string& out, const std::string& text) {
      size_t size = text.size();
      out.reserve(out.size() + size);
      for (size_t i = 0; i < size; ++i) {
         char c = text[i];
         switch (c) {
            case '&': out += "&amp;"; continue;
            case '<': out += "&lt;"; continue;
            case '>': out += "&gt;"; continue;
         }
         out += c;
      }
   }
}

void handle_base_tag(std::string& html, int depth) {
   if (depth <= 0)
      return;
   std::string needle  = "<base />";
   std::size_t pos     = html.find(needle.c_str());
   assert(pos != std::string::npos);
   std::string replace = "<base href=\"";
   do {
      replace += "../";
   } while (--depth);
   replace += "\" />";
   html.replace(pos, needle.length(), replace);
}
void handle_page_title_tag(std::string& html, std::string title) {
   std::string needle = "<title>";
   std::size_t pos    = html.find(needle.c_str());
   if (pos != std::string::npos) {
      std::string replace;
      cobb::sprintf(replace, "<title>%s", title.c_str());
      html.replace(pos, needle.length(), replace);
   }
}

void minimize_indent(std::string& text) {
   uint32_t lowest  = std::numeric_limits<uint32_t>::max();
   uint32_t current = 0;
   //
   for (size_t i = 0; i < text.size(); ++i) {
      auto c = text[i];
      if (c == '\r')
         continue;
      if (c == '\n') {
         current = 0;
         continue;
      }
      if (!isspace(c)) {
         if (current < lowest)
            lowest = current;
         continue;
      }
      ++current;
   }
   //
   std::string replace = "";
   current = 0;
   bool is_line_start = true;
   for (size_t i = 0; i < text.size(); ++i) {
      auto c = text[i];
      if (c == '\r' || c == '\n' || !isspace(c) || !is_line_start) {
         replace += c;
         current  = 0;
         if (!isspace(c))
            is_line_start = false;
         else if (c == '\r' || c == '\n')
            is_line_start = true;
         continue;
      }
      ++current;
      if (current > lowest)
         replace += c;
   }
   cobb::rtrim(replace);
   text = replace;
}

size_t extract_html_from_xml(uint32_t token_index, cobb::xml::document& doc, std::string& out, std::string stem) {
   using namespace cobb::xml;
   //
   std::string name = doc.tokens[token_index].name;
   std::string last_opened_tag;
   uint32_t    nesting = 1;
   bool        is_in_open_tag = false;
   //
   size_t i = token_index + 1;
   for (; i < doc.tokens.size(); ++i) { // advance past the opening tag (it may have attributes)
      auto& token = doc.tokens[i];
      if (token.code != token_code::attribute)
         break;
   }
   for (; i < doc.tokens.size(); ++i) {
      auto&       token = doc.tokens[i];
      std::string temp;
      if (token.code != token_code::attribute && is_in_open_tag) {
         out += '>';
         is_in_open_tag = false;
      }
      switch (token.code) {
         case token_code::element_open:
            cobb::sprintf(temp, "<%s", token.name.c_str());
            is_in_open_tag = true;
            if (token.name == name)
               ++nesting;
            last_opened_tag = token.name;
            break;
         case token_code::attribute:
            if (token.name == "href" || token.name == "src") {
               //
               // Really hacky link/image fixup code:
               //
               if (token.value.compare(0, 7, "script/") != 0) {
                  //
                  // Articles typically use hyperlink and image paths that are relative to themselves, so 
                  // we need to fix those up since we're using a <base/> element to change relative paths 
                  // (so that the nav and asset paths can work). However, API documentation sometimes has 
                  // what are, in effect, absolute paths that we want to ignore.
                  //
                  token.value = stem + token.value;
               }
               //
               // Links between articles usually lack file extensions.
               //
               auto a = token.value.find_last_of(".");
               auto b = token.value.find_last_of("/");
               if (a == std::string::npos || a < b) {
                  token.value += ".html";
               }
            }
            cobb::sprintf(temp, " %s=\"%s\"", token.name.c_str(), token.value.c_str());
            break;
         case token_code::text_content:
            if (last_opened_tag == "pre") {
               //
               // De-indent the tag.
               //
               minimize_indent(token.value);
            }
            _append_text_content(out, token.value);
            break;
         case token_code::element_close:
            if (token.name == name) {
               --nesting;
               if (!nesting)
                  break;
            }
            last_opened_tag.clear();
            cobb::sprintf(temp, "</%s>", token.name.c_str());
            break;
      }
      out += temp;
      if (nesting == 0)
         break;
   }
   if (nesting != 0)
      return std::string::npos;
   return i;
}