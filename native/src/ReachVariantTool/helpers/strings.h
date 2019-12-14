#pragma once
#include <string>

namespace cobb {
   void sprintf(std::string& out, const char* format, ...);
   int  strieq(const std::string& a, const std::string& b);
}