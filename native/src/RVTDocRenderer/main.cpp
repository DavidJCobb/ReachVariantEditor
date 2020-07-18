#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "helpers/strings.h"
#include "helpers/xml.h"
#include "common.h"
#include "api_entry.h"

//
// This is a very, very quick-and-dirty tool used to export XML-formatted documentation as HTML. 
// Writing the documentation in XML and using a tool like this is faster than hand-writing all 
// of the documentation in HTML. It also allows us to shortcut some tasks. For example, the 
// page for a given Megalo function may have a "See also" section linking to other, related, 
// functions. In XML, we can define the list of related functions on just one of those functions, 
// and the code to actually load and interpret that XML can then mirror that list onto all of the 
// other functions (i.e. only A has to be told that it's related to B; the loader then tells B 
// that it's related to A).
//

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

//
// Read the contents of the HTML template files. These files contain <content-placeholder /> 
// elements that we find and replace (via a string search, not parsing) with content that we've 
// loaded from XML and formatted into HTML.
//
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
const std::string& get_ns_member_template() {
   static std::string article_template;
   if (article_template.empty()) {
      read_file(L"_ns_member.html", article_template);
   }
   return article_template;
}

void print_xml_errors(const cobb::xml::document& doc) {
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
}

void handle_article(std::filesystem::path xml, cobb::xml::document& doc) {
   auto& registry = APIRegistry::get();
   std::string stem;
   registry.make_stem(xml, stem);
   int depth = registry.depth_of(xml);
   //
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

namespace {
   void _handle_file_content(std::filesystem::path xml, cobb::xml::document& doc) {
      print_xml_errors(doc);
      auto root = doc.root_node_name();
      if (strncmp(root, "article", strlen("article")) == 0) {
         handle_article(xml, doc);
      } else if (strncmp(root, "script-namespace", strlen("script-namespace")) == 0) {
         APIRegistry::get().load_namespace(xml, doc);
      } else if (strncmp(root, "script-type", strlen("script-type")) == 0) {
         APIRegistry::get().load_type(xml, doc);
      } else if (strncmp(root, "reuse", strlen("reuse")) == 0) {
         std::string src;
         auto i = doc.find_element("reuse") + 1;
         for (; i < doc.tokens.size(); ++i) {
            auto& token = doc.tokens[i];
            if (token.code != cobb::xml::token_code::attribute)
               break;
            if (strncmp(token.name.c_str(), "src", strlen("src")) == 0) {
               src = token.value;
               break;
            }
         }
         if (src.empty()) {
            std::wcout << L"\nWARNING: <reuse/> element with no SRC in: " << xml.c_str() << L"\n";
            return;
         }
         doc.clear();
         auto target = xml;
         target.remove_filename();
         target.append(src);
         target = target.lexically_normal();
         {
            std::string raw;
            if (!read_file(target.c_str(), raw)) {
               std::wcout << L"\nWARNING: <reuse/> element with bad path in: " << xml.c_str() << L"\n   Bad path was: " << target.c_str() << "\n";
               return;
            }
            cobb::xml::parse(doc, raw);
            raw.clear();
         }
         _handle_file_content(xml, doc);
      }
   }
}
void handle_file(std::filesystem::path xml, int depth) {
   #if _DEBUG
      assert(depth == APIRegistry::get().depth_of(xml)); // depth_of correctness check; the (depth) argument is otherwise not needed since depth_of can retrieve that
   #endif
   cobb::xml::document doc;
   doc.case_insensitive = true;
   doc.define_html_entities();
   {
      std::string raw;
      read_file(xml.c_str(), raw);
      cobb::xml::parse(doc, raw);
      raw.clear();
   }
   print_xml_errors(doc);
   _handle_file_content(xml, doc);
}

int main(int argc, char* argv[]) {
   using fs_rdi = std::filesystem::recursive_directory_iterator;
   //
   auto out_path = std::filesystem::current_path();
   if (argc <= 1) {
      std::wcout << L"Path format: forward-slashes; enclosed in double-quotes. A terminating slash on the folder name is not needed.\nEnter root folder: ";
      std::cin >> out_path;
      std::wcout << L"\nConfirmed. Path in use is: " << out_path.c_str() << L"\n";
   } else {
      out_path = argv[1];
   }
   //
   auto& registry = APIRegistry::get();
   registry.root_path = out_path;
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
   for (auto* type : registry.types) {
      type->write(get_article_template(), get_type_template());
   }
   for (auto* ns : registry.namespaces) {
      ns->write(get_article_template(), get_ns_member_template());
   }
}