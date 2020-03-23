#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"
#include "variables/object.h"

//
// TODO: "paired_variable.h" would probably be a better name for this file
//

namespace Megalo {
   class OpcodeArgValueContextualVariableBaseClass : public OpcodeArgValue {
      public:
         OpcodeArgValueContextualVariableBaseClass(variable_scope scope, variable_type type) : baseScope(scope), baseType(type) {}
         //
         variable_scope baseScope;
         variable_type  baseType;
         int32_t        index = -1; // loaded argument value: which variable (e.g. for an object.number variable, which number)
         //
         inline bool is_none() const noexcept { return this->index == -1; }
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
   class OpcodeArgValueObjectTimerVariable : public OpcodeArgValueContextualVariableBaseClass {
      //
      // The index of a timer variable scoped to any object. Typically, the object in question is 
      // determined contextually, such as by another argument to the opcode containing this argument.
      //
      public:
         OpcodeArgValueObjectTimerVariable() : OpcodeArgValueContextualVariableBaseClass(variable_scope::object, variable_type::timer) {}
         //
         static OpcodeArgTypeinfo typeinfo;
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueObjectTimerVariable;
         }
   };

   class OpcodeArgValueObjectPlayerVariable : public OpcodeArgValue {
      //
      // An object variable and the index of a player variable scoped to that object.
      //
      public:
         static OpcodeArgTypeinfo typeinfo;
         static OpcodeArgValue* factory(cobb::ibitreader&) {
            return new OpcodeArgValueObjectPlayerVariable();
         }
         //
      public:
         OpcodeArgValueObject object;
         int32_t playerIndex = -1;
         //
         virtual bool read(cobb::ibitreader& stream) noexcept override;
         virtual void write(cobb::bitwriter& stream) const noexcept override;
         virtual void to_string(std::string& out) const noexcept override;
   };
}