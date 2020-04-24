#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"
#include "variables/object.h"

//
// TODO: "paired_variable.h" would probably be a better name for this file
//

namespace Megalo {
   class OpcodeArgValueObjectTimerVariable : public OpcodeArgValue {
      //
      // The index of a timer variable scoped to any object. Typically, the object in question is 
      // determined contextually, such as by another argument to the opcode containing this argument.
      //
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
         variable_scope baseScope;
         variable_type  baseType;
         int32_t        index = -1; // loaded argument value: which variable (e.g. for an object.number variable, which number)
         //
         inline bool is_none() const noexcept { return this->index == -1; }
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;
   };

   class OpcodeArgValueObjectPlayerVariable : public OpcodeArgValue {
      //
      // An object variable and the index of a player variable scoped to that object.
      //
      public:
         static OpcodeArgTypeinfo typeinfo;
         //
      public:
         OpcodeArgValueObject object;
         int32_t playerIndex = -1;
         //
         virtual bool read(cobb::ibitreader& stream, GameVariantDataMultiplayer& mp) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
         virtual void decompile(Decompiler& out, Decompiler::flags_t flags = Decompiler::flags::none) noexcept override;

         //
         // This argument is used in only one place: the opcode to set an object's shape color to match the 
         // armor color of the player referred to by one of the object's nested player variables. That opcode 
         // uses this type as its sole argument, but specifies the argument as both the context and a mapped 
         // argument, i.e.
         //
         //    same_arg.opcode(same_arg)
         //
         // This means that "the same" argument will be parsed twice, and will be passed to this class's 
         // (compile) function twice. The trick is that a different overload wil be used in each case: the 
         // context will be passed as a VariableReference, and the argument as a string.
         //
         virtual arg_compile_result compile(Compiler&, Script::string_scanner&, uint8_t part) noexcept override;
         virtual arg_compile_result compile(Compiler&, Script::VariableReference&, uint8_t part) noexcept override;
   };
}