#include "opcode_arg.h"
#include "variables_and_scopes.h"
#include "opcode_arg_types/variables/all_core.h"
#include "../../errors.h"
#include "../../../helpers/qt/string.h"

namespace Megalo {
   void OpcodeArgTypeRegistry::register_type(const OpcodeArgTypeinfo& type) {
      this->types.push_back(&type);
   }
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_by_internal_name(const QString& name) const {
      for (auto type : this->types)
         if (cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
}