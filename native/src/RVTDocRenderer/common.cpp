#include "common.h"
#include <cassert>
#include "helpers/strings.h"

namespace {
   void _append_text_content(std::string& out, const std::string& text) {
      //
      // Appends the content of (text) to (out), substituting HTML entities where appropriate.
      //
      size_t size = text.size();
      out.reserve(out.size() + size);
      //
      bool in_entity = false;
      std::string entity;
      //
      for (size_t i = 0; i < size; ++i) {
         char c = text[i];
         if (in_entity) {
            //
            // We previously found a '&' which may or may not be an HTML entity.
            //
            if (isspace(c) || c == '&') { // turns out, this wasn't an HTML entity after all
               out += "&amp;";
               out += entity;
               if (c != '&') {
                  in_entity = false;
                  out += c;
               }
               entity.clear();
               continue;
            }
            if (c == ';') { // turns out, this was an HTML entity after all
               out += '&';
               out += entity;
               out += c;
               in_entity = false;
               entity.clear();
               continue;
            }
            entity += c;
            continue;
         }
         switch (c) {
            case '&':
               in_entity = true;
               continue;
            case '<': out += "&lt;"; continue;
            case '>': out += "&gt;"; continue;
         }
         out += c;
      }
      if (in_entity) {
         out += "&amp;";
         out += entity;
      }
   }
}

void ensure_paragraph(std::string& text) {
   size_t start = 0;
   size_t size  = text.size();
   for (; start < size; ++start)
      if (!isspace(text[start]))
         break;
   if (text[start] == '<') // text likely already starts with a tag
      return;
   text.insert(0, "<p>");
   text += "</p>";
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
      std::string replace = "<title>";
      replace.reserve(8 + title.size());
      //
      bool in_tag = false;
      for (size_t i = 0; i < title.size(); ++i) {
         char c = title[i];
         if (in_tag) {
            if (c == '>')
               in_tag = false;
            continue;
         }
         if (c == '<') {
            in_tag = true;
            continue;
         }
         replace += c;
      }
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

namespace {
   std::string _keywords[] = {
      "alias",
      "alt",
      "altif",
      "and",
      "declare",
      "do",
      "end",
      "for",
      "function",
      "if",
      "not",
      "on",
      "or",
      "then",
   };
   bool _is_operator_char(char c) {
      return strchr("=<>!+-*/%&|~^", c) != nullptr;
   }
   bool _is_quote_char(char c) {
      return strchr("`'\"", c) != nullptr;
   }
   bool _is_syntax_char(char c) {
      return strchr("(),:", c) != nullptr;
   }
}
void syntax_highlight_in_html(const std::string& in, std::string& out) {
   out.clear();
   size_t size = in.size();
   out.reserve(size);
   //
   std::string in_keyword;
   size_t      keyword_done = 0;
   std::string last_keyword; // TODO: use teal for words "attached" to a keyword, e.g. "[blue]for[/blue] [teal]each object with label[/teal]" or "[blue]declare[/blue] varname [teal]with network priority low[/teal]"
   bool        in_comment   = false;
   char        in_string    = '\0';
   for (size_t i = 0; i < size; ++i) {
      char c = in[i];
      if (in_string) {
         if (c == in_string) {
            in_string = '\0';
            out += "</span>";
         }
         out += c;
         continue;
      }
      if (in_comment) {
         if (c == '\r' || c == '\n') {
            in_comment = false;
            out += "</span>";
         }
         out += c;
         continue;
      }
      if (_is_quote_char(c)) {
         in_string = c;
         out += "<span class=\"string\">";
      }
      if (c == '-' && i + 1 < size) {
         char d = in[i + 1];
         if (d == '-') {
            in_comment = true;
            out += "<span class=\"comment\">";
         }
      }
      //
      if (in_keyword.empty()) {
         bool can_start = i == 0;
         if (i > 0)
            can_start = isspace(in[i - 1]);
         //
         if (can_start) {
            for (auto& k : _keywords) {
               if (strncmp(in.data() + i, k.data(), k.size()) == 0) {
                  in_keyword = k;
                  keyword_done = 0;
                  break;
               }
            }
            if (!in_keyword.empty()) {
               bool ends = false;
               if (i + in_keyword.size() == in.size())
                  ends = true;
               else if (i + in_keyword.size() < in.size()) {
                  char d = in[i + in_keyword.size()];
                  ends = _is_quote_char(d) || _is_syntax_char(d) || isspace(d) || _is_operator_char(d);
               }
               if (ends)
                  out += "<span class=\"keyword\">";
               else
                  in_keyword.clear();
            }
         }
         if (in_keyword.empty())
            last_keyword.clear();
      } else if (++keyword_done == in_keyword.size()) {
         last_keyword.clear();
         std::swap(last_keyword, in_keyword);
         out += "</span>";
      }
      //
      out += c;
   }
   if (!in_keyword.empty() || in_comment || in_string) {
      out += "</span>";
   }
}
void syntax_highlight_in_html(std::string& out) {
   std::string content;
   syntax_highlight_in_html(out, content);
   out.swap(content);
}

namespace {
   bool _is_relative_path(const std::string& path) {
      if (path[0] == '/' || path[0] == '\\')
         return false;
      if (path.compare(0, 7, "script/") == 0) // hack to avoid having to edit 90% of the documentation files again
         return false;
      return true;
   }
   void _fix_link(const std::string& stem, std::string& out) {
      //
      // Really hacky link/image fixup code:
      //
      std::string hash;
      {
         auto h = out.find_last_of('#');
         if (h != std::string::npos) {
            hash = out.substr(h, out.size() - h);
            out  = out.substr(0, h);
         }
      }
      if (_is_relative_path(out)) {
         //
         // Articles typically use hyperlink and image paths that are relative to themselves, so 
         // we need to fix those up since we're using a <base/> element to change relative paths 
         // (so that the nav and asset paths can work).
         //
         out = stem + out;
      } else if (out[0] == '/' || out[0] == '\\') {
         out = out.substr(1);
      }
      //
      // Links between articles usually lack file extensions.
      //
      auto a = out.find_last_of(".");
      auto b = out.find_last_of("/");
      if (a == std::string::npos || (a < b && b != std::string::npos)) {
         out += ".html";
      }
      out += hash;
   }
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
            if (token.name == "href" || token.name == "src")
               _fix_link(stem, token.value);
            cobb::sprintf(temp, " %s=\"%s\"", token.name.c_str(), token.value.c_str());
            break;
         case token_code::text_content:
            if (last_opened_tag == "pre") {
               std::string html;
               _append_text_content(html, token.value);
               minimize_indent(html);
               syntax_highlight_in_html(html);
               out += html;
               break;
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
            if (token.name != "img")
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