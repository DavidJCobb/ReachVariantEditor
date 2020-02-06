#pragma once
#include <cassert>
#include <string>
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/reference_tracked_object.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/strings.h"
#include "enums.h"
#include "opcode_arg.h"
#include "variables_and_scopes.h"
#include "decompiler/decompiler.h"

class GameVariantDataMultiplayer;

namespace Megalo {
   class OpcodeFuncToScriptMapping {
      public:
         enum class mapping_type {
            none,         // only used for the "none" opcodes
            assign,       // only used for the "modify  variable" opcode; special-case to be handled by the (de)compiler
            compare,      // only used for the "compare variable" opcode; special-case to be handled by the (de)compiler
            function,     // the opcode is a function (member or non-member)
            property_get, // the opcode should be accessed as (result = arg_context.primary_name);   only use this when there is a corresponding (property_set) opcode
            property_set, // the opcode should be modified as (arg_context.primary_name = argument); only use this when any assign operator (e.g. +=, -=) is allowed
         };
         //
         struct flags {
            flags() = delete;
            enum type : uint8_t {
               secondary_property_zeroes_result = 0x01, // if set, invoking this opcode via its second name sets the result argument to no_object/no_player/no_team/etc. before calling; this is the difference between try_get_killer() and get_killer()
            };
         };
         using flags_type = std::underlying_type_t<flags::type>;
         //
         // Values for (context_arg):
         static constexpr int8_t no_context     = -1;
         static constexpr int8_t game_namespace = -2; // e.g. "game.end_round()"
         //
         static constexpr int8_t no_argument = -1;
         //
      public:
         mapping_type type = mapping_type::none;
         std::string  primary_name;
         std::string  secondary_name; // an alternate name through which the opcode can be invoked
         int8_t       arg_context  = no_context;  // if non-negative, refers to one of the arguments and indicates that the opcode function is a member of that argument type
         int8_t       arg_name     = no_argument; // only used by property mappings, and only if (primary_name) is blank; indicates that one of the arguments (which must be an enum type) should be treated as a property name
         int8_t       arg_operator = no_argument; // for comparisons, assignments, and property setters, this indicates which argument is the operator
         int8_t       arg_index_mappings[8] = { no_argument, no_argument, no_argument, no_argument, no_argument, no_argument, no_argument, no_argument }; // map native argument order to script argument order
         flags_type   flags = 0;
         //
         // TODO: This class needs to be able to access its owning opcode-function, so that it can access the 
         // argument list (not only to see the types of the function's arguments, but also to identify the 
         // out-argument if any exists). This requires:
         //
         //  - ...that ConditionFunction and ActionFunction inherit from a common base class, and share the 
         //    argument lists.
         //
         //  - ...that this class have an OpcodeFunction* member, which we'll name (owner).
         //
         //  - ...that the OpcodeFunction constructor set (this->mapping->owner = this).
         //
         // TODO: Constructors.
         //
   };

   class OpcodeBase {
      public:
         const char* name;
         const char* desc = "";
         const char* format;
         std::vector<OpcodeArgBase> arguments;
         //
         OpcodeBase(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a
         ) :
            name(n),
            desc(d),
            format(f),
            arguments(a)
         {}
   };

   class Opcode { // base class for Condition and Action
      public:
         virtual bool read(cobb::ibitreader&) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void postprocess(GameVariantDataMultiplayer* newlyLoaded) noexcept = 0;
         virtual void decompile(Decompiler& out) noexcept { // override me!
            out.write(u8"OPCODE");
         };
   };
}