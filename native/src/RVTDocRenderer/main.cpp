#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "helpers/strings.h"
#include "helpers/xml.h"
#include "common.h"
#include "api_entry.h"

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

const std::string& get_article_template() {
   static std::string article_template;
   if (article_template.empty()) {
      read_file(L"_article.html", article_template);
   }
   return article_template;
}
const std::string& get_type_template() {
   static std::string article_template;
   if (article_template.empty()) {
      read_file(L"_type.html", article_template);
   }
   return article_template;
}

void handle_article(std::filesystem::path xml, cobb::xml::document& doc, int depth, std::string stem) {
   std::string title;
   auto index = doc.find_element("title");
   doc.element_content_to_string(index, title);
   //
   std::string body;
   cobb::sprintf(body, "<h1>%s</h1>\n", title.c_str());
   extract_html_from_xml(doc.find_element("body"), doc, body, stem);
   //
   std::string content = get_article_template(); // copy
   std::string needle  = "<content-placeholder id=\"main\" />";
   std::size_t pos     = content.find(needle.c_str());
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   content.replace(pos, needle.length(), body);
   //
   handle_page_title_tag(content, title);
   handle_base_tag(content, depth);
   //
   xml.replace_extension(".html");
   std::ofstream file(xml.c_str());
   file.write(content.c_str(), content.size());
   file.close();
}
void handle_namespace(std::filesystem::path xml, cobb::xml::document& doc, int depth, std::string stem) {
   //
   // TODO: everything
   //
}
void handle_type(std::filesystem::path xml, cobb::xml::document& doc, int depth, std::string stem) {
   if (!doc.errors.empty()) {
      std::cout << "WARNING: " << doc.errors.size() << " errors detected.\n";
      for (auto& e : doc.errors) {
         std::cout << " - ";
         if (e.line != e.no_line_number) {
            std::cout << "Line " << e.line;
            if (e.col != e.no_col_number)
               std::cout << " col " << e.col;
            std::cout << ": ";
         }
         std::cout << e.text.c_str() << '\n';
      }
   }
   //
   APIType type;
   type.load(doc);
   type.write(xml, depth, stem, get_article_template(), get_type_template());
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
   if (strncmp(root, "script-namespace", strlen("script-namespace")) == 0) {
      handle_namespace(xml, doc, depth, stem);
   }
   if (strncmp(root, "script-type", strlen("script-type")) == 0) {
      handle_type(xml, doc, depth, stem);
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