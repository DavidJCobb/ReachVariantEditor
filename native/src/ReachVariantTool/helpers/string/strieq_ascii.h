#pragma once
#include <string>

namespace cobb {
   // check if two strings are equal, using case-insensitivity for ASCII glyphs
   extern int strieq_ascii(const std::string& a, const std::string& b);
}