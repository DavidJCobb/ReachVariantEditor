#include "api_entry.h"
#include "helpers/strings.h"

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
      blurb,
      scope,
      description,
   };
   //
   int32_t root = doc.find_element("script-type");
   if (root < 0)
      return;
   //
   location where = location::nowhere;
   target   focus = target::nothing;
   uint32_t depth = 0; // how many unrecognized tags are we currently nested under?
   for (uint32_t i = root + 1; i < doc.tokens.size(); ++i) {
      auto& token = doc.tokens[i];
      //
      switch (focus) {
         case target::blurb:
            //
            // TODO: read and preserve HTML content; adjust links and images as appropriate
            //
            continue;
         case target::description:
            //
            // TODO: read and preserve HTML content; adjust links and images as appropriate
            //
            continue;
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
               if (token.name == "numbers" && valid)
                  this->scope.numbers = i;
               if (token.name == "objects" && valid)
                  this->scope.objects = i;
               if (token.name == "players" && valid)
                  this->scope.players = i;
               if (token.name == "teams" && valid)
                  this->scope.teams = i;
               if (token.name == "timers" && valid)
                  this->scope.timers = i;
            }
            continue;
      }
      //
      switch (where) {
         case location::nowhere:
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
                  focus = target::blurb;
                  continue;
               }
               if (token.name == "description") {
                  focus = target::description;
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
                     //
                     // TODO
                     //
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
                     //
                     // TODO
                     //
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
}