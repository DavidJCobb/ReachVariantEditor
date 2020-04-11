#include "opcode_arg.h"
#include "variables_and_scopes.h"
#include "opcode_arg_types/variables/all_core.h"
#include "../../errors.h"
#include "../../../helpers/qt/string.h"
#include "../../../formats/detailed_enum.h"
#include "../../../formats/detailed_flags.h"

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
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_static_indexable_type(const QString& name) const {
      for (auto type : this->types)
         if (type->can_be_static() && cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
   const OpcodeArgTypeinfo* OpcodeArgTypeRegistry::get_variable_type(const QString& name) const {
      for (auto type : this->types)
         if (type->is_variable() && cobb::qt::stricmp(name, type->internal_name) == 0)
            return type;
      return nullptr;
   }
   void OpcodeArgTypeRegistry::lookup_imported_name(const QString& name, type_list_t& out) const {
      out.clear();
      for (auto type : this->types) {
         auto& imp = type->imported_names;
         auto  e   = imp.enum_values;
         auto  f   = imp.flag_values;
         if (e && e->lookup(name)) {
            out.push_back(type);
            continue;
         }
         if (f && f->lookup(name)) {
            out.push_back(type);
            continue;
         }
      }
   }

   const Script::Property* OpcodeArgTypeinfo::get_property_by_name(QString name) const {
      name = name.toLower();
      for (auto& prop : this->properties)
         if (cobb::qt::stricmp(name, prop.name) == 0)
            return &prop;
      return nullptr;
   }
}