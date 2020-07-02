#pragma once
#include "base.h"
#include "variable_reference.h"
#include <QString>

namespace Megalo {
   class OpcodeArgTypeinfo;
   namespace Script {
      class Enum;
      //
      class Alias : public ParsedItem {
         protected:
            bool _validate_name_initial(Compiler&);
            bool _validate_name_final(Compiler&);
            //
         public:
            QString name;
            QString target_imported_name; // for imported names only
            VariableReference* target = nullptr; // for variable-references only, including relative aliases; owned by this Alias and deleted in its destructor
            bool invalid = true;
            //
            Alias(Compiler&, QString name, QString target);
            Alias(Compiler&, QString name, int32_t target);
            ~Alias();
            //
            bool    is_enumeration() const noexcept;
            const Enum* get_enumeration() const noexcept;
            bool    is_integer_constant()  const noexcept;
            int32_t get_integer_constant() const noexcept;
            bool    is_relative_alias() const noexcept;
            bool    is_imported_name() const noexcept;
            const OpcodeArgTypeinfo* get_basis_type() const noexcept;
      };
   }
}