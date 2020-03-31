#pragma once
#include <string>
#include <vector>
#include "types.h"
#include "../variables_and_scopes.h"

namespace Megalo {
   namespace Script {
      class Namespace;
      class NamespaceMember {
         public:
            std::string name;
            bool        is_read_only = false;
            bool        is_none      = false;
            //
            Namespace* owner = nullptr;
      };
      class Namespace {
         public:
            std::string name;
            bool can_have_statics   = false; // only true for unnamed namespace; determines whether you can refer to absolute players/teams e.g. player[2]
            bool can_have_variables = false; // only true for global namespace
            std::vector<NamespaceMember> members;
            //
            Namespace() {}
            Namespace(const char* n, bool s, bool v) : name(n), can_have_statics(s), can_have_variables(v) {}
            Namespace(const char* n, bool s, bool v, std::initializer_list<NamespaceMember> m) : name(n), can_have_statics(s), can_have_variables(v), members(m) {}
      };
      namespace namespaces {
         extern Namespace unnamed;
         extern Namespace global;
         extern Namespace game;
      }
   }
}