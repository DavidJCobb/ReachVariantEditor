#pragma once
#include <string>
#include <vector>
#include "types.h"
#include "../variables_and_scopes.h"
#include <QString>

namespace Megalo {
   namespace Script {
      class Namespace;
      class NamespaceMember {
         public:
            struct flag {
               flag() = delete;
               enum type : uint8_t {
                  none = 0,
                  is_none      = 0x01,
                  is_read_only = 0x02, // needed for members that consist only of a (which)
               };
            };
            using flags_t = std::underlying_type_t<flag::type>;
            //
            static constexpr int8_t  no_which = 0;
            static constexpr int16_t no_scope = -1;
            //
         public:
            std::string name;
            const OpcodeArgTypeinfo& type;
            flags_t     flags = flag::none;
            uint32_t    which = no_which; // signature
            int16_t     scope_indicator_id = no_scope;
            //
            Namespace* owner = nullptr;
            //
            NamespaceMember(const char* n, const OpcodeArgTypeinfo& t, int8_t w, int16_t s, flags_t fl = flag::none) : name(n), type(t), which(w), scope_indicator_id(s), flags(fl) {}
            static NamespaceMember make_which_member(const char* n, const OpcodeArgTypeinfo& t, int8_t w, flags_t fl = flag::none) {
               return NamespaceMember(n, t, w, no_scope, fl);
            }
            static NamespaceMember make_scope_member(const char* n, const OpcodeArgTypeinfo& t, int16_t s) {
               return NamespaceMember(n, t, no_which, s);
            }
            //
            inline bool is_which_member() const noexcept { return this->which != no_which; }
            inline bool is_scope_member() const noexcept { return this->scope_indicator_id != no_scope; }
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