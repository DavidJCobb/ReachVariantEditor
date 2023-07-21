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
      class Enum;
      class Namespace;
      //
      class NamespaceMember {
         //
         // What is a namespace member? Well, it's a member of one of the namespaces defined our gametype scripting language, 
         // obviously. But what does being a namespace member entail? Well, there are four kinds of namespace members. All 
         // represent top-level values -- that is, values that aren't nested under any other object.
         //
         // A "which" member represents a single top-level value described by a "which" value -- so, a top-level object, 
         // player, or team, then. Some of these values enable access to nested variables, like current_object, others, like 
         // no_object, don't, for reasons that are hopefully self-evident.
         //
         // A "scope" member represents a single top-level value described by a "scope" value and, potentially, an index. 
         // This includes things like (game.round_timer) and (script_option[3]).
         //
         // A "bare" member represents a single top-level value described solely by its type. As of this writing, bare members 
         // literally only exist to facilitate expressions of the form (all_players.score += 5), where (all_players) is the 
         // member and is of type OpcodeArgValuePlayerOrGroup. That type's (compile) overload will be expected to check for 
         // the bare member and react appropriately.
         //
         // An "enum" member represents an enum containing integer constant values.
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
            const Enum* enumeration = nullptr;
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
            static NamespaceMember make_bare_member(const char* n, const OpcodeArgTypeinfo& t) {
               return NamespaceMember(n, t, no_which, no_scope);
            }
            static NamespaceMember make_enum_member(Namespace&, Enum& e); // passing the containing namespace is an unfortunate necessity here
            //
            bool has_index() const noexcept;
            constexpr bool is_which_member() const noexcept { return this->which != no_which; }
            constexpr bool is_scope_member() const noexcept { return this->scope != no_scope; }
            constexpr bool is_bare_member() const noexcept { return !this->is_which_member() && !this->is_scope_member(); }
            constexpr bool is_enum_member() const noexcept { return this->enumeration != nullptr; }
            bool is_read_only() const noexcept;
      };
      class Namespace {
         public:
            std::string name;
            bool can_have_statics   = false; // only true for unnamed namespace; determines whether you can refer to absolute players/teams e.g. player[2]
            bool can_have_variables = false; // only true for global and temporaries namespaces
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
         extern Namespace enums;
         extern Namespace temporaries; // added in MCC (backported from H4)
         //
         extern std::array<Namespace*, 5> list;
         extern Namespace* get_by_name(const QString&);
      }
   }
}