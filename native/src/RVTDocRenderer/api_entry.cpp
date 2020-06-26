#include "api_entry.h"
#undef NDEBUG // i want asserts in the Release build
#include <cassert>
#include <fstream>
#include "common.h"
#include "helpers/strings.h"

size_t APIMethod::load(cobb::xml::document& doc, uint32_t root_token, std::string member_of, bool is_condition) {
   enum class location {
      nowhere,
      args,
   };
   enum class target {
      nothing,
      arg,
      note,
      related,
   };
   //
   std::string stem;
   cobb::sprintf(stem, "script/api/%s/%s/", member_of.c_str(), is_condition ? "conditions" : "actions");
   //
   location where = location::nowhere;
   target   focus = target::nothing;
   uint32_t depth = 0;
   size_t   extract;
   uint32_t i = root_token + 1;
   for (; i < doc.tokens.size(); ++i) {
      auto& token = doc.tokens[i];
      //
      switch (focus) {
         case target::arg:
            if (token.code == cobb::xml::token_code::element_open)
               ++depth;
            if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0 && token.name == "arg") {
                  focus = target::nothing;
                  continue;
               }
               --depth;
            }
            //
            // TODO
            //
            continue;
         case target::note:
            if (token.code == cobb::xml::token_code::element_open)
               ++depth;
            if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0 && token.name == "note") {
                  focus = target::nothing;
                  continue;
               }
               --depth;
            }
            //
            // TODO: extract attributes for the note tag, AND get the content as HTML
            //
            continue;
         case target::related:
            if (token.code == cobb::xml::token_code::element_open)
               ++depth;
            if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0 && token.name == "related") {
                  focus = target::nothing;
                  continue;
               }
               --depth;
            }
            //
            // TODO: extract attributes; ignore content
            //
            continue;
      }
      //
      switch (where) {
         case location::nowhere:
            if (token.code == cobb::xml::token_code::attribute && depth == 0) {
               if (token.name == "name")
                  this->name = token.value;
               else if (token.name == "name2")
                  this->name2 = token.value;
               else if (token.name == "returns")
                  this->return_value_type = token.value;
               else if (token.name == "nodiscard") {
                  if (token.value == "true")
                     this->nodiscard = true;
                  else if (token.value == "false")
                     this->nodiscard = false;
               } else if (token.name == "id") {
                  int32_t i;
                  if (cobb::string_to_int(token.value.c_str(), i))
                     this->opcode_id = i;
               }
               continue;
            }
            //
            if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  assert(token.name == "method");
                  return i;
               }
               --depth;
            }
            if (token.code != cobb::xml::token_code::element_open)
               continue;
            if (depth == 0) {
               if (token.name == "args") {
                  where = location::args;
                  continue;
               }
               //
               if (token.name == "blurb") {
                  std::string out;
                  extract = extract_html_from_xml(i, doc, out, stem);
                  assert(extract != std::string::npos);
                  std::swap(this->blurb, out);
                  focus = target::nothing;
                  i = extract;
                  //
                  continue;
               }
               if (token.name == "description") {
                  std::string out;
                  extract = extract_html_from_xml(i, doc, out, stem);
                  assert(extract != std::string::npos);
                  std::swap(this->description, out);
                  focus = target::nothing;
                  i = extract;
                  //
                  continue;
               }
               if (token.name == "example") {
                  std::string out;
                  extract = extract_html_from_xml(i, doc, out, stem);
                  assert(extract != std::string::npos);
                  minimize_indent(out); // the example will be rendered in PRE tags
                  std::swap(this->example, out);
                  focus = target::nothing;
                  i = extract;
                  //
                  continue;
               }
               if (token.name == "note") {
                  focus = target::note;
                  continue;
               }
               if (token.name == "related") {
                  focus = target::related;
                  continue;
               }
            }
            ++depth;
            break;
         case location::args:
            if (token.code == cobb::xml::token_code::text_content && this->bare_argument_text.empty()) {
               if (token.value.find_first_not_of(" \r\n\t\v") != std::string::npos) {
                  this->bare_argument_text += token.value;
                  continue;
               }
            }
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0 && token.name == "arg") {
                  focus = target::arg;
                  continue;
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0 && token.name == "args") {
                  where = location::nowhere;
                  continue;
               }
               --depth;
            }
            break;
      }
   }
   return std::string::npos;
}
void APIMethod::write(std::string& out, std::string stem, const std::string& type_template) {
}

const char* APIType::get_friendly_name() const noexcept {
   if (!this->friendly_name.empty())
      return this->friendly_name.c_str();
   if (!this->name.empty())
      return this->name.c_str();
   return this->name2.c_str();
}
void APIType::load(cobb::xml::document& doc) {
   enum class location {
      nowhere,
      methods,
      conditions,
      actions,
      properties,
      accessors,
   };
   enum class target {
      nothing,
      friendly,
      scope,
   };
   //
   int32_t root = doc.find_element("script-type");
   if (root < 0)
      return;
   //
   std::string stem = "script/api/";
   //
   location where = location::nowhere;
   target   focus = target::nothing;
   uint32_t depth = 0; // how many unrecognized tags are we currently nested under?
   for (uint32_t i = root + 1; i < doc.tokens.size(); ++i) {
      auto& token = doc.tokens[i];
      //
      switch (focus) {
         case target::friendly:
            //
            // NOTE: Ignores nested tags; only reads text content not nested in any tag
            //
            if (token.code == cobb::xml::token_code::element_open)
               ++depth;
            else if (token.code == cobb::xml::token_code::element_close) {
               if (token.name == "friendly") {
                  focus = target::nothing;
                  continue;
               }
               --depth;
            } else if (token.code == cobb::xml::token_code::text_content)
               this->friendly_name += token.value;
            continue;
         case target::scope:
            //
            // NOTE: Only cares about specific attributes on itself; ignores all other content
            //
            if (token.code == cobb::xml::token_code::element_open)
               ++depth;
            if (token.code == cobb::xml::token_code::element_close) {
               if (token.name == "scope") {
                  focus = target::nothing;
                  continue;
               }
               --depth;
            }
            if (token.code == cobb::xml::token_code::attribute) {
               int32_t i;
               bool    valid = cobb::string_to_int(token.value.c_str(), i);
               if (valid) {
                  this->scope.defined = true;
                  if (token.name == "numbers")
                     this->scope.numbers = i;
                  if (token.name == "objects")
                     this->scope.objects = i;
                  if (token.name == "players")
                     this->scope.players = i;
                  if (token.name == "teams")
                     this->scope.teams = i;
                  if (token.name == "timers")
                     this->scope.timers = i;
               }
            }
            continue;
      }
      //
      switch (where) {
         case location::nowhere:
            if (token.code == cobb::xml::token_code::attribute && depth == 0) {
               if (token.name == "name")
                  this->name = token.value;
               else if (token.name == "name2")
                  this->name2 = token.value;
               continue;
            }
            //
            if (token.code == cobb::xml::token_code::element_close)
               --depth;
            if (token.code != cobb::xml::token_code::element_open)
               continue;
            if (depth == 0) {
               if (token.name == "methods") {
                  where = location::methods;
                  continue;
               }
               if (token.name == "properties") {
                  where = location::properties;
                  continue;
               }
               if (token.name == "accessors") {
                  where = location::accessors;
                  continue;
               }
               //
               if (token.name == "blurb") {
                  std::string out;
                  auto result = extract_html_from_xml(i, doc, out, stem);
                  assert(result != std::string::npos);
                  std::swap(this->blurb, out);
                  i = result;
                  //
                  continue;
               }
               if (token.name == "description") {
                  std::string out;
                  auto result = extract_html_from_xml(i, doc, out, stem);
                  assert(result != std::string::npos);
                  std::swap(this->description, out);
                  i = result;
                  //
                  continue;
               }
               if (token.name == "friendly") {
                  focus = target::friendly;
                  continue;
               }
               if (token.name == "scope") {
                  focus = target::scope;
                  continue;
               }
            }
            ++depth;
            break;
         case location::methods:
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "conditions") {
                     where = location::conditions;
                     continue;
                  }
                  if (token.name == "actions") {
                     where = location::actions;
                     continue;
                  }
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  if (token.name == "methods") {
                     where = location::nowhere;
                     continue;
                  }
               }
               --depth;
            }
            break;
         case location::conditions:
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "method") {
                     this->conditions.emplace_back();
                     auto& opcode  = this->conditions.back();
                     auto  extract = opcode.load(doc, i, this->name, true);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     continue;
                  }
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  if (token.name == "conditions") {
                     where = location::methods;
                     continue;
                  }
               }
               --depth;
            }
            break;
         case location::actions:
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "method") {
                     this->actions.emplace_back();
                     auto& opcode  = this->actions.back();
                     auto  extract = opcode.load(doc, i, this->name, false);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     continue;
                  }
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  if (token.name == "actions") {
                     where = location::methods;
                     continue;
                  }
               }
               --depth;
            }
            break;
         case location::properties:
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "property") {
                     //
                     // TODO
                     //
                  }
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  if (token.name == "properties") {
                     where = location::nowhere;
                     continue;
                  }
               }
               --depth;
            }
            break;
         case location::accessors:
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "accessor") {
                     //
                     // TODO
                     //
                  }
               }
               ++depth;
            } else if (token.code == cobb::xml::token_code::element_close) {
               if (depth == 0) {
                  if (token.name == "accessors") {
                     where = location::nowhere;
                     continue;
                  }
               }
               --depth;
            }
            break;
      }
   }
   //
   // TODO: make all relationships defined with <related/> tags bidirectional
   //
   std::sort(this->actions.begin(), this->actions.end(), [](const APIMethod& a, const APIMethod& b) { return a.name < b.name; });
   std::sort(this->conditions.begin(), this->conditions.end(), [](const APIMethod& a, const APIMethod& b) { return a.name < b.name; });
   std::sort(this->properties.begin(), this->properties.end(), [](const APIEntry& a, const APIEntry& b) { return a.name < b.name; });
   std::sort(this->accessors.begin(), this->accessors.end(), [](const APIEntry& a, const APIEntry& b) { return a.name < b.name; });
}
void APIType::_handle_member_nav(std::string& content, const std::string& stem) {
   std::string needle  = "<content-placeholder id=\"nav-self-link\" />";
   std::size_t pos     = content.find(needle.c_str());
   std::string replace;
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"self-link\" />)");
   cobb::sprintf(replace, "<a href=\"%s%s.xml\">%s</a>", stem.c_str(), this->name.c_str(), this->get_friendly_name());
   content.replace(pos, needle.length(), replace);
   //
   {
      needle = "<content-placeholder id=\"nav-properties\" />";
      pos    = content.find(needle.c_str());
      assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"nav-properties\" />)");
      //
      replace.clear();
      for (auto& prop : this->properties) {
         std::string li;
         cobb::sprintf(li, "<li><a href=\"%sscript/api/%s/properties/%s.html\">%s</a></li>\n", stem.c_str(), this->name.c_str(), prop.name.c_str(), prop.name.c_str());
         replace += li;
      }
      content.replace(pos, needle.length(), replace);
   }
   {
      needle = "<content-placeholder id=\"nav-accessors\" />";
      pos    = content.find(needle.c_str());
      assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"nav-accessors\" />)");
      //
      replace.clear();
      for (auto& prop : this->accessors) {
         std::string li;
         cobb::sprintf(li, "<li><a href=\"%sscript/api/%s/accessors/%s.html\">%s</a></li>\n", stem.c_str(), this->name.c_str(), prop.name.c_str(), prop.name.c_str());
         replace += li;
      }
      content.replace(pos, needle.length(), replace);
   }
   {
      needle = "<content-placeholder id=\"nav-conditions\" />";
      pos    = content.find(needle.c_str());
      assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"nav-conditions\" />)");
      //
      replace.clear();
      for (auto& prop : this->conditions) {
         std::string li;
         cobb::sprintf(li, "<li><a href=\"%sscript/api/%s/conditions/%s.html\">%s</a></li>\n", stem.c_str(), this->name.c_str(), prop.name.c_str(), prop.name.c_str());
         replace += li;
      }
      content.replace(pos, needle.length(), replace);
   }
   {
      needle = "<content-placeholder id=\"nav-actions\" />";
      pos    = content.find(needle.c_str());
      assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"nav-actions\" />)");
      //
      replace.clear();
      for (auto& prop : this->actions) {
         std::string li;
         cobb::sprintf(li, "<li><a href=\"%sscript/api/%s/actions/%s.html\">%s</a></li>\n", stem.c_str(), this->name.c_str(), prop.name.c_str(), prop.name.c_str());
         replace += li;
      }
      content.replace(pos, needle.length(), replace);
   }
}
void APIType::write(std::filesystem::path p, int depth, std::string stem, const std::string& article_template, const std::string& type_template) {
   std::string content = article_template; // copy
   std::string needle  = "<content-placeholder id=\"main\" />";
   std::size_t pos     = content.find(needle.c_str());
   std::string replace;
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   //
   cobb::sprintf(replace, "<h1>%s</h1>\n", this->get_friendly_name());
   if (this->description.empty())
      replace += this->blurb;
   else
      replace += this->description;
   if (this->scope.defined) {
      std::string temp;
      cobb::sprintf(
         temp,
         "<p>This type contains nested variables. There are %u nested <a href=\"script/api/number.html\">numbers</a>, %u nested <a href=\"script/api/object.html\">objects</a>, %u nested <a href=\"script/api/player.html\">players</a>, %u nested <a href=\"script/api/team.html\">teams</a>, and %u nested <a href=\"script/api/timer.html\">timers</a> available.</p>",
         this->scope.numbers, this->scope.objects, this->scope.players, this->scope.teams, this->scope.timers
      );
      replace += temp;
   }
   if (this->properties.size()) {
      replace += "\n<h2>Properties</h2>\n<dl>";
      for (auto& prop : this->properties) {
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/properties/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         replace += "</a></dt>\n   <dd>";
         if (prop.blurb.empty())
            replace += "No description available.";
         else
            replace += prop.blurb;
         replace += "</dd>\n";
      }
      replace += "</dl>\n";
   }
   if (this->accessors.size()) {
      replace += "\n<h2>Accessors</h2>\n<dl>";
      for (auto& prop : this->accessors) {
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/accessors/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         replace += "</a></dt>\n   <dd>";
         if (prop.blurb.empty())
            replace += "No description available.";
         else
            replace += prop.blurb;
         replace += "</dd>\n";
      }
      replace += "</dl>\n";
   }
   if (this->conditions.size()) {
      replace += "\n<h2>Member conditions</h2>\n<dl>";
      for (auto& prop : this->conditions) {
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/conditions/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         replace += "</a></dt>\n   <dd>";
         if (prop.blurb.empty())
            replace += "No description available.";
         else
            replace += prop.blurb;
         replace += "</dd>\n";
      }
      replace += "</dl>\n";
   }
   if (this->actions.size()) {
      replace += "\n<h2>Member actions</h2>\n<dl>";
      for (auto& prop : this->actions) {
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/actions/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         replace += "</a></dt>\n   <dd>";
         if (prop.blurb.empty())
            replace += "No description available.";
         else
            replace += prop.blurb;
         replace += "</dd>\n";
      }
      replace += "</dl>\n";
   }
   content.replace(pos, needle.length(), replace);
   //
   handle_base_tag(content, depth);
   handle_page_title_tag(content, this->get_friendly_name());
   //
   p.replace_filename(this->name);
   p.replace_extension(".html");
   std::ofstream file(p.c_str());
   file.write(content.c_str(), content.size());
   file.close();
   //
   // Now generate the pages for members:
   //
   {
      std::string stem_member = stem + this->name;
      stem_member += "/conditions/";
      for (auto& member : this->conditions) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         //
         // TODO: generate the member content
         //
         // TODO: export the file
         //
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/actions/";
      for (auto& member : this->actions) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         //
         // TODO: generate the member content
         //
         // TODO: export the file
         //
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/properties/";
      for (auto& member : this->properties) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         //
         // TODO: generate the member content
         //
         // TODO: export the file
         //
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/accessors/";
      for (auto& member : this->accessors) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         handle_page_title_tag(content, member.name);
         this->_handle_member_nav(content, stem_member);
         //
         // TODO: generate the member content
         //
         // TODO: export the file
         //
      }
   }
}