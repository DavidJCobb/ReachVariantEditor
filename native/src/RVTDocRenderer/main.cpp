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
   in.seekg(0, std::ios::end);
   out.resize(in.tellg());
   in.seekg(0, std::ios::beg);
   in.read(&out[0], out.size());
   in.close();
   return true;
}

void handle_article(std::filesystem::path xml, cobb::xml::document& doc, int depth) {
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
   index = doc.find_element("body");
   doc.element_content_to_string(index, body);
   cobb::sprintf(body, "<h1>%s</h1>\n%s", title.c_str(), body.c_str());
   //
   // TODO: custom function to collect article body into a string, so we can fix up "href" and 
   // "src" attributes. They're written as relative to the XML file itself, but we use <base/> 
   // so that we can use the same code for the CSS, JS, nav, etc., so we need to match attribs 
   // in the body accordingly.


   //
   std::string content = article_template; // copy
   std::string needle  = "<content-placeholder id=\"main\" />";
   std::size_t pos     = content.find(needle.c_str());
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   content.replace(pos, needle.length(), body);

   //
   // TODO: for some reason we sure are inserting a ton of \0s at the end of the file and it'd 
   // be real swell if we weren't doing that
   //

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
   cobb::xml::document doc;
   doc.case_insensitive = true;
   {
      std::string raw;
      read_file(xml.c_str(), raw);
      cobb::xml::parse(doc, raw);
      raw.clear();
   }
   auto root = doc.root_node_name();
   if (strncmp(root, "article", strlen("article")) == 0) {
      handle_article(xml, doc, depth);
   }
}

int main(int argc, char* argv[]) {
   using fs_rdi = std::filesystem::recursive_directory_iterator;
   //
   auto out_path = std::filesystem::current_path();
   if (argc <= 1) {
      std::wcout << L"Enter root folder: ";
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