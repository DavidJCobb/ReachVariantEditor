#pragma once
#include "base.h"
#include <vector>
#include "../../opcode_arg.h"

namespace Megalo {
   namespace Script {
      class VariableReference : public ParsedItem {
         public:
            QString content;
            #if !_DEBUG
               static_assert(false, "replace (content) with logic to actually parse parts");
            #endif
            //
            struct Part {
               QString name;
               QString index_str;
               int32_t index = -1;
               bool    index_is_numeric = false;
               //
               Part() {} // needed for std::vector::push_back
               Part(QString n, QString i) : name(n), index_str(i) {}
               Part(QString n, int32_t i) : name(n), index(i) {};
               //
               inline bool has_index() const noexcept {
                  return this->index_is_numeric || !this->index_str.isEmpty();
               }
            };
            struct InterpretedPart {
               const OpcodeArgTypeinfo* type = nullptr;
               int8_t  scope_id      = -1;
               int32_t disambiguator =  0; // which/index value; if type is nullptr and this is the first part, then the disambiguator is an integer constant value
            };
            //
            std::vector<Part> parts;
            std::vector<InterpretedPart> interpreted;
            struct {
               bool done = false;
               //
               const OpcodeArgTypeinfo* which_type = nullptr; // also used for namespace scope-members
               int16_t  which      = -1; // used for namespace which-members
               uint32_t scope      =  0; // used for namespace scope-members
               const OpcodeArgTypeinfo* var_type   = nullptr;
               int32_t  var_index  =  0;
               QString  property;        // must use a string and not a pointer to a definition, because Opcodes can serve as implicit property definitions
               int32_t  constant   =  0;
            } resolved;
            //
            VariableReference(QString);
            VariableReference(int32_t);
            //
            void resolve();
            //
            inline bool is_constant_integer() const noexcept { return this->parts.empty(); }
            //
         protected:
            inline Part* _part(int i) {
               if (i < 0)
                  i += this->parts.size();
               else if (i >= this->parts.size())
                  return nullptr;
               return &this->parts[i];
            }
      };
   }
}