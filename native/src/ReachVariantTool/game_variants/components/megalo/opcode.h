#pragma once
#include <cassert>
#include <string>
#include <vector>
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
   class OpcodeBase;

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
         OpcodeBase*  owner = nullptr;
         //
         OpcodeFuncToScriptMapping() {}
         OpcodeFuncToScriptMapping(mapping_type t, std::initializer_list<int8_t> args) : type(t) {
            size_t i = 0;
            for (auto it = args.begin(); it != args.end() && i < std::extent<decltype(arg_index_mappings)>::value; ++it, ++i) {
               this->arg_index_mappings[i] = *it;
            }
         }
         static OpcodeFuncToScriptMapping make_function(const char* pn, const char* sn, std::initializer_list<int8_t> args, int8_t ct = no_context, flags_type flags = 0) {
            auto instance = OpcodeFuncToScriptMapping(mapping_type::function, args);
            instance.primary_name   = pn;
            instance.secondary_name = sn;
            instance.arg_context    = ct;
            instance.flags          = flags;
            return instance;
         }
         static OpcodeFuncToScriptMapping make_getter(const char* pn, int8_t ct) {
            auto instance = OpcodeFuncToScriptMapping(mapping_type::property_get, {});
            instance.primary_name = pn;
            instance.arg_context  = ct;
            return instance;
         }
         static OpcodeFuncToScriptMapping make_setter(const char* pn, int8_t ct, int8_t op = no_argument, int8_t name = no_argument) {
            auto instance = OpcodeFuncToScriptMapping(mapping_type::property_set, {});
            instance.primary_name = pn;
            instance.arg_context  = ct;
            instance.arg_name     = name;
            instance.arg_operator = op;
            return instance;
         }
         static OpcodeFuncToScriptMapping make_intrinsic_assignment(int8_t op) {
            auto instance = OpcodeFuncToScriptMapping(mapping_type::assign, {});
            instance.arg_operator = op;
            return instance;
         }
         static OpcodeFuncToScriptMapping make_intrinsic_comparison(std::initializer_list<int8_t> args, int8_t op) {
            auto instance = OpcodeFuncToScriptMapping(mapping_type::compare, args);
            instance.arg_operator = op;
            return instance;
         }
   };

   class OpcodeBase {
      public:
         const char* name;
         const char* desc = "";
         const char* format;
         std::vector<OpcodeArgBase> arguments;
         OpcodeFuncToScriptMapping mapping;
         //
         OpcodeBase(
            const char* n,
            const char* d,
            const char* f,
            std::initializer_list<OpcodeArgBase> a,
            OpcodeFuncToScriptMapping m
         ) :
            name(n),
            desc(d),
            format(f),
            arguments(a),
            mapping(m)
         {
            this->mapping.owner = this;
         }
         //
         void decompile(Decompiler& out, std::vector<OpcodeArgValue*>& args) const noexcept;
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