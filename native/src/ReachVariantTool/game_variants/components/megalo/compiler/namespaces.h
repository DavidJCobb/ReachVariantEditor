#pragma once
#include <string>
#include <vector>
#include "types.h"
#include "../variables_and_scopes.h"
#include <QString>

namespace Megalo {
   class VariableScopeIndicatorValue;
   class VariableScopeWhichValue;
   //
   namespace Script {
      class Namespace;
      class NamespaceMember {
         public:
            static constexpr VariableScopeWhichValue*     no_which = nullptr;
            static constexpr VariableScopeIndicatorValue* no_scope = nullptr;
            //
         public:
            std::string name;
            const OpcodeArgTypeinfo& type;
            const VariableScopeWhichValue*     which = no_which;
            const VariableScopeIndicatorValue* scope = no_scope;
            //
            Namespace* owner = nullptr;
            //
            NamespaceMember(const char* n, const OpcodeArgTypeinfo& t, const VariableScopeWhichValue* w, const VariableScopeIndicatorValue* s) : name(n), type(t), which(w), scope(s) {}
            static NamespaceMember make_which_member(const char* n, const OpcodeArgTypeinfo& t, const VariableScopeWhichValue& w) {
               return NamespaceMember(n, t, &w, no_scope);
            }
            static NamespaceMember make_scope_member(const char* n, const OpcodeArgTypeinfo& t, const VariableScopeIndicatorValue& s) {
               return NamespaceMember(n, t, no_which, &s);
            }
            //
            inline bool is_which_member() const noexcept { return this->which != no_which; }
            inline bool is_scope_member() const noexcept { return this->scope != no_scope; }
            bool is_read_only() const noexcept;
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
            Namespace(const char* n, bool s, bool v, std::initializer_list<NamespaceMember> m) : name(n), can_have_statics(s), can_have_variables(v), members(m) {
               for (auto& member : this->members)
                  member.owner = this;
            }
            //
            const NamespaceMember* get_member(const QString& name) const noexcept;
      };
      namespace namespaces {
         extern Namespace unnamed;
         extern Namespace global;
         extern Namespace game;
         //
         extern std::array<Namespace*, 3> list;
         extern Namespace* get_by_name(const QString&);
      }
   }
}