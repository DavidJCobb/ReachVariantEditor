#include "api_entry.h"
#undef NDEBUG // i want asserts in the Release build
#include <cassert>
#include <fstream>
#include "common.h"
#include "helpers/strings.h"

size_t APIEntry::_load_blurb(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem) {
   std::string out;
   auto extract = extract_html_from_xml(start_tag_token_index, doc, out, stem);
   if (extract != std::string::npos) {
      ensure_paragraph(out);
      std::swap(this->blurb, out);
   }
   return extract;
}
size_t APIEntry::_load_description(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem) {
   std::string out;
   auto extract = extract_html_from_xml(start_tag_token_index, doc, out, stem);
   if (extract != std::string::npos)
      std::swap(this->description, out);
   return extract;
}
size_t APIEntry::_load_example(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem) {
   std::string out;
   auto extract = extract_html_from_xml(start_tag_token_index, doc, out, stem);
   if (extract != std::string::npos) {
      minimize_indent(out); // the example will be rendered in PRE tags
      std::swap(this->example, out);
   }
   return extract;
}
size_t APIEntry::_load_relationship(cobb::xml::document& doc, uint32_t start_tag_token_index) {
   this->related.emplace_back();
   auto& rel = this->related.back();
   //
   uint32_t depth = 0;
   uint32_t j     = start_tag_token_index + 1;
   for (; j < doc.tokens.size(); ++j) {
      auto& token = doc.tokens[j];
      if (token.code == cobb::xml::token_code::element_open)
         ++depth;
      if (token.code == cobb::xml::token_code::element_close) {
         if (depth == 0 && token.name == "related")
            break;
         --depth;
      }
      if (token.code == cobb::xml::token_code::attribute && depth == 0) {
         assert(!this->related.empty());
         if (token.name == "type") {
            if (token.value == "action")
               rel.type = api_entry_type::action;
            else if (token.value == "condition")
               rel.type = api_entry_type::condition;
            else if (token.value == "property")
               rel.type = api_entry_type::property;
            else if (token.value == "accessor")
               rel.type = api_entry_type::accessor;
         } else if (token.name == "name") {
            rel.name = token.value;
         } else if (token.name == "context") {
            rel.context = token.value;
         }
      }
   }
   if (depth)
      return std::string::npos;
   return j;
}
size_t APIEntry::_load_note(cobb::xml::document& doc, uint32_t start_tag_token_index, std::string stem) {
   this->notes.emplace_back();
   auto& note = this->notes.back();
   //
   for (size_t j = start_tag_token_index + 1; j < doc.tokens.size(); ++j) {
      auto& token = doc.tokens[j];
      if (token.code != cobb::xml::token_code::attribute)
         break;
      if (token.name == "title")
         note.title = token.value;
      else if (token.name == "id")
         note.id = token.value;
   }
   auto extract = extract_html_from_xml(start_tag_token_index, doc, note.content, stem);
   assert(extract != std::string::npos);
   ensure_paragraph(note.content);
   return extract;
}
//
void APIEntry::_write_description(std::string& out) const {
   if (this->description.empty()) {
      if (this->blurb.empty())
         out += "<p>No description available.</p>";
      else
         out += this->blurb;
   } else
      out += this->description;
}
void APIEntry::_write_example(std::string& out) const {
   if (this->example.empty())
      return;
   out += "<h2>Example</h2>\n<pre>";
   out += this->example;
   out += "</pre>\n";
}
void APIEntry::_write_notes(std::string& out) const {
   if (this->notes.empty())
      return;
   bool has_any_named = false;
   bool has_any_bare  = false;
   //
   out += "<h2>Notes</h2>\n";
   for (auto& note : this->notes) {
      if (note.title.empty()) {
         has_any_bare = true;
         continue;
      }
      has_any_named = true;
      //
      out += "<h3>";
      if (!note.id.empty()) {
         out += "<a name=\"";
         out += note.id;
         out += "\">";
      }
      out += note.title;
      if (!note.id.empty())
         out += "</a>";
      out += "</h3>\n";
      //
      out += note.content;
   }
   if (has_any_bare) {
      if (has_any_named)
         out += "<h3>Other notes</h3>\n";
      out += "<ul>\n";
      for (auto& note : this->notes) {
         if (!note.title.empty())
            continue;
         out += "   <li>";
         out += note.content;
         out += "</li>\n";
      }
      out += "</ul>";
   }
}
void APIEntry::_write_relationships(std::string& out, const std::string& member_of, api_entry_type my_type) const {
   if (this->related.empty())
      return;
   out += "<h2>See also</h2>\n<ul>\n";
   for (auto& rel : this->related) {
      out += "   <li><a href=\"script/api/";
      //
      std::string context = rel.context;
      if (context.empty())
         context = member_of;
      //
      out += context;
      out += "/";
      auto rt = rel.type;
      if (rt == api_entry_type::same)
         rt = my_type;
      switch (rt) {
         case api_entry_type::action: out += "actions/"; break;
         case api_entry_type::condition: out += "conditions/"; break;
         case api_entry_type::property: out += "properties/"; break;
         case api_entry_type::accessor: out += "accessors/"; break;
      }
      out += rel.name;
      out += ".html\">";
      out += context;
      if (!context.empty())
         out += '.';
      out += rel.name;
      out += "</a></li>\n";
   }
   out += "</ul>\n";
}

#pragma region APIMethod 
size_t APIMethod::load(cobb::xml::document& doc, uint32_t root_token, std::string member_of, bool is_condition) {
   enum class location {
      nowhere,
      args,
   };
   //
   std::string stem;
   cobb::sprintf(stem, "script/api/%s/%s/", member_of.c_str(), is_condition ? "conditions" : "actions");
   //
   location where = location::nowhere;
   uint32_t depth = 0;
   size_t   extract;
   uint32_t i    = root_token + 1;
   size_t   size = doc.tokens.size();
   size_t   args_at = std::string::npos;
   for (; i < size; ++i) {
      auto& token = doc.tokens[i];
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
                  args_at = i;
                  where   = location::args;
                  continue;
               }
               //
               if (token.name == "blurb") {
                  extract = this->_load_blurb(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
               if (token.name == "description") {
                  extract = this->_load_description(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
               if (token.name == "example") {
                  extract = this->_load_example(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
               if (token.name == "note") {
                  extract = this->_load_note(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
               if (token.name == "related") {
                  extract = this->_load_relationship(doc, i);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
            }
            ++depth;
            break;
         case location::args:
            if (this->args.empty()) {
               if (token.code == cobb::xml::token_code::text_content && this->bare_argument_text.empty()) {
                  if (token.value.find_first_not_of(" \r\n\t\v") == std::string::npos)
                     //
                     // This text content is just leading whitespace. Ignore it.
                     //
                     continue;
                  assert(args_at != std::string::npos);
                  std::string out;
                  auto result = extract_html_from_xml(args_at, doc, out, stem);
                  assert(result != std::string::npos);
                  std::swap(this->bare_argument_text, out);
                  i       = result;
                  args_at = std::string::npos;
                  where   = location::nowhere;
                  continue;
               }
            }
            if (token.code == cobb::xml::token_code::element_open) {
               if (depth == 0) {
                  if (token.name == "arg") {
                     this->args.emplace_back();
                     auto& arg = this->args.back();
                     //
                     // Get attributes:
                     //
                     for (uint32_t j = i + 1; j < size; ++j) {
                        auto& token = doc.tokens[j];
                        if (token.code != cobb::xml::token_code::attribute)
                           break;
                        if (token.name == "name")
                           arg.name = token.value;
                        else if (token.name == "type")
                           arg.type = token.value;
                     }
                     //
                     // Get argument content:
                     //
                     extract = extract_html_from_xml(i, doc, arg.content, stem);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     continue;
                  }
                  if (this->bare_argument_text.empty()) {
                     assert(args_at != std::string::npos);
                     std::string out;
                     auto result = extract_html_from_xml(args_at, doc, out, stem);
                     assert(result != std::string::npos);
                     std::swap(this->bare_argument_text, out);
                     i       = result;
                     args_at = std::string::npos;
                     where   = location::nowhere;
                     continue;
                  }
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
void APIMethod::write(std::string& content, std::string stem, std::string member_of, const std::string& type_template) {
   std::string full_title = member_of;
   if (!full_title.empty())
      full_title += '.';
   full_title += this->name;
   //
   handle_page_title_tag(content, full_title);
   //
   std::string needle = "<content-placeholder id=\"main\" />";
   std::size_t pos    = content.find(needle.c_str());
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   //
   std::string replace;
   replace += "<h1>";
   replace += full_title;
   if (!this->name2.empty()) {
      replace += "<span class=\"a-k-a\">a.k.a. ";
      if (!member_of.empty()) {
         replace += member_of;
         replace += '.';
      }
      replace += this->name2;
      replace += "</span>";
   }
   replace += "</h1>\n";
   this->_write_description(replace);
   if (this->has_return_value()) {
      replace += "<p>This function returns <a href=\"script/api/";
      replace += this->return_value_type;
      replace += ".html\">";
      replace += this->return_value_type;
      replace += "</a>.";
      if (this->nodiscard)
         replace += " Calling this function without storing its return value in a variable is an error.";
      replace += "</p>\n";
   }
   if (this->args.size() || !this->bare_argument_text.empty()) {
      replace += "<h2>Arguments</h2>\n";
      if (!this->bare_argument_text.empty()) {
         replace += this->bare_argument_text;
      } else {
         replace += "<dl>\n";
         for (auto& arg : this->args) {
            bool has_type = !arg.type.empty() && arg.type.find_first_of(':') == std::string::npos;
            //
            replace += "   <dt>";
            if (has_type) {
               replace += "<a href=\"script/api/";
               replace += arg.type;
               replace += ".html\">";
            }
            if (!arg.name.empty()) {
               replace += arg.name;
            } else {
               replace += arg.type;
            }
            if (has_type)
               replace += "</a>";
            replace += "</dt>\n      <dd>";
            if (arg.content.empty())
               //
               // TODO: if (has_type), then this should use the blurb or description for that type, preferring 
               // the blurb. This requires loading ALL types into memory before being able to export HTML for 
               // any of them.
               //
               replace += "No description available.";
            else
               replace += arg.content;
            replace += "</dd>\n";
         }
         replace += "</dl>\n";
      }
   }
   this->_write_example(replace);
   this->_write_notes(replace);
   this->_write_relationships(replace, member_of, this->is_action ? api_entry_type::action : api_entry_type::condition);
   content.replace(pos, needle.length(), replace);
}
#pragma endregion

#pragma region APIProperty 
size_t APIProperty::load(cobb::xml::document& doc, uint32_t root_token, std::string member_of) {
   std::string stem;
   cobb::sprintf(stem, "script/api/%s/properties/", member_of.c_str());
   //
   int32_t depth = 0;
   size_t  extract;
   size_t  size = doc.tokens.size();
   for (size_t i = i = root_token + 1; i < size; ++i) {
      auto& token = doc.tokens[i];
      //
      if (token.code == cobb::xml::token_code::attribute && depth == 0) {
         if (token.name == "name")
            this->name = token.value;
         else if (token.name == "name2")
            this->name2 = token.value;
         else if (token.name == "type")
            this->type = token.value;
         continue;
      }
      //
      if (token.code == cobb::xml::token_code::element_close) {
         if (depth == 0) {
            assert(token.name == "property");
            return i;
         }
         --depth;
      }
      if (token.code != cobb::xml::token_code::element_open)
         continue;
      if (depth == 0) {
         if (token.name == "blurb") {
            extract = this->_load_blurb(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "description") {
            extract = this->_load_description(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "example") {
            extract = this->_load_example(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "note") {
            extract = this->_load_note(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "related") {
            extract = this->_load_relationship(doc, i);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
      }
      ++depth;
   }
   return std::string::npos;
}
void APIProperty::write(std::string& content, std::string stem, std::string member_of, const std::string& type_template) {
   std::string full_title = member_of;
   if (!full_title.empty())
      full_title += '.';
   full_title += this->name;
   //
   handle_page_title_tag(content, full_title);
   //
   std::string needle = "<content-placeholder id=\"main\" />";
   std::size_t pos = content.find(needle.c_str());
   std::string replace;
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   //
   replace += "<h1>";
   replace += full_title;
   replace += "</h1>\n";
   this->_write_description(replace);
   this->_write_example(replace);
   this->_write_notes(replace);
   this->_write_relationships(replace, member_of, api_entry_type::property);
   content.replace(pos, needle.length(), replace);
}
#pragma endregion

#pragma region APIAccessor 
size_t APIAccessor::load(cobb::xml::document& doc, uint32_t root_token, std::string member_of) {
   std::string stem;
   cobb::sprintf(stem, "script/api/%s/accessors/", member_of.c_str());
   //
   int32_t depth = 0;
   size_t  extract;
   size_t  size = doc.tokens.size();
   for (size_t i = i = root_token + 1; i < size; ++i) {
      auto& token = doc.tokens[i];
      //
      if (token.code == cobb::xml::token_code::attribute && depth == 0) {
         if (token.name == "name")
            this->name = token.value;
         else if (token.name == "name2")
            this->name2 = token.value;
         else if (token.name == "type")
            this->type = token.value;
         else if (token.name == "getter")
            this->getter = true;
         else if (token.name == "setter")
            this->setter = true;
         continue;
      }
      //
      if (token.code == cobb::xml::token_code::element_close) {
         if (depth == 0) {
            assert(token.name == "accessor");
            return i;
         }
         --depth;
      }
      if (token.code != cobb::xml::token_code::element_open)
         continue;
      if (depth == 0) {
         if (token.name == "blurb") {
            extract = this->_load_blurb(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "description") {
            extract = this->_load_description(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "example") {
            extract = this->_load_example(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "note") {
            extract = this->_load_note(doc, i, stem);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
         if (token.name == "related") {
            extract = this->_load_relationship(doc, i);
            assert(extract != std::string::npos);
            i = extract;
            continue;
         }
      }
      ++depth;
   }
   return std::string::npos;
}
void APIAccessor::write(std::string& content, std::string stem, std::string member_of, const std::string& type_template) {
   std::string full_title = member_of;
   if (!full_title.empty())
      full_title += '.';
   full_title += this->name;
   //
   handle_page_title_tag(content, full_title);
   //
   std::string needle = "<content-placeholder id=\"main\" />";
   std::size_t pos = content.find(needle.c_str());
   std::string replace;
   assert(pos != std::string::npos && "Missing placeholder (<content-placeholder id=\"main\" />)");
   //
   replace += "<h1>";
   replace += full_title;
   replace += "</h1>\n";
   this->_write_description(replace);
   if (this->getter) {
      if (this->setter) {
         replace += "<p>This accessor supports read and write access.</p>\n";
      } else {
         replace += "<p>This accessor supports read access only.</p>\n"; // should never happen; accessors should only be used when the "set" opcode allows arbitrary mathematical operators
      }
   } else {
      replace += "<p>This accessor supports write access only; you cannot read an existing value.</p>\n";
   }
   this->_write_example(replace);
   this->_write_notes(replace);
   this->_write_relationships(replace, member_of, api_entry_type::accessor);
   content.replace(pos, needle.length(), replace);
}
#pragma endregion

const char* APIType::get_friendly_name() const noexcept {
   if (!this->friendly_name.empty())
      return this->friendly_name.c_str();
   if (!this->name.empty())
      return this->name.c_str();
   return this->name2.c_str();
}
APIMethod* APIType::get_action_by_name(const std::string& name) {
   for (auto& member : this->actions) {
      if (member.is_stub)
         continue;
      if (member.name == name || member.name2 == name)
         return &member;
   }
   return nullptr;
}
APIMethod* APIType::get_condition_by_name(const std::string& name) {
   for (auto& member : this->conditions) {
      if (member.is_stub)
         continue;
      if (member.name == name || member.name2 == name)
         return &member;
   }
   return nullptr;
}
APIProperty* APIType::get_property_by_name(const std::string& name) {
   for (auto& member : this->properties)
      if (member.name == name || member.name2 == name)
         return &member;
   return nullptr;
}
APIAccessor* APIType::get_accessor_by_name(const std::string& name) {
   for (auto& member : this->accessors)
      if (member.name == name || member.name2 == name)
         return &member;
   return nullptr;
}
//
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
                  auto extract = this->_load_blurb(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
                  continue;
               }
               if (token.name == "description") {
                  auto extract = this->_load_description(doc, i, stem);
                  assert(extract != std::string::npos);
                  i = extract;
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
                     opcode.is_action = false;
                     auto  extract = opcode.load(doc, i, this->name, true);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     if (!opcode.name2.empty()) {
                        this->conditions.emplace_back();
                        auto& stub   = this->conditions.back();
                        auto& source = this->conditions[this->conditions.size() - 2]; // can't use (opcode) if (emplace_back) ends up invalidating iterators
                        stub.is_stub = true;
                        stub.name    = source.name2;
                     }
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
                     opcode.is_action = true;
                     auto  extract = opcode.load(doc, i, this->name, false);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     if (!opcode.name2.empty()) {
                        this->actions.emplace_back();
                        auto& stub   = this->actions.back();
                        auto& source = this->actions[this->actions.size() - 2]; // can't use (opcode) if (emplace_back) ends up invalidating iterators
                        stub.is_stub = true;
                        stub.name    = source.name2;
                     }
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
                     this->properties.emplace_back();
                     auto& member = this->properties.back();
                     auto  extract = member.load(doc, i, this->name);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     continue;
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
                     this->accessors.emplace_back();
                     auto& member = this->accessors.back();
                     auto  extract = member.load(doc, i, this->name);
                     assert(extract != std::string::npos);
                     i = extract;
                     //
                     continue;
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
   this->_make_member_relationships_bidirectional();
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
   cobb::sprintf(replace, "<a href=\"script/api/%s.html\">%s</a>", this->name.c_str(), this->get_friendly_name());
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
         cobb::sprintf(li, "<li><a href=\"script/api/%s/properties/%s.html\">%s</a></li>\n", this->name.c_str(), prop.name.c_str(), prop.name.c_str());
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
         cobb::sprintf(li, "<li><a href=\"script/api/%s/accessors/%s.html\">%s</a></li>\n", this->name.c_str(), prop.name.c_str(), prop.name.c_str());
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
         if (prop.is_stub) {
            //
            // Conditions with two names should have menu items generated for both names.
            //
            auto* target = this->get_condition_by_name(prop.name);
            if (target) {
               std::string li;
               cobb::sprintf(li, "<li><a href=\"script/api/%s/conditions/%s.html\">%s</a></li>\n", this->name.c_str(), target->name.c_str(), prop.name.c_str());
               replace += li;
            }
            continue;
         }
         std::string li;
         cobb::sprintf(li, "<li><a href=\"script/api/%s/conditions/%s.html\">%s</a></li>\n", this->name.c_str(), prop.name.c_str(), prop.name.c_str());
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
         if (prop.is_stub) {
            //
            // Actions with two names should have menu items generated for both names.
            //
            auto* target = this->get_action_by_name(prop.name);
            if (target) {
               std::string li;
               cobb::sprintf(li, "<li><a href=\"script/api/%s/actions/%s.html\">%s</a></li>\n", this->name.c_str(), target->name.c_str(), prop.name.c_str());
               replace += li;
            }
            continue;
         }
         std::string li;
         cobb::sprintf(li, "<li><a href=\"script/api/%s/actions/%s.html\">%s</a></li>\n", this->name.c_str(), prop.name.c_str(), prop.name.c_str());
         replace += li;
      }
      content.replace(pos, needle.length(), replace);
   }
}
void APIType::_mirror_member_relationships(APIType& member_of, APIEntry& member, api_entry_type member_type) {
   for (auto& rel : member.related) {
      if (rel.mirrored)
         continue;
      if (!rel.context.empty() && rel.context != member_of.name) // this is a relationship to something in another type
         continue;
      APIEntry* target = nullptr;
      //
      auto rt = rel.type;
      if (rt == api_entry_type::same)
         rt = member_type;
      switch (rt) {
         case api_entry_type::condition:
            target = member_of.get_condition_by_name(rel.name);
            break;
         case api_entry_type::action:
            target = member_of.get_action_by_name(rel.name);
            break;
         case api_entry_type::property:
            target = member_of.get_property_by_name(rel.name);
            break;
         case api_entry_type::accessor:
            target = member_of.get_accessor_by_name(rel.name);
            break;
      }
      //
      if (target == nullptr)
         continue;
      target->related.emplace_back();
      auto& mirrored = target->related.back();
      mirrored.mirrored = true;
      mirrored.name = member.name;
      mirrored.type = member_type;
      //
      for (auto& rel2 : member.related) {
         if (&rel2 == &rel)
            continue;
         target->related.emplace_back();
         auto& mirrored = target->related.back();
         mirrored = rel2;
         mirrored.mirrored = true;
      }
   }
}
void APIType::_make_member_relationships_bidirectional() {
   for (auto& member : this->conditions)
      this->_mirror_member_relationships(*this, member, api_entry_type::condition);
   for (auto& member : this->actions)
      this->_mirror_member_relationships(*this, member, api_entry_type::action);
   for (auto& member : this->properties)
      this->_mirror_member_relationships(*this, member, api_entry_type::property);
   for (auto& member : this->accessors)
      this->_mirror_member_relationships(*this, member, api_entry_type::accessor);
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
         if (prop.is_stub)
            continue;
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/conditions/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         if (!prop.name2.empty()) {
            replace += " &nbsp; a.k.a. &nbsp; ";
            replace += prop.name2;
         }
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
         if (prop.is_stub)
            continue;
         replace += "<dt><a href=\"script/api/";
         replace += this->name;
         replace += "/actions/";
         replace += prop.name;
         replace += ".html\">";
         replace += prop.name;
         if (!prop.name2.empty()) {
            replace += " &nbsp; a.k.a. &nbsp; ";
            replace += prop.name2;
         }
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
         if (member.is_stub)
            continue;
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         member.write(content, stem, this->name, type_template);
         //
         auto pm = p.parent_path();
         pm.append(this->name);
         std::filesystem::create_directory(pm);
         pm.append("conditions");
         std::filesystem::create_directory(pm);
         pm.append(""); // the above line produced "some/path/conditions" and the API doesn't remember that "conditions" was a directory, so we need this or the next two calls will replace "conditions" with the filename
         pm.replace_filename(member.name); // this api sucks
         pm.replace_extension(".html");
         std::ofstream file(pm.c_str());
         file.write(content.c_str(), content.size());
         file.close();
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/actions/";
      for (auto& member : this->actions) {
         if (member.is_stub)
            continue;
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         member.write(content, stem, this->name, type_template);
         //
         auto pm = p.parent_path();
         pm.append(this->name);
         std::filesystem::create_directory(pm);
         pm.append("actions");
         std::filesystem::create_directory(pm);
         pm.append(""); // the above line produced "some/path/actions" and the API doesn't remember that "actions" was a directory, so we need this or the next two calls will replace "actions" with the filename
         pm.replace_filename(member.name); // this api sucks
         pm.replace_extension(".html");
         std::ofstream file(pm.c_str());
         file.write(content.c_str(), content.size());
         file.close();
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/properties/";
      for (auto& member : this->properties) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         member.write(content, stem, this->name, type_template);
         //
         auto pm = p.parent_path();
         pm.append(this->name);
         std::filesystem::create_directory(pm);
         pm.append("properties");
         std::filesystem::create_directory(pm);
         pm.append(""); // the above line produced "some/path/properties" and the API doesn't remember that "properties" was a directory, so we need this or the next two calls will replace "properties" with the filename
         pm.replace_filename(member.name); // this api sucks
         pm.replace_extension(".html");
         std::ofstream file(pm.c_str());
         file.write(content.c_str(), content.size());
         file.close();
      }
   }
   {
      std::string stem_member = stem + this->name;
      stem_member += "/accessors/";
      for (auto& member : this->accessors) {
         content = type_template;
         handle_base_tag(content, depth + 2);
         this->_handle_member_nav(content, stem_member);
         member.write(content, stem, this->name, type_template);
         //
         auto pm = p.parent_path();
         pm.append(this->name);
         std::filesystem::create_directory(pm);
         pm.append("accessors");
         std::filesystem::create_directory(pm);
         pm.append(""); // the above line produced "some/path/accessors" and the API doesn't remember that "accessors" was a directory, so we need this or the next two calls will replace "accessors" with the filename
         pm.replace_filename(member.name); // this api sucks
         pm.replace_extension(".html");
         std::ofstream file(pm.c_str());
         file.write(content.c_str(), content.size());
         file.close();
      }
   }
}