#pragma once
#include <string>

namespace cobb {
   void sprintf(std::string& out, const char* format, ...);
   void sprintfp(std::string& out, const char* format, ...); // enables positional parameters i.e. %2$d // TODO: these are wrong: I think they assert if the buffer is too small, which isn't what we want
   int  strieq(const std::string& a, const std::string& b);
   //
   extern bool string_says_false(const char* str); // the string spells the case-insensitive word "false", ignoring whitespace
   extern bool string_to_int(const char* str, int32_t& out, bool allowHexOrDecimal = false); // returns true if it's a valid integer and no non-whitespace follows the number; out is not modified otherwise
   extern bool string_to_int(const char* str, uint32_t& out, bool allowHexOrDecimal = false);
   extern bool string_to_float(const char* str, float& out); // returns true if it's a valid float and no non-whitespace follows the number; out is not modified otherwise
}