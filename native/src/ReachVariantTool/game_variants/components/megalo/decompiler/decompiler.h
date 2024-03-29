#pragma once
#include "../../../base.h"
#include <cstdint>
#include <string>
#include <QString>

namespace Megalo {
   class Decompiler {
      public:
         using string_type = QString;
         using char_type   = QChar;
         //
         struct flags {
            flags() = delete;
            enum type : uint64_t {
               none = 0,
               //
               is_call_context = 0x000000000000001,
            };
         };
         using flags_t = std::underlying_type_t<flags::type>;
         //
      protected:
         string_type  current_indent;
         uint8_t      indent_size = 3;
         GameVariant& variant;
         //
      public:
         Decompiler(GameVariant& v) : variant(v) {};
         //
         string_type current_content;
         //
         inline uint8_t get_indent_size() const noexcept { return this->indent_size; }
         inline void set_indent_size(uint8_t spaces) noexcept { this->indent_size = spaces; } // TOOD: fail if decompiling is in progress
         //
         void decompile();
         //
         // Code below is provided to the objects being decompiled:
         //
         void modify_indent_count(int16_t nesting) noexcept;
         //
         void write(const string_type& content);
         void write(const std::string& content);
         void write(const char* content);
         void write(char_type c);
         void write_line(const string_type& content); // writes `\n${current_indent}${content}`
         void write_line(const std::string& content); // writes `\n${current_indent}${content}`
         void write_line(const char*  content); // writes `\n${current_indent}${content}`
         void write_string_literal(const char*  content); // content is taken as UTF-8
         void write_string_literal(std::string& content); // content is taken as UTF-8
         //
         inline GameVariant& get_variant() { return this->variant; }
   };
}