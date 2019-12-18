#pragma once
#include <string>

namespace cobb {
   void sprintf(std::string& out, const char* format, ...);
   void sprintfp(std::string& out, const char* format, ...); // enables positional parameters i.e. %2$d // TODO: these are wrong: I think they assert if the buffer is too small, which isn't what we want
   int  strieq(const std::string& a, const std::string& b);
}