#pragma once
#include "../opcode_arg.h"
#include "../variables_and_scopes.h"

namespace Megalo {
   class OpcodeArgValueSpecificVariableScopeAndType : public OpcodeArgValue {
      public:
         OpcodeArgValueSpecificVariableScopeAndType(variable_scope scope, variable_type type) : baseScope(scope), baseType(type) {}
         //
         variable_scope baseScope;
         variable_type  baseType;
         uint32_t       index = 0; // loaded argument value: which variable (e.g. for an object.number variable, which number)
         //
         virtual bool read(cobb::bitstream& stream) noexcept override {
            auto scope = getScopeObjectForConstant(this->baseScope);
            this->index = stream.read_bits(scope.index_bits(this->baseType));
            return true;
         }
         virtual void to_string(std::string& out) const noexcept override {
            const char* type  = "INVALID";
            switch (this->baseType) {
               case variable_type::object:
                  type = "Object";
                  break;
               case variable_type::player:
                  type = "Player";
                  break;
               case variable_type::scalar:
                  type = "Number";
                  break;
               case variable_type::team:
                  type = "Team";
                  break;
               case variable_type::timer:
                  type = "Timer";
                  break;
            }
            cobb::sprintf(out, "the object in question's %s[%u] variable", type, this->index);
         }
   };
   //
   #define megalo_make_specific_variable_type(classname, scope, type) \
      class classname : public OpcodeArgValueSpecificVariableScopeAndType { \
         public: \
            classname##() : OpcodeArgValueSpecificVariableScopeAndType( scope, type ) {} \
            static OpcodeArgValue* factory(cobb::bitstream&) { \
               return new classname##(); \
            } \
      };

   megalo_make_specific_variable_type(OpcodeArgValueObjectTimerVariable, variable_scope::object, variable_type::timer);
}