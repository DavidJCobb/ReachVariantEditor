#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include "helpers/xml.h"

enum class api_entry_type {
   generic,
   same, // target is of the same type as subject
   action,
   condition,
   property,
   accessor,
};

class APIEntry {
   public:
      struct note {
         std::string title; // optional; generates sub-heading
         std::string id;    // if there is a title, then this is the <a name="..."> identifier
         std::string content;
      };
      struct relationship {
         std::string    context; // basis.whatever
         api_entry_type type = api_entry_type::same;
         std::string    name;
      };
      //
      std::string name;
      std::string name2;
      std::string blurb;
      std::string description;
      std::string example;
      //
      std::vector<note> notes;
      std::vector<relationship> related;
};

class APIMethod : public APIEntry {
   public:
      struct argument {
         std::string name;
         std::string type; // optional
         std::string content; // optional
      };
      //
      std::string bare_argument_text; // some methods use <args>some text here</args> instead of <args> <arg/> </args>
      std::string return_value_type;  // optional
      std::vector<argument> args;
      int32_t opcode_id = -1;
      bool    nodiscard = true; // only relevant if (return_value_type) is not empty
      bool    is_action = false;
      //
      inline bool has_return_value() const noexcept { return !this->return_value_type.empty();  }

      size_t load(cobb::xml::document& doc, uint32_t root_token, std::string member_of, bool is_condition);
      void write(std::string& out, std::string stem, std::string member_of, const std::string& type_template);
};

class APIProperty : public APIEntry {
   public:
      std::string type;
      bool is_read_only = false;
};

class APIAccessor : public APIEntry {
   public:
      std::string type;
      bool getter = false;
      bool setter = false;
};

class APIType : public APIEntry {
   protected:
      void _handle_member_nav(std::string& content, const std::string& stem);
   public:
      std::string              friendly_name;
      std::vector<APIMethod>   conditions;
      std::vector<APIMethod>   actions;
      std::vector<APIProperty> properties;
      std::vector<APIAccessor> accessors;
      bool is_variable = false;
      struct {
         bool    defined = false;
         uint8_t numbers = 0;
         uint8_t timers  = 0;
         uint8_t teams   = 0;
         uint8_t players = 0;
         uint8_t objects = 0;
      } scope; // if this type can be a variable scope, how many of each variable type can it hold?

      const char* get_friendly_name() const noexcept;

      void load(cobb::xml::document& doc);
      void write(std::filesystem::path, int depth, std::string stem, const std::string& article_template, const std::string& type_template);
};