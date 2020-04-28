#pragma once
#include <array>
#include <initializer_list>
#include <string>
#include <vector>
#include "limits_bitnumbers.h"
#include "opcode.h"
#include "opcode_arg.h"

// NOTE: conditions.cpp contains a preprocessor flag, MEGALO_DISALLOW_NONE_CONDITION, 
// which causes us to fail with an error if we encounter the "None" opcode. This flag 
// exists for development/debugging purposes and should not be enabled in release 
// builds.

namespace Megalo {
   class ConditionFunction : public OpcodeBase {
      public:
         const char* verb_normal = "is";
         const char* verb_invert = "is not";
         //
         ConditionFunction(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a,
            OpcodeFuncToScriptMapping mapping,
            const char* vn = "is",
            const char* vi = "is not"
         ) :
            OpcodeBase(n, d, f, a, mapping),
            verb_normal(vn),
            verb_invert(vi)
         {}
   };
   extern std::array<ConditionFunction, 18> conditionFunctionList;

   class Condition : public Opcode {
      //
      // NOTES:
      //
      // Bungie implemented "or"-linked conditions using the (or_group) field. The (or_group) 
      // field is set as follows: when compiling a Trigger, start a counter at 0. Every time 
      // you compile a condition, copy the counter into its (or_group) and then, if the 
      // condition is "and"-linked, increment the counter. Here's an example from Freeze Tag:
      //
      //    if  a -- 0
      //    and b -- 1
      //    then
      //       if  c -- 2
      //       or  d -- 2
      //       and e -- 3
      //       and f -- 4
      //       then
      //          ...
      //       end
      //    end
      //
      public:
         virtual ~Condition() {
            this->reset();
         }
         //
         #if _DEBUG
            std::string debug_str;
            uint32_t    bit_offset = 0;
         #endif
         bool     inverted =  false;
         condition_index or_group = 0;
         action_index    action   = 0; // execute before this action (cannot be none, which implies that a condition can't be the last opcode in a trigger)
         //
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer&) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out) noexcept override;
         virtual void reset() noexcept override;
         virtual Opcode* create_of_this_type() const noexcept override { return new Condition; }
         virtual Opcode* clone() const noexcept override;
   };
}
