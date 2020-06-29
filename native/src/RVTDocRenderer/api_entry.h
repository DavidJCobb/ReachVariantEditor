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

class APIRegistry;

class APIEntry {
   public:
      struct note {
         std::string title; // optional; generates sub-heading
         std::string id;    // if there is a title, then this is the <a name="..."> identifier
         std::string content;
      };
      struct relationship {
         std::string    context; // basis.whatever
         std::string    name;
         api_entry_type type = api_entry_type::same;
         bool mirrored = false;
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
      //
   protected:
      size_t _load_blurb(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem);
      size_t _load_description(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem);
      size_t _load_example(cobb::xml::document& doc, uint32_t start_tag_token_index, const std::string& stem);
      size_t _load_relationship(cobb::xml::document& doc, uint32_t start_tag_token_index);
      size_t _load_note(cobb::xml::document& doc, uint32_t start_tag_token_index, std::string stem);
      //
      void _write_description(std::string& out) const;
      void _write_example(std::string& out) const;
      void _write_notes(std::string& out) const;
      void _write_relationships(std::string& out, const std::string& member_of, api_entry_type my_type) const;
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
      bool    nodiscard = true;  // only relevant if (return_value_type) is not empty
      bool    is_action = false;
      bool    is_stub   = false; // used to simplify the sidebar listing both names for an opcode with two names: we just create a dummy entry whose name is the same as the real entry's name2
      //
      inline bool has_return_value() const noexcept { return !this->return_value_type.empty(); }

      size_t load(cobb::xml::document& doc, uint32_t root_token, std::string member_of, bool is_condition);
      void write(std::string& out, std::string stem, std::string member_of, const std::string& type_template);
};

class APIProperty : public APIEntry {
   public:
      std::string type;
      bool is_read_only = false;
      bool is_indexed   = false;

      size_t load(cobb::xml::document& doc, uint32_t root_token, std::string member_of);
      void write(std::string& out, std::string stem, std::string member_of, const std::string& type_template);
};

class APIAccessor : public APIEntry {
   public:
      std::string type;
      bool getter = false;
      bool setter = false;

      size_t load(cobb::xml::document& doc, uint32_t root_token, std::string member_of);
      void write(std::string& out, std::string stem, std::string member_of, const std::string& type_template);
};

class APIType : public APIEntry {
   protected:
      void _handle_member_nav(std::string& content, const std::string& stem);
      void _make_member_relationships_bidirectional();
      void _mirror_member_relationships(APIType& member_of, APIEntry& member, api_entry_type member_type);
      //
   public:
      std::filesystem::path    loaded_from;
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
      APIMethod* get_action_by_name(const std::string&);
      APIMethod* get_condition_by_name(const std::string&);
      APIProperty* get_property_by_name(const std::string&);
      APIAccessor* get_accessor_by_name(const std::string&);

      void load(cobb::xml::document& doc);
      void write(const std::string& article_template, const std::string& type_template);
};

class APINamespaceMember : public APIEntry {
   public:
      std::string type;
      bool is_indexed   = false;
      bool is_read_only = false;
      bool is_none      = false;

      size_t load(cobb::xml::document& doc, uint32_t root_token, std::string member_of);
      void write(std::string& out, std::string stem, std::string member_of, const std::string& type_template);
};

class APINamespace : public APIEntry {
   protected:
      void _handle_member_nav(std::string& content, const std::string& stem);
      void _make_member_relationships_bidirectional();
      void _mirror_member_relationships(APINamespace& member_of, APIEntry& member, api_entry_type member_type);
      //
   public:
      std::filesystem::path  loaded_from;
      std::string            friendly_name;
      std::vector<APIMethod> conditions;
      std::vector<APIMethod> actions;
      std::vector<APINamespaceMember> members;

      const char* get_friendly_name() const noexcept;
      APIMethod* get_action_by_name(const std::string&);
      APIMethod* get_condition_by_name(const std::string&);
      APINamespaceMember* get_member_by_name(const std::string&);

      void load(cobb::xml::document& doc);
      void write(const std::string& article_template, const std::string& type_template);
};

class APIRegistry {
   private:
      APIRegistry() {}
   public:
      static APIRegistry& get() {
         static APIRegistry instance;
         return instance;
      }
      ~APIRegistry();
      //
      std::filesystem::path root_path;
      std::vector<APIType*> types;
      std::vector<APINamespace*> namespaces;
      //
      void clear();
      APIType* get_type(const std::string& name);
      void load_type(const std::filesystem::path& source_file, cobb::xml::document& doc);
      void load_namespace(const std::filesystem::path& source_file, cobb::xml::document& doc);
      //
      int  depth_of(std::filesystem::path path);
      void make_stem(std::filesystem::path path, std::string& out);
};