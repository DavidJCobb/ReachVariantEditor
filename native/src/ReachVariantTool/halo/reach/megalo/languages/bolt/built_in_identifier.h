#pragma once

namespace halo::reach::megalo::bolt {
   // Definition for a built-in identifier.
   class built_in_identifier {
      public:
         const char* name = nullptr;

         consteval built_in_identifier() {}
         consteval built_in_identifier(const char* n) : name(n) {}
   };
}