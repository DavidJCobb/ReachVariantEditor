#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "helpers/strings.h"
#include "helpers/xml.h"

bool read_file(const wchar_t* path, std::string& out) {
   std::ifstream in(path, std::ios::in);
   if (!in)
      return false;
   constexpr size_t ce_read_size = 4096;
   auto buf = std::string(ce_read_size, '\0');
   while (in.read(&buf[0], ce_read_size)) {
      out.append(buf, 0, in.gcount());
   }
   out.append(buf, 0, in.gcount());
   //
   return true;
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

void extract_html_from_xml(uint32_t token_index, cobb::xml::document& doc, std::string& out, std::string stem) {
   using namespace cobb::xml;
   //
   std::string name;
   std::string last_opened_tag;
   uint32_t    nesting = 1;
   bool        is_in_open_tag = false;
   //
   name = doc.tokens[token_index].name;
   //
   for (size_t i = token_index + 1; i < doc.tokens.size(); ++i) {
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
            out += token.value;
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
}

void handle_article(std::filesystem::path xml, cobb::xml::document& doc, int depth, std::string stem) {
   static std::string article_template;
   if (article_template.empty()) {
      read_file(L"_article.html", article_template);
   }
   //
   std::string title;
   auto index = doc.find_element("title");
   doc.element_content_to_string(index, title);
   //
   std::string body;
   cobb::sprintf(body, "<h1>%s</h1>\n", title.c_str());
   extract_html_from_xml(doc.find_element("body"), doc, body, stem);
   //
   std::string content = article_template; // copy
   std::string needle  = "<content-placeholder id=\"main\" />";
   std::size_t pos     = content.find(needle.c_str());
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   content.replace(pos, needle.length(), body);
   //
   needle  = "<title>";
   pos     = content.find(needle.c_str());
   std::string replace;
   cobb::sprintf(replace, "<title>%s", title.c_str());
   content.replace(pos, needle.length(), replace);
   //
   if (depth > 0) {
      needle = "<base />";
      pos    = content.find(needle.c_str());
      std::string replace = "<base href=\"";
      do {
         replace += "../";
      } while (--depth);
      replace += "\" />";
      content.replace(pos, needle.length(), replace);
   }
   //
   xml.replace_extension(".html");
   std::ofstream file(xml.c_str());
   file.write(content.c_str(), content.size());
   file.close();
}

void handle_file(std::filesystem::path xml, int depth) {
   std::string stem;
   {
      auto path = xml;
      for (auto temp = depth; temp; --temp) {
         path = path.parent_path();
         stem = path.filename().u8string() + '/' + stem;
      }
   }
   //
   cobb::xml::document doc;
   doc.case_insensitive = true;
   doc.define_html_entities();
   {
      std::string raw;
      read_file(xml.c_str(), raw);
      cobb::xml::parse(doc, raw);
      raw.clear();
   }
   auto root = doc.root_node_name();
   if (strncmp(root, "article", strlen("article")) == 0) {
      handle_article(xml, doc, depth, stem);
   }
}

int main(int argc, char* argv[]) {
   using fs_rdi = std::filesystem::recursive_directory_iterator;
   //
   auto out_path = std::filesystem::current_path();
   if (argc <= 1) {
      std::wcout << L"Path format: forward-slashes; enclosed in double-quotes.\nEnter root folder: ";
      std::cin >> out_path;
      std::wcout << L"\nConfirmed. Path in use is: " << out_path.c_str() << L"\n";
   } else {
      out_path = argv[1];
   }
   //
   std::error_code err;
   for (auto di = fs_rdi(out_path, err); di != fs_rdi(); ++di) {
      if (err) {
         std::wcout << L"bad directory.\n";
         return 1;
      }
      int   depth = di.depth();
      auto& entry = *di; // a folder or a file
      if (entry.is_regular_file()) {
         auto& path = entry.path();
         if (path.extension() == ".xml") {
            auto temp = path;
            temp.replace_extension(".html");
            std::wcout << L"File: " << temp.c_str() << L"\n";
            //
            handle_file(path, depth);
         }
      }
   }
}