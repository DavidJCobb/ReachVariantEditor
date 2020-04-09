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
         //
         // What is a namespace member? Well, it's a member of one of the namespaces defined our gametype scripting language, 
         // obviously. But what does being a namespace member entail? Well, there are two kinds of namespace members.
         //
         // A "which" member represents a single top-level value described by a "which" value -- so, a top-level object, 
         // player, or team, then. Some of these values enable access to nested variables, like current_object, others, like 
         // no_object, don't, for reasons that are hopefully self-evident.
         //
         // A "scope" member doesn't necessarily represent a single top-level value, but rather a single value. Variables 
         // consist of a "scope" and, depending on that scope, a "which" and an "index," yes? Well, a "scope" member refers 
         // not to a "scope" in isolation, but to an entire variable value consisting solely of a scope. If a script refers 
         // to a "scope" member, then, it's referring to an entire value, whereas a "which" member can be an entire value 
         // (access to a top-level variable) or part of a value (access to a variable, property, or accessor nested under 
         // that top-level variable).
         //
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