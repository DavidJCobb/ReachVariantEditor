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
   bool OpcodeArgTypeinfo::has_imported_name(const QString& name) const noexcept {
      for (auto p : this->imported_names.bare_values)
         if (name.compare(p, Qt::CaseInsensitive) == 0)
            return true;
      if (auto e = this->imported_names.enum_values) {
         auto i = e->lookup(name);
         if (i >= 0)
            return true;
      }
      if (auto f = this->imported_names.flag_values) {
         auto i = f->lookup(name);
         if (i >= 0)
            return true;
      }
      return false;
   }

   void OpcodeArgTypeRegistry::lookup_imported_name(const QString& name, type_list_t& out) const {
      out.clear();
      for (auto type : this->types)
         if (type->has_imported_name(name))
            out.push_back(type);
   }

   const Script::Property* OpcodeArgTypeinfo::get_property_by_name(QString name) const {
      name = name.toLower();
      for (auto& prop : this->properties)
         if (cobb::qt::stricmp(name, prop.name) == 0)
            return &prop;
      return nullptr;
   }
   bool OpcodeArgTypeinfo::has_accessor_proxy_type(const OpcodeArgTypeinfo& type) const noexcept {
      for (auto t : this->accessor_proxy_types)
         if (t == &type)
            return true;
      return false;
   }

   /*static*/ arg_compile_result arg_compile_result::failure() {
      return arg_compile_result(code_t::failure);
   }
   /*static*/ arg_compile_result arg_compile_result::failure(QString error) {
      auto result = arg_compile_result(code_t::failure);
      result.error = error;
      return result;
   }
   /*static*/ arg_compile_result arg_compile_result::success() {
      return arg_compile_result(code_t::success);
   }
   /*static*/ arg_compile_result arg_compile_result::unresolved_string(QString content) {
      auto result = arg_compile_result(code_t::unresolved_string);
      result.error = content;
      return result;
   }
}