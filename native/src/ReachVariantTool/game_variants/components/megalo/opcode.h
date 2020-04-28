#pragma once
#include <cassert>
#include <string>
#include <vector>
#include "../../../helpers/bitwriter.h"
#include "../../../helpers/refcounting.h"
#include "../../../helpers/stream.h"
#include "../../../helpers/strings.h"
#include "opcode_arg.h"
#include "variables_and_scopes.h"
#include "decompiler/decompiler.h"
#include <QString>

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
            property_set, // the opcode should be modified as (arg_context.primary_name = argument); only use this when any assign operator (e.g. +=, -=) is allowed; every property_get has a property_set but not every property_set has a property_get
         };
         //
         struct flags {
            flags() = delete;
            enum type : uint8_t {
               secondary_property_zeroes_result = 0x01, // if set, invoking this opcode via its second name sets the result argument to no_object/no_player/no_team/etc. before calling; this is the difference between try_get_killer() and get_killer(). only valid for function-type opcodes with variable-type out-arguments
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
         std::string  primary_name;   // for functions this is NOT required to be unique; we DO support function overloads; the name "send_incident" is shared by two different actions
         std::string  secondary_name; // an alternate name through which the opcode can be invoked; note: currently, secondary names are NOT supported for properties
         int8_t       arg_context  = no_context;  // if non-negative, refers to one of the arguments and indicates that the opcode function is a member of that argument type
         int8_t       arg_name     = no_argument; // only used by property mappings, and only if (primary_name) is blank; indicates that one of the arguments (which must be an enum type) should be treated as a property name; needed for "modify grenade count" action
         int8_t       arg_operator = no_argument; // for comparisons, assignments, and property setters, this indicates which argument is the operator
         int8_t       arg_index_mappings[8] = { no_argument, no_argument, no_argument, no_argument, no_argument, no_argument, no_argument, no_argument }; // map native argument order to script argument order
         flags_type   flags = 0;
         OpcodeBase*  owner = nullptr;
         variable_scope double_context_type = variable_scope::not_a_scope; // use when the opcode is (non_global_scope.var.opcode()) where the types of both the scope and the variable are significant
         //
         uint8_t mapped_arg_count() const noexcept {
            for (uint8_t i = 0; i < std::extent<decltype(arg_index_mappings)>::value; ++i)
               if (this->arg_index_mappings[i] == no_argument)
                  return i;
            return std::extent<decltype(arg_index_mappings)>::value;
         }
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
         static OpcodeFuncToScriptMapping make_doubly_contextual_call(const char* pn, const char* sn, std::initializer_list<int8_t> args, variable_scope dc, int8_t ct = no_context, flags_type flags = 0) {
            auto instance = OpcodeFuncToScriptMapping::make_function(pn, sn, args, ct, flags);
            instance.double_context_type = dc;
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
         bool context_is(const OpcodeArgTypeinfo&) const noexcept;
         void decompile(Decompiler& out, std::vector<OpcodeArgValue*>& args) const noexcept;
         const OpcodeArgTypeinfo* get_context_type() const noexcept;
         const OpcodeArgTypeinfo* get_name_type() const noexcept;
         int  index_of_operand_argument() const noexcept; // only returns a sensible result for setter-accessors
         int  index_of_out_argument() const noexcept; // returns -1 if there is none
   };

   class Opcode { // base class for Condition and Action
      public:
         virtual ~Opcode() {
            this->reset();
         }
         virtual bool read(cobb::ibitreader&, GameVariantDataMultiplayer&) noexcept = 0;
         virtual void write(cobb::bitwriter& stream) const noexcept = 0;
         virtual void to_string(std::string& out) const noexcept = 0;
         virtual void decompile(Decompiler& out) noexcept = 0;
         virtual void reset() noexcept;
         virtual Opcode* create_of_this_type() const noexcept = 0;
         virtual Opcode* clone() const noexcept;
         //
         std::vector<OpcodeArgValue*> arguments;
         const OpcodeBase* function = nullptr;
         //
         arg_compile_result compile_argument(Compiler&, Script::string_scanner&,    uint8_t arg_index, uint8_t part) noexcept; // creates the argument if it doesn't exist
         arg_compile_result compile_argument(Compiler&, Script::VariableReference&, uint8_t arg_index, uint8_t part) noexcept; // creates the argument if it doesn't exist
   };

   class AccessorRegistry {
      public:
         static AccessorRegistry& get() {
            static AccessorRegistry instance;
            return instance;
         }
         //
         struct Definition {
            std::string name; // will be blank if the property's name is determined by an opcode argument (see OpcodeFuncToScriptMapping::arg_name above)
            const OpcodeBase* getter = nullptr;
            const OpcodeBase* setter = nullptr;
            //
            Definition() {}
            Definition(const char* n) : name(n) {}
            Definition(const std::string& n) : name(n) {}
            //
            const OpcodeBase* get_opcode_base() const noexcept; // returns getter or setter -- whichever is defined, preferring the former
            inline bool is_variably_named() const noexcept { return this->name.empty(); }
         };
         std::vector<Definition> definitions;
         //
         Definition* get_by_name(const char*, const OpcodeArgTypeinfo& base) const noexcept;
         Definition* get_variably_named_accessor(const OpcodeArgTypeinfo& property_name_type, const OpcodeArgTypeinfo& base) const noexcept;
         const Definition* get_variably_named_accessor(Compiler&, const QString& name, const OpcodeArgTypeinfo& base) const noexcept;
         //
      protected:
         AccessorRegistry();
   };
}