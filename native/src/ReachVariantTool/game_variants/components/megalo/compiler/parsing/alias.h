#pragma once
#include "base.h"
#include "variable_reference.h"
#include <QString>

namespace Megalo {
   namespace Script {
      class Alias : public ParsedItem {
         public:
            QString name;
            QString target_imported_name; // for imported names only
            VariableReference* target = nullptr; // for variable-references only, including relative aliases
            //
            Alias(Compiler&, QString name, QString target);
            //
            bool    is_integer_constant()  const noexcept;
            int32_t get_integer_constant() const noexcept;
            bool    is_relative_alias() const noexcept;
            bool    is_imported_name() const noexcept;
      };
   }
}